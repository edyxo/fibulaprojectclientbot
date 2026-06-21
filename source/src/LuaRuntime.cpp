#include "LuaRuntime.h"

#include "creature.h"
#include "engine.h"
#include "game.h"
#include "map.h"

extern "C"
{
#include "third_party/lua54/lauxlib.h"
#include "third_party/lua54/lua.h"
#include "third_party/lua54/lualib.h"
}

extern Engine g_engine;
extern Game g_game;
extern Map g_map;

namespace
{
	constexpr size_t LUA_INSTRUCTION_LIMIT = 1000000;
	constexpr int LUA_HOOK_INTERVAL = 1000;

	LuaRuntime* getRuntime(lua_State* state)
	{
		return *static_cast<LuaRuntime**>(lua_getextraspace(state));
	}

	std::string getSafeLuaValue(lua_State* state, int index)
	{
		switch(lua_type(state, index))
		{
			case LUA_TNIL:
				return "nil";
			case LUA_TBOOLEAN:
				return (lua_toboolean(state, index) ? "true" : "false");
			case LUA_TNUMBER:
			case LUA_TSTRING:
			{
				size_t length = 0;
				const char* text = lua_tolstring(state, index, &length);
				return (text ? std::string(text, length) : std::string());
			}
			default:
				return std::string("<").append(luaL_typename(state, index)).append(">");
		}
	}

	void setIntegerField(lua_State* state, const char* name, lua_Integer value)
	{
		lua_pushinteger(state, value);
		lua_setfield(state, -2, name);
	}

	void setStringField(lua_State* state, const char* name, const std::string& value)
	{
		lua_pushlstring(state, value.c_str(), value.length());
		lua_setfield(state, -2, name);
	}
}

LuaRuntime::~LuaRuntime()
{
	if(m_state)
		lua_close(m_state);
}

void* LuaRuntime::allocate(void* userData, void* pointer, size_t oldSize, size_t newSize)
{
	MemoryState* memory = static_cast<MemoryState*>(userData);
	size_t trackedOldSize = (pointer ? oldSize : 0);
	if(newSize == 0)
	{
		free(pointer);
		memory->used -= (trackedOldSize <= memory->used ? trackedOldSize : memory->used);
		return nullptr;
	}

	if(newSize > trackedOldSize && memory->used + (newSize - trackedOldSize) > memory->limit)
		return nullptr;

	void* resized = realloc(pointer, newSize);
	if(resized)
		memory->used = memory->used - (trackedOldSize <= memory->used ? trackedOldSize : memory->used) + newSize;
	return resized;
}

bool LuaRuntime::initialize(std::string& error)
{
	if(m_state)
		return true;

	m_state = lua_newstate(&LuaRuntime::allocate, &m_memory);
	if(!m_state)
	{
		error = "Unable to allocate the Lua state.";
		return false;
	}

	*static_cast<LuaRuntime**>(lua_getextraspace(m_state)) = this;

	// The terminal intentionally omits file, process, package, debug, and
	// coroutine libraries. It is a bounded client console, not a shell.
	luaL_requiref(m_state, LUA_GNAME, luaopen_base, 1);
	lua_pop(m_state, 1);
	luaL_requiref(m_state, LUA_TABLIBNAME, luaopen_table, 1);
	lua_pop(m_state, 1);
	luaL_requiref(m_state, LUA_STRLIBNAME, luaopen_string, 1);
	lua_pop(m_state, 1);
	luaL_requiref(m_state, LUA_MATHLIBNAME, luaopen_math, 1);
	lua_pop(m_state, 1);
	luaL_requiref(m_state, LUA_UTF8LIBNAME, luaopen_utf8, 1);
	lua_pop(m_state, 1);

	lua_pushnil(m_state);
	lua_setglobal(m_state, "dofile");
	lua_pushnil(m_state);
	lua_setglobal(m_state, "loadfile");
	lua_pushnil(m_state);
	lua_setglobal(m_state, "collectgarbage");

	lua_pushcfunction(m_state, &LuaRuntime::luaPrint);
	lua_setglobal(m_state, "print");

	lua_newtable(m_state);
	lua_pushcfunction(m_state, &LuaRuntime::clientLog);
	lua_setfield(m_state, -2, "log");
	lua_pushcfunction(m_state, &LuaRuntime::clientSay);
	lua_setfield(m_state, -2, "say");
	lua_pushcfunction(m_state, &LuaRuntime::clientStatus);
	lua_setfield(m_state, -2, "status");
	lua_pushcfunction(m_state, &LuaRuntime::clientHelp);
	lua_setfield(m_state, -2, "help");
	lua_setglobal(m_state, "client");
	return true;
}

void LuaRuntime::appendOutput(const std::string& text)
{
	if(!m_currentOutput)
		return;

	std::string bounded = text;
	if(bounded.length() > 2048)
		bounded.resize(2048);
	m_currentOutput->push_back(std::move(bounded));
}

void LuaRuntime::instructionHook(lua_State* state, lua_Debug*)
{
	LuaRuntime* runtime = getRuntime(state);
	runtime->m_instructionCount += LUA_HOOK_INTERVAL;
	if(runtime->m_instructionCount > LUA_INSTRUCTION_LIMIT)
		luaL_error(state, "instruction limit exceeded");
}

int LuaRuntime::luaPrint(lua_State* state)
{
	LuaRuntime* runtime = getRuntime(state);
	std::string line;
	int count = lua_gettop(state);
	for(int index = 1; index <= count; ++index)
	{
		if(index > 1)
			line.push_back('\t');
		line.append(getSafeLuaValue(state, index));
	}
	runtime->appendOutput(line);
	return 0;
}

int LuaRuntime::clientLog(lua_State* state)
{
	size_t length = 0;
	const char* text = luaL_checklstring(state, 1, &length);
	getRuntime(state)->appendOutput(std::string(text, length));
	return 0;
}

int LuaRuntime::clientSay(lua_State* state)
{
	if(!g_engine.isIngame())
		return luaL_error(state, "client is not in game");

	size_t length = 0;
	const char* text = luaL_checklstring(state, 1, &length);
	if(length == 0 || length > 255)
		return luaL_error(state, "message length must be 1-255 bytes");

	for(size_t index = 0; index < length; ++index)
	{
		if(static_cast<unsigned char>(text[index]) < 0x20)
			return luaL_error(state, "message contains control characters");
	}

	g_game.sendSay(MessageSay, 0, std::string(), std::string(text, length));
	lua_pushboolean(state, 1);
	return 1;
}

int LuaRuntime::clientStatus(lua_State* state)
{
	lua_newtable(state);
	lua_pushboolean(state, g_engine.isIngame() ? 1 : 0);
	lua_setfield(state, -2, "ingame");

	if(!g_engine.isIngame())
		return 1;

	setStringField(state, "name", g_game.getPlayerName());
	setIntegerField(state, "level", g_game.getPlayerLevel());
	setIntegerField(state, "health", g_game.getPlayerHealth());
	setIntegerField(state, "maxHealth", g_game.getPlayerMaxHealth());
	setIntegerField(state, "mana", g_game.getPlayerMana());
	setIntegerField(state, "maxMana", g_game.getPlayerMaxMana());

	Creature* localCreature = g_map.getLocalCreature();
	if(g_engine.hasShowDebugCoordinates() && localCreature)
	{
		Position& position = localCreature->getCurrentPosition();
		setIntegerField(state, "x", position.x);
		setIntegerField(state, "y", position.y);
		setIntegerField(state, "z", position.z);
	}
	return 1;
}

int LuaRuntime::clientHelp(lua_State* state)
{
	lua_pushliteral(state, "client.log(text), client.status(), client.say(text)");
	return 1;
}

bool LuaRuntime::execute(const std::string& code, std::vector<std::string>& output, std::string& error)
{
	output.clear();
	error.clear();
	if(code.empty())
		return true;
	if(!initialize(error))
		return false;

	lua_settop(m_state, 0);
	std::string expression = std::string("return ").append(code);
	int loadResult = luaL_loadbuffer(m_state, expression.c_str(), expression.length(), "=terminal");
	if(loadResult != LUA_OK)
	{
		lua_pop(m_state, 1);
		loadResult = luaL_loadbuffer(m_state, code.c_str(), code.length(), "=terminal");
	}

	if(loadResult != LUA_OK)
	{
		const char* message = lua_tostring(m_state, -1);
		error = (message ? message : "Lua syntax error.");
		lua_settop(m_state, 0);
		return false;
	}

	m_currentOutput = &output;
	m_instructionCount = 0;
	lua_sethook(m_state, &LuaRuntime::instructionHook, LUA_MASKCOUNT, LUA_HOOK_INTERVAL);
	int callResult = lua_pcall(m_state, 0, LUA_MULTRET, 0);
	lua_sethook(m_state, nullptr, 0, 0);

	if(callResult != LUA_OK)
	{
		const char* message = lua_tostring(m_state, -1);
		error = (message ? message : "Lua execution failed.");
		m_currentOutput = nullptr;
		lua_settop(m_state, 0);
		return false;
	}

	int resultCount = lua_gettop(m_state);
	if(resultCount > 0 && !(resultCount == 1 && lua_isnil(m_state, 1)))
	{
		std::string line;
		for(int index = 1; index <= resultCount; ++index)
		{
			if(index > 1)
				line.push_back('\t');
			line.append(getSafeLuaValue(m_state, index));
		}
		appendOutput(line);
	}

	m_currentOutput = nullptr;
	lua_settop(m_state, 0);
	return true;
}
