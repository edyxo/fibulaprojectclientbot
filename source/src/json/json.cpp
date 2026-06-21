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

#include "json.h"

#define FREE_ARRAY(x) {for(JSON_ARRAY::iterator it = x.begin(), end = x.end(); it != end; ++it){delete (*it);}}
#define FREE_OBJECT(x) {for(JSON_OBJECT::iterator it = x.begin(), end = x.end(); it != end; ++it){delete (*it).second;}}

char JSON_tolower(char src)
{
	return (('A' <= src && src <= 'Z') ? src + 0x20 : src);
}

JSON_VALUE::JSON_VALUE(const JSON_VALUE& _source)
{
	type = _source.type;
	switch(type)
	{
		case JSON_TYPE_STRING:
			string_value = new std::string(*_source.string_value);
			break;

		case JSON_TYPE_BOOL:
			bool_value = _source.bool_value;
			break;

		case JSON_TYPE_NUMBER:
			number_value = _source.number_value;
			break;

		case JSON_TYPE_ARRAY:
		{
			JSON_ARRAY source_array = *_source.array_value;
			array_value = new JSON_ARRAY();
			for(JSON_ARRAY::iterator it = source_array.begin(), end = source_array.end(); it != end; ++it)
				array_value->push_back(new JSON_VALUE(*(*it)));
		}
		break;

		case JSON_TYPE_OBJECT:
		{
			JSON_OBJECT source_object = *_source.object_value;
			object_value = new JSON_OBJECT();
			for(JSON_OBJECT::iterator it = source_object.begin(), end = source_object.end(); it != end; ++it)
			{
				std::string name = (*it).first;
				(*object_value)[name] = new JSON_VALUE(*((*it).second));
			}
		}
		break;

		default:
			break;
	}
}

JSON_VALUE::~JSON_VALUE()
{
	if(type == JSON_TYPE_ARRAY)
	{
		for(JSON_ARRAY::iterator it = array_value->begin(), end = array_value->end(); it != end; ++it)
			delete (*it);
		delete array_value;
	}
	else if(type == JSON_TYPE_OBJECT)
	{
		for(JSON_OBJECT::iterator it = object_value->begin(), end = object_value->end(); it != end; ++it)
			delete (*it).second;
		delete object_value;
	}
	else if(type == JSON_TYPE_STRING)
		delete string_value;
}

bool JSON_VALUE::SkipWhitespace(const char* &data)
{
	while(data[0] != '\0' && (data[0] == ' ' || data[0] == '\t' || data[0] == '\r' || data[0] == '\n'))
		++data;

	return (data[0] != '\0');
}

bool JSON_VALUE::ParseString(const char* &data, std::string& str)
{
	str.clear();
	while(data[0] != '\0')
	{
		char current_character = data[0];
		if(current_character == '\\')//Escaped character
		{
			current_character = (++data)[0];
			switch(current_character)
			{
				case '"': current_character = '"'; break;
				case '\\': current_character = '\\'; break;
				case '/': current_character = '/'; break;
				case 'b': current_character = '\b'; break;
				case 'f': current_character = '\f'; break;
				case 'n': current_character = '\n'; break;
				case 'r': current_character = '\r'; break;
				case 't': current_character = '\t'; break;
				case 'u':
				{
					++data;
					if(data[0] == '\0' || data[1] == '\0' || data[2] == '\0' || data[3] == '\0')
						return false;

					data += 4;
					current_character = '?';
				}
				break;
				default: return false;
			}
		}
		else if(current_character == '"')//String ending
		{
			++data;
			return true;
		}
		else if(current_character < ' ' && current_character != '\t')//Unallowed characters
			return false;

		str.append(1, current_character);
		++data;
	}
	return false;
}

double JSON_VALUE::ParseInt(const char* &data)
{
	double _int = 0.0;
	while(data[0] != '\0' && (data[0] >= '0' && data[0] <= '9'))
		_int = _int * 10 + ((data++)[0] - '0');

	return _int;
}

double JSON_VALUE::ParseDecimal(const char* &data)
{
	double _decimal = 0.0;
	double _factor = 0.1;
	while(data[0] != '\0' && (data[0] >= '0' && data[0] <= '9'))
	{
		Sint32 _int = ((data++)[0] - '0');
		_decimal += (_int * _factor);
		_factor *= 0.1;
	}
	return _decimal;
}

std::string JSON_VALUE::MakeString(const std::string& str)
{
	std::string str_out = "\"";
	for(std::string::const_iterator it = str.begin(), end = str.end(); it != end; ++it)
	{
		char chr = (*it);
		if(chr == '"' || chr == '\\' || chr == '/')
		{
			str_out.append(1, '\\');
			str_out.append(1, chr);
		}
		else if(chr == '\b')
		{
			str_out.append(1, '\\');
			str_out.append(1, 'b');
		}
		else if(chr == '\f')
		{
			str_out.append(1, '\\');
			str_out.append(1, 'f');
		}
		else if(chr == '\n')
		{
			str_out.append(1, '\\');
			str_out.append(1, 'n');
		}
		else if(chr == '\r')
		{
			str_out.append(1, '\\');
			str_out.append(1, 'r');
		}
		else if(chr == '\t')
		{
			str_out.append(1, '\\');
			str_out.append(1, 't');
		}
		else if(chr < ' ' || chr > 126)
			str_out.append(1, '?');
		else
			str_out.append(1, chr);
	}
	str_out.append(1, '"');
	return str_out;
}

std::string JSON_VALUE::serialize()
{
	std::string str;
	switch(type)
	{
		case JSON_TYPE_NULL:
			str = "null";
			break;

		case JSON_TYPE_STRING:
			str = MakeString(*string_value);
			break;

		case JSON_TYPE_BOOL:
			str = (bool_value ? "true" : "false");
			break;

		case JSON_TYPE_NUMBER:
		{
			if(isinf(number_value) || isnan(number_value))
				str = "null";
			else
			{
				std::stringstream ss;
				ss.precision(15);
				ss << number_value;
				str = ss.str();
			}
		}
		break;

		case JSON_TYPE_ARRAY:
		{
			str = "[";
			for(JSON_ARRAY::const_iterator it = array_value->begin(), end = array_value->end(); it != end;)
			{
				str.append((*it)->serialize());
				if(++it != end)
					str.append(1, ',');
			}
			str.append(1, ']');
		}
		break;

		case JSON_TYPE_OBJECT:
		{
			str = "{";
			for(JSON_OBJECT::const_iterator it = object_value->begin(), end = object_value->end(); it != end;)
			{
				str.append(MakeString((*it).first));
				str.append(1, ':');
				str.append((*it).second->serialize());
				if(++it != end)
					str.append(1, ',');
			}
			str.append(1, '}');
		}
		break;

		default:
			break;
	}
	return str;
}

JSON_VALUE* JSON_VALUE::unserialize(const char* &data)
{
	//String
	if(data[0] == '"')
	{
		std::string str;
		if(!ParseString(++data, str))
			return NULL;
		else
			return new JSON_VALUE(str);
	}
	//Bool - true
	else if((data[0] != '\0' && data[1] != '\0' && data[2] != '\0' && data[3] != '\0')
		&& (JSON_tolower(data[0]) == 't' && JSON_tolower(data[1]) == 'r' && JSON_tolower(data[2]) == 'u' && JSON_tolower(data[3]) == 'e'))
	{
		data += 4;
		return new JSON_VALUE(true);
	}
	//Bool - false
	else if((data[0] != '\0' && data[1] != '\0' && data[2] != '\0' && data[3] != '\0' && data[4] != '\0')
		&& (JSON_tolower(data[0]) == 'f' && JSON_tolower(data[1]) == 'a' && JSON_tolower(data[2]) == 'l' && JSON_tolower(data[3]) == 's' && JSON_tolower(data[4]) == 'e'))
	{
		data += 5;
		return new JSON_VALUE(false);
	}
	//Null
	else if((data[0] != '\0' && data[1] != '\0' && data[2] != '\0' && data[3] != '\0')
		&& (JSON_tolower(data[0]) == 'n' && JSON_tolower(data[1]) == 'u' && JSON_tolower(data[2]) == 'l' && JSON_tolower(data[3]) == 'l'))
	{
		data += 4;
		return new JSON_VALUE();
	}
	//Number
	else if(data[0] == '-' || (data[0] >= '0' && data[0] <= '9'))
	{
		bool negative = (data[0] == '-');
		if(negative)
			++data;

		double number = 0.0;
		if(data[0] == '0')
			++data;
		else if(data[0] >= '1' && data[0] <= '9')
			number = ParseInt(data);
		else
			return NULL;

		//Decimal
		if(data[0] == '.')
		{
			++data;
			if(data[0] < '0' || data[0] > '9')
				return NULL;

			number += ParseDecimal(data);
		}

		//Exponent
		if(data[0] == 'E' || data[0] == 'e')
		{
			++data;

			bool negative_exponent = false;
			if(data[0] == '-' || data[0] == '+')
			{
				negative_exponent = (data[0] == '-');
				++data;
			}
			if(data[0] < '0' || data[0] > '9')
				return NULL;

			double exponent = ParseInt(data);
			for(double i = 0.0; i < exponent; ++i)
				number = negative_exponent ? (number / 10.0) : (number * 10.0);
		}

		//Negate
		if(negative)
			number *= -1.0;

		return new JSON_VALUE(number);
	}
	//Object
	else if(data[0] == '{')
	{
		JSON_OBJECT newObject;
		++data;
		while(data[0] != '\0')
		{
			if(!SkipWhitespace(data))
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			if(newObject.size() == 0 && data[0] == '}')
			{
				++data;
				return new JSON_VALUE(newObject);
			}

			std::string name;
			if(!ParseString(++data, name))
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			if(!SkipWhitespace(data))
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			if((data++)[0] != ':')
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			if(!SkipWhitespace(data))
			{
				FREE_OBJECT(newObject);
				return NULL;
			}

			JSON_VALUE* value = unserialize(data);
			if(value == NULL)
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			if(newObject.find(name) != newObject.end())
				delete newObject[name];
			newObject[name] = value;

			if(!SkipWhitespace(data))
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			if(data[0] == '}')
			{
				++data;
				return new JSON_VALUE(newObject);
			}
			if(data[0] != ',')
			{
				FREE_OBJECT(newObject);
				return NULL;
			}
			++data;
		}
		FREE_OBJECT(newObject);
		return NULL;
	}
	//Array
	else if(data[0] == '[')
	{
		JSON_ARRAY newArray;
		++data;
		while(data[0] != '\0')
		{
			if(!SkipWhitespace(data))
			{
				FREE_ARRAY(newArray);
				return NULL;
			}
			if(newArray.size() == 0 && data[0] == ']')
			{
				++data;
				return new JSON_VALUE(newArray);
			}
			JSON_VALUE* value = unserialize(data);
			if(value == NULL)
			{
				FREE_ARRAY(newArray);
				return NULL;
			}
			newArray.push_back(value);
			if(!SkipWhitespace(data))
			{
				FREE_ARRAY(newArray);
				return NULL;
			}
			if(data[0] == ']')
			{
				++data;
				return new JSON_VALUE(newArray);
			}
			if(data[0] != ',')
			{
				FREE_ARRAY(newArray);
				return NULL;
			}
			++data;
		}
		FREE_ARRAY(newArray);
		return NULL;
	}
	//Unknown
	return NULL;
}

std::string JSON_VALUE::encode(JSON_VALUE* value)
{
	if(value != NULL)
		return value->serialize();

	return std::string();
}

JSON_VALUE* JSON_VALUE::decode(const char* data)
{
	if(!SkipWhitespace(data))
		return NULL;

	return unserialize(data);
}

size_t JSON_VALUE::size() const
{
	switch(type)
	{
		case JSON_TYPE_ARRAY: return array_value->size();
		case JSON_TYPE_OBJECT: return object_value->size();
		default: break;
	}
	return 0;
}

JSON_VALUE* JSON_VALUE::find(size_t index)
{
	if(type != JSON_TYPE_ARRAY)
		return NULL;

	return (index < array_value->size() ? (*array_value)[index] : NULL);
}

JSON_VALUE* JSON_VALUE::find(const std::string& name)
{
	if(type != JSON_TYPE_OBJECT)
		return NULL;

	JSON_OBJECT::const_iterator it = object_value->find(name);
	if(it != object_value->end())
		return it->second;

	return NULL;
}
