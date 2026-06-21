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

#ifndef __FILE_CIRCULAR_BUFFER_h_
#define __FILE_CIRCULAR_BUFFER_h_

#include <algorithm>

namespace std
{
	template <class T, size_t s>
	class circular_buffer
	{
		public:
			T& operator[](size_t i)
			{
				size_t pos = (_back_ + i) % _size_;
				return _data_[pos];
			}
			T& at(size_t i)
			{
				size_t pos = (_back_ + i) % _size_;
				return _data_[pos];
			}

			T& front() {return _data_[_front_];}
			T& back() {return _data_[_back_];}

			T *data() {return _data_;}

			void push_back(T v)
			{
				_data_[_back_] = v;
				if(++_back_ == _size_)
					_back_ = 0;

				if(_back_ == _front_)
				{
					if(++_front_ == _size_)
						_front_ = 0;
				}
			}
			void push_front(T v)
			{
				_data_[_front_] = v;
				if(--_front_ < 0)
					_front_ = _size_ - 1;

				if(_front_ == _back_)
				{
					if(--_back_ < 0)
						_back_ = _size_ - 1;
				}
			}

			void pop_back(void)
			{
				if(--_back_ < 0)
					_back_ = _size_ - 1;
			}
			void pop_front(void)
			{
				if(++_front_ == _size_)
					_front_ = 0;
			}

			void clear(void) {_front_ = _back_;}
			bool empty(void) {return _front_ == _back_;}
			bool full(void) {return ((_back_ + 1) & (_size_ - 1)) == _front_;}
			Sint32 max_size(void) {return _size_;}

		private:
			T _data_[s];
			Sint32 _front_ = 0;
			Sint32 _back_ = 0;
			Sint32 _size_ = s;
	};
}

#endif
