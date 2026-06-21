/*
  The Forgotten Client
  Copyright (C) 2020 Saiyans King

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __FILE_CONFIG_h_
#define __FILE_CONFIG_h_

#include "defines.h"

typedef std::vector<std::string> StringVector;

struct ConfigKey
{
	std::string name;
	std::string value;

	ConfigKey(const std::string _name, const std::string _value)
	{
		name = std::move(_name);
		value = std::move(_value);
	}

	// non-copyable
	ConfigKey(const ConfigKey&) = delete;
	ConfigKey& operator=(const ConfigKey&) = delete;

	// moveable
	ConfigKey(ConfigKey&& rhs) noexcept : name(std::move(rhs.name)), value(std::move(rhs.value)) {}
	ConfigKey& operator=(ConfigKey&& rhs) noexcept
	{
		if(this != &rhs)
		{
			name = std::move(rhs.name);
			value = std::move(rhs.value);
		}
		return (*this);
	}
};

class Config
{
	public:
		Config() = default;
		~Config();

		// non-copyable
		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;

		// non-moveable
		Config(Config&&) = delete;
		Config& operator=(Config&&) = delete;

		bool openToRead(const char* fileName);
		bool openToSave(const char* fileName);

		void insertKey(const std::string& keyName, const std::string& value);
		std::string fetchKey(const std::string& keyName);
		StringVector fetchKeys(const std::string& keyName);

		void insertNewLine();
		void saveFile();

	protected:
		std::vector<ConfigKey> m_keys;
		SDL_RWops* m_fileHandle = NULL;
		bool m_saveHandle = false;
};

#endif /* __FILE_CONFIG_h_ */
