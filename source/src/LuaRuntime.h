#ifndef __FILE_LUA_RUNTIME_h_
#define __FILE_LUA_RUNTIME_h_

#include <cstddef>
#include <string>
#include <vector>

struct lua_State;
struct lua_Debug;

class LuaRuntime
{
	public:
		LuaRuntime() = default;
		~LuaRuntime();

		LuaRuntime(const LuaRuntime&) = delete;
		LuaRuntime& operator=(const LuaRuntime&) = delete;

		bool execute(const std::string& code, std::vector<std::string>& output, std::string& error);

	private:
		struct MemoryState
		{
			size_t used = 0;
			size_t limit = 32 * 1024 * 1024;
		};

		bool initialize(std::string& error);
		void appendOutput(const std::string& text);

		static void* allocate(void* userData, void* pointer, size_t oldSize, size_t newSize);
		static void instructionHook(lua_State* state, lua_Debug* debugRecord);
		static int luaPrint(lua_State* state);
		static int clientLog(lua_State* state);
		static int clientSay(lua_State* state);
		static int clientStatus(lua_State* state);
		static int clientHelp(lua_State* state);

		lua_State* m_state = nullptr;
		MemoryState m_memory;
		std::vector<std::string>* m_currentOutput = nullptr;
		size_t m_instructionCount = 0;
};

#endif /* __FILE_LUA_RUNTIME_h_ */
