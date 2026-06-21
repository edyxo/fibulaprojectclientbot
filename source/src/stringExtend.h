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

#ifndef __FILE_STRINGEXTEND_h_
#define __FILE_STRINGEXTEND_h_

#include <string>
#include <cstring>

constexpr char ext_digits[] =
"0001020304050607080910111213141516171819"
"2021222324252627282930313233343536373839"
"4041424344454647484950515253545556575859"
"6061626364656667686970717273747576777879"
"8081828384858687888990919293949596979899";

namespace std
{
	class stringExtended
	{
		public:
			stringExtended() = default;
			stringExtended(size_t reserveSize) {outStr.reserve(reserveSize);}

			SDL_INLINE string::iterator begin() noexcept {
				return outStr.begin();
			}
			SDL_INLINE string::iterator end() noexcept {
				return outStr.end();
			}
			SDL_INLINE string::const_iterator begin() const noexcept {
				return outStr.begin();
			}
			SDL_INLINE string::const_iterator end() const noexcept {
				return outStr.end();
			}
			SDL_INLINE size_t length() const noexcept {
				return outStr.length();
			}
			SDL_INLINE void clear() noexcept {
				outStr.clear();
			}
			SDL_INLINE void reserve(size_t count) {
				outStr.reserve(count);
			}
			SDL_INLINE void insert(string::const_iterator it, string::value_type element) {
				outStr.insert(it, element);
			}
			SDL_INLINE void push_back(string::value_type element) {
				outStr.push_back(element);
			}
			SDL_INLINE string substr(size_t pos, size_t len = std::string::npos) {
				return outStr.substr(pos, len);
			}
			SDL_INLINE stringExtended& append(const string& str) {
				outStr.append(str);
				return (*this);
			}
			SDL_INLINE stringExtended& append(const string& str, size_t subpos, size_t sublen) {
				outStr.append(str, subpos, sublen);
				return (*this);
			}
			SDL_INLINE stringExtended& append(const char* str) {
				outStr.append(str);
				return (*this);
			}
			SDL_INLINE stringExtended& append(const char* str, size_t n) {
				outStr.append(str, n);
				return (*this);
			}
			SDL_INLINE stringExtended& append(size_t n, char c) {
				outStr.append(n, c);
				return (*this);
			}
			SDL_INLINE string::reference operator[](size_t index) {
				return outStr.operator[](index);
			}
			SDL_INLINE string::const_reference operator[](size_t index) const {
				return outStr.operator[](index);
			}

			stringExtended& appendInt(uint64_t value) {
				char str_buffer[22]; // Should be able to contain uint64_t max value + sign
				char* ptrBufferEnd = str_buffer + 21;
				char* ptrBuffer = ptrBufferEnd;

				while (value >= 100) {
					uint32_t index = static_cast<uint32_t>((value % 100) * 2);
					value /= 100;

					ptrBuffer -= 2;
					memcpy(ptrBuffer, ext_digits + index, 2);
				}
				if (value < 10) {
					*--ptrBuffer = static_cast<char>('0' + value);
					return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
				}

				uint32_t index = static_cast<uint32_t>(value * 2);
				ptrBuffer -= 2;
				memcpy(ptrBuffer, ext_digits + index, 2);
				return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
			}
		
			stringExtended& appendInt(int64_t value) {
				uint64_t abs_value = static_cast<uint64_t>(value);
				bool neg = (value < 0);
				if (neg) {
					abs_value = 0 - abs_value;
				}

				char str_buffer[22]; // Should be able to contain uint64_t max value + sign
				char* ptrBufferEnd = str_buffer + 21;
				char* ptrBuffer = ptrBufferEnd;

				while (abs_value >= 100) {
					uint32_t index = static_cast<uint32_t>((abs_value % 100) * 2);
					abs_value /= 100;

					ptrBuffer -= 2;
					memcpy(ptrBuffer, ext_digits + index, 2);
				}
				if (abs_value < 10) {
					*--ptrBuffer = static_cast<char>('0' + abs_value);
					if (neg) {
						*--ptrBuffer = '-';
					}
					return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
				}

				uint32_t index = static_cast<uint32_t>(abs_value * 2);
				ptrBuffer -= 2;
				memcpy(ptrBuffer, ext_digits + index, 2);
				if (neg) {
					*--ptrBuffer = '-';
				}
				return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
			}

			SDL_INLINE stringExtended& appendInt(uint32_t value) {return appendInt(static_cast<uint64_t>(value));}
			SDL_INLINE stringExtended& appendInt(uint16_t value) {return appendInt(static_cast<uint64_t>(value));}
			SDL_INLINE stringExtended& appendInt(uint8_t value) {return appendInt(static_cast<uint64_t>(value));}
			SDL_INLINE stringExtended& appendInt(int32_t value) {return appendInt(static_cast<int64_t>(value));}
			SDL_INLINE stringExtended& appendInt(int16_t value) {return appendInt(static_cast<int64_t>(value));}
			SDL_INLINE stringExtended& appendInt(int8_t value) {return appendInt(static_cast<int64_t>(value));}
			
			stringExtended& appendIntShowPos(uint64_t value) {
				char str_buffer[22]; // Should be able to contain uint64_t max value + sign
				char* ptrBufferEnd = str_buffer + 21;
				char* ptrBuffer = ptrBufferEnd;

				while (value >= 100) {
					uint32_t index = static_cast<uint32_t>((value % 100) * 2);
					value /= 100;

					ptrBuffer -= 2;
					memcpy(ptrBuffer, ext_digits + index, 2);
				}
				if (value < 10) {
					*--ptrBuffer = static_cast<char>('0' + value);
					*--ptrBuffer = '+';
					return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
				}

				uint32_t index = static_cast<uint32_t>(value * 2);
				ptrBuffer -= 2;
				memcpy(ptrBuffer, ext_digits + index, 2);
				*--ptrBuffer = '+';
				return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
			}
			
			stringExtended& appendIntShowPos(int64_t value) {
				uint64_t abs_value = static_cast<uint64_t>(value);
				bool neg = (value < 0);
				if (neg) {
					abs_value = 0 - abs_value;
				}

				char str_buffer[22]; // Should be able to contain uint64_t max value + sign
				char* ptrBufferEnd = str_buffer + 21;
				char* ptrBuffer = ptrBufferEnd;

				while (abs_value >= 100) {
					uint32_t index = static_cast<uint32_t>((abs_value % 100) * 2);
					abs_value /= 100;

					ptrBuffer -= 2;
					memcpy(ptrBuffer, ext_digits + index, 2);
				}
				if (abs_value < 10) {
					*--ptrBuffer = static_cast<char>('0' + abs_value);
					if (neg) {
						*--ptrBuffer = '-';
					} else {
						*--ptrBuffer = '+';
					}
					return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
				}

				uint32_t index = static_cast<uint32_t>(abs_value * 2);
				ptrBuffer -= 2;
				memcpy(ptrBuffer, ext_digits + index, 2);
				if (neg) {
					*--ptrBuffer = '-';
				} else {
					*--ptrBuffer = '+';
				}
				return append(ptrBuffer, std::distance(ptrBuffer, ptrBufferEnd));
			}

			SDL_INLINE stringExtended& appendIntShowPos(uint32_t value) {return appendIntShowPos(static_cast<uint64_t>(value));}
			SDL_INLINE stringExtended& appendIntShowPos(uint16_t value) {return appendIntShowPos(static_cast<uint64_t>(value));}
			SDL_INLINE stringExtended& appendIntShowPos(uint8_t value) {return appendIntShowPos(static_cast<uint64_t>(value));}
			SDL_INLINE stringExtended& appendIntShowPos(int32_t value) {return appendIntShowPos(static_cast<int64_t>(value));}
			SDL_INLINE stringExtended& appendIntShowPos(int16_t value) {return appendIntShowPos(static_cast<int64_t>(value));}
			SDL_INLINE stringExtended& appendIntShowPos(int8_t value) {return appendIntShowPos(static_cast<int64_t>(value));}

			SDL_INLINE stringExtended& operator<< (const string& str) {return append(str);}
			SDL_INLINE stringExtended& operator<< (const char* str) {return append(str);}
			SDL_INLINE stringExtended& operator<< (const char c) {return append(1, c);}
			
			SDL_INLINE stringExtended& operator<< (uint64_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (uint32_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (uint16_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (uint8_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (int64_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (int32_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (int16_t value) {return appendInt(value);}
			SDL_INLINE stringExtended& operator<< (int8_t value) {return appendInt(value);}
			
			SDL_INLINE stringExtended& operator<<= (uint64_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (uint32_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (uint16_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (uint8_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (int64_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (int32_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (int16_t value) {return appendIntShowPos(value);}
			SDL_INLINE stringExtended& operator<<= (int8_t value) {return appendIntShowPos(value);}

			//Allow implicit conversion to std::string&
			operator string&() {return outStr;}

		private:
			string outStr;
	};
};

#endif /* __FILE_STRINGEXTEND_h_ */
