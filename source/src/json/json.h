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

#ifndef __FILE_JSON_h_
#define __FILE_JSON_h_

#include "../defines.h"

enum JSON_TYPE
{
	JSON_TYPE_NULL,
	JSON_TYPE_STRING,
	JSON_TYPE_BOOL,
	JSON_TYPE_NUMBER,
	JSON_TYPE_ARRAY,
	JSON_TYPE_OBJECT
};

class JSON_VALUE;
typedef std::vector<JSON_VALUE*> JSON_ARRAY;
typedef std::map<std::string, JSON_VALUE*> JSON_OBJECT;

class JSON_VALUE
{
	public:
		JSON_VALUE(/*NULL*/) {type = JSON_TYPE_NULL;}
		JSON_VALUE(const char* _const_char_value) {type = JSON_TYPE_STRING; string_value = new std::string(std::string(_const_char_value));}
		JSON_VALUE(const std::string& _string_value) {type = JSON_TYPE_STRING; string_value = new std::string(_string_value);}
		JSON_VALUE(bool _bool_value) {type = JSON_TYPE_BOOL; bool_value = _bool_value;}
		JSON_VALUE(float _float_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _float_value);}
		JSON_VALUE(double _number_value) {type = JSON_TYPE_NUMBER; number_value = _number_value;}
		JSON_VALUE(Sint8 _integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _integer_value);}
		JSON_VALUE(Uint8 _unsigned_integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _unsigned_integer_value);}
		JSON_VALUE(Sint16 _integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _integer_value);}
		JSON_VALUE(Uint16 _unsigned_integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _unsigned_integer_value);}
		JSON_VALUE(Sint32 _integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _integer_value);}
		JSON_VALUE(Uint32 _unsigned_integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _unsigned_integer_value);}
		JSON_VALUE(Sint64 _integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _integer_value);}
		JSON_VALUE(Uint64 _unsigned_integer_value) {type = JSON_TYPE_NUMBER; number_value = SDL_static_cast(double, _unsigned_integer_value);}
		JSON_VALUE(const JSON_ARRAY& _array_value) {type = JSON_TYPE_ARRAY; array_value = new JSON_ARRAY(_array_value);}
		JSON_VALUE(const JSON_OBJECT& _object_value) {type = JSON_TYPE_OBJECT; object_value = new JSON_OBJECT(_object_value);}
		JSON_VALUE(const JSON_VALUE& _source);
		~JSON_VALUE();

		JSON_TYPE IsValue() {return type;}
		bool IsNull() const {return (type == JSON_TYPE_NULL);}
		bool IsString() const {return (type == JSON_TYPE_STRING);}
		bool IsBool() const {return (type == JSON_TYPE_BOOL);}
		bool IsNumber() const {return (type == JSON_TYPE_NUMBER);}
		bool IsArray() const {return (type == JSON_TYPE_ARRAY);}
		bool IsObject() const {return (type == JSON_TYPE_OBJECT);}

		const std::string& AsString() const {return (*string_value);}
		bool AsBool() const {return bool_value;}
		double AsNumber() const {return number_value;}
		const JSON_ARRAY& AsArray() const {return (*array_value);}
		const JSON_OBJECT& AsObject() const {return (*object_value);}

		static bool SkipWhitespace(const char* &data);
		static bool ParseString(const char* &data, std::string& str);
		static double ParseInt(const char* &data);
		static double ParseDecimal(const char* &data);
		static std::string MakeString(const std::string& str);

		std::string serialize();
		static JSON_VALUE* unserialize(const char* &data);

		static std::string encode(JSON_VALUE* value);
		static JSON_VALUE* decode(const char* data);

		size_t size() const;
		JSON_VALUE* find(size_t index);
		JSON_VALUE* find(const std::string& name);

	private:
		JSON_TYPE type;
		union
		{
			bool bool_value;
			double number_value;
			std::string* string_value;
			JSON_ARRAY* array_value;
			JSON_OBJECT* object_value;
		};
};

#endif /* __FILE_JSON_h_ */
