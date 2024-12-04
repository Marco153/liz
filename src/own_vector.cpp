
namespace own_std
{

	struct string
	{
		char* data_;
		char** c_str_vec;
		u64 c_str_vec_count;
		u64 c_str_vec_len;
		unsigned long long len;

		bool contains_char_rev(char c, u64* p)
		{
			for (unsigned long long i = (len - 1); i >= 0; i--)
			{
				if (data_[i] == c)
				{
					*p = i;
					return true;
				}
			}
			return false;
		}
		/*
		u64 insert(u64 offset, u64 sz)
		{
			u64 total = len + sz;
			char* buffer = (char *)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, sz);
			memcpy(buffer + sz, other.data + offset + sz, other.len);
			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = total;
		}
		*/
		u64 insert(u64 offset, own_std::string other)
		{
			u64 total = len + other.len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, offset);
			memcpy(buffer + offset, other.data_, other.len);
			memcpy(buffer + offset + other.len, data_ + offset, len - offset);
			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = total;
		}
		u64 erase(u64 offset, u64 sz)
		{
			u64 total = len - sz;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, sz);
			memcpy(buffer + sz, data_ + offset + sz, len - (offset + sz));
			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = total;
		}
		s64 find_last_of(char ch)
		{
			u64 idx;
			if (contains_char_rev(ch, &idx))
				return idx;
			return -1;
		}
		s64 find_last_of(const char* chars)
		{
			u64 l = strlen(chars);

			for (u64 i = l; i >= 0; i--)
			{
				u64 idx = 0;
				if (contains_char_rev(chars[i], &idx))
					return idx;
			}
			return -1;
		}
		void new_c_str_vec(u64 new_len)
		{
			auto prev = c_str_vec;
			c_str_vec = (char**)__lang_globals.alloc(__lang_globals.data, new_len * 8);
			memset(c_str_vec, 0, new_len * 8);
			memcpy(c_str_vec, prev, c_str_vec_len * 8);
			c_str_vec_len = new_len;
		}
		char* c_str()
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, len + 1);
			memcpy(buffer, data_, len);

			buffer[len + 1] = 0;
			if (c_str_vec_len <= 0 || c_str_vec_len >= 64)
			{
				new_c_str_vec(4);
			}
			if (c_str_vec_count >= c_str_vec_len)
				new_c_str_vec(c_str_vec_len * 2);

			c_str_vec[c_str_vec_count] = buffer;


			return buffer;
		}
		char* data()
		{
			return data_;
		}
		void pop_back()
		{
			len--;
		}
		void reserve(u64 size)
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, size);
			data_ = buffer;
			len = size;
		}
		unsigned long long length()
		{
			return len;
		}
		unsigned long long size() const
		{
			return len;
		}
		unsigned long long size()
		{
			return len;
		}
		string(const string& other)
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, other.len);
			memcpy(buffer, other.data_, other.len);
			data_ = buffer;
			len = other.len;
		}
		void operator =(string &src)
		{
			u64 l = src.len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, l);
			memcpy(buffer, src.data_, l);
			data_ = buffer;
			len = l;
		}
		void operator =(const char* src)
		{
			u64 l = strlen(src);
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, l);
			memcpy(buffer, src, strlen(src));
			data_ = buffer;
			len = l;
		}
		string(const char* src)
		{
			u64 l = strlen(src);
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, l);
			memcpy(buffer, src, strlen(src));
			data_ = buffer;
			len = l;
		}
		string(const char* src, u64 size)
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, size);
			string new_one;
			memcpy(buffer, src, size);
			data_ = buffer;
			len = size;
			//pru64f("data_
		}
		string(char* src, u64 size)
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, size);
			string new_one;
			memcpy(buffer, src, size);
			data_ = buffer;
			len = size;
		}
		~string()
		{
			if (data_)
				__lang_globals.free(__lang_globals.data, data_);
		}
		string()
		{
			memset(this, 0, sizeof(*this));
		}
		string substr()
		{
			return substr(0, len);
		}
		string substr(u64 idx)
		{
			u64 new_l = len - idx;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, new_l);
			string new_one;
			memcpy(buffer, data_ + idx, new_l);
			new_one.data_;
			new_one.len = new_l;

			return new_one;
		}
		string substr(u64 idx, u64 size)
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, size);
			string new_one;
			memcpy(buffer, data_ + idx, size);
			new_one.data_ = buffer;
			new_one.len = size;

			return new_one;
		}
		void concat_in_place(string& other)
		{
			u64 total = other.len + len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, len);
			memcpy(buffer + len, other.data_, other.len);
			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = total;

		}
		void concat_in_place(const char* other_buffer)
		{
			u64 other_len = strlen(other_buffer);
			u64 total = other_len + len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, len);
			memcpy(buffer + len, other_buffer, other_len);

			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = total;

		}
		void concat_in_place(char* other_buffer, u64 other_len)
		{
			u64 total = other_len + len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, len);
			memcpy(buffer + len, other_buffer, other_len);

			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = total;

		}
		void assign(string& other)
		{
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, other.len);
			memcpy(buffer, other.data_, other.len);
			__lang_globals.free(__lang_globals.data, data_);
			data_ = buffer;
			len = other.len;

		}
		string concat(char* other, u64 other_len)
		{
			u64 total = other_len + len; char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, len);
			memcpy(buffer + len, other, other_len);
			new_one.data_ = buffer;
			new_one.len = total;

			return new_one;
		}
		string concat(const char* other, u64 other_len)
		{
			u64 total = other_len + len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, len);
			memcpy(buffer + len, other, other_len);
			new_one.data_ = buffer;
			new_one.len = total;

			return new_one;
		}
		string concat(string& other)
		{
			u64 total = other.len + len;
			char* buffer = (char*)__lang_globals.alloc(__lang_globals.data, total);
			string new_one;
			memcpy(buffer, data_, len);
			memcpy(buffer + len, other.data_, other.len);
			new_one.data_ = buffer;
			new_one.len = total;

			return new_one;
		}
		bool operator !=(const char* other)
		{
			if (len != strlen(other))
				return false;

			return memcmp(data_, other, len) != 0;
		}
		bool operator ==(const char* other)
		{
			if (len != strlen(other))
				return false;

			return memcmp(data_, other, len) == 0;
		}
		bool operator ==(string&& other)
		{
			if (len != other.len)
				return false;

			return memcmp(data_, other.data_, len) == 0;
		}
		bool operator ==(const string other) const
		{
			if (len != other.len)
				return false;

			return memcmp(data_, other.data_, len) == 0;
		}
		bool operator ==(string& other)
		{
			if (len != other.len)
				return false;

			return memcmp(data_, other.data_, len) == 0;
		}
		string operator +=(char other)
		{
			return concat(&other, 1);
		}
		string operator +=(const char* other)
		{
			return concat(other, strlen(other));
		}
		string operator +=(char* other)
		{
			return concat(other, strlen(other));
		}
		string operator +=(string other)
		{
			return concat(other);
		}
		void push_back(char other)
		{
			concat_in_place(&other, 1);
		}
		void append(string&& other)
		{
			concat_in_place(other);
		}
		void append(string& other)
		{
			concat_in_place(other);
		}
		string operator +(string other)
		{
			// use concat_in_place instead
			//ASSERT(0);
			return concat(other);
		}
		char& operator [](u64 idx)
		{
			if (idx >= len)
			{
				*(u64*)0 = 0;
			}

			return data_[idx];
		}
	};

	template<typename T, u64 method = 0>
	struct vector
	{
		LangArray<T> ar;
		void Init(u64 len)
		{
			if (__lang_globals.data == nullptr)
				return;

			// 
			T* b = (T*)__lang_globals.alloc(__lang_globals.data, (len + 1) * sizeof(T));
			memset(b, 0, ar.length * sizeof(T));

			ar.start = b;
			ar.end = b;
			ar.count = 0;
			ar.length = len;
		}
		vector(u64 len)
		{
			memset(this, 0, sizeof(*this));
			Init(len);
		}
		vector()
		{
			memset(this, 0, sizeof(*this));
			Init(1);
		}

		T& operator [](u64 idx)
		{
			return *ar[idx];
		}

		vector(vector& a)
		{
			memset(this, 0, sizeof(*this));
			this->assign(a.begin(), a.end());
		}
		vector& operator=(vector& a)
		{
			this->assign(a.begin(), a.end());
			return *this;

		}

		void assign(T* start, T* end)
		{
			u64 count = end - start;

			regrow(count);
			ar.count = count;
			ar.end += count;

			memcpy(ar.start, start, count * sizeof(T));
		}

		T* end()
		{
			return begin() + ar.count;
		}
		T* begin()
		{
			if (ar.start == nullptr)
				Init(1);
			return ar.start;
		}
		void make_count(u64 new_size)
		{
			regrow(new_size);
			ar.count = new_size;
		}
		void regrow(u64 new_size)
		{
			if (new_size < ar.length)
			{
				//TODO: call destructor for stuff
				//ar.count = new_size;
			}
			else
			{
				T* aux = ar.start;
				u64 before = ar.count;

				u64 prev_len = ar.length;
				Init(new_size);
				memcpy(ar.start, aux, prev_len * sizeof(T));

				ar.count = before;
				ar.end += before;

				if (aux != nullptr)
					__lang_globals.free(__lang_globals.data, aux);
			}
		}
		T& back()
		{
			return *ar[ar.count - 1];
		}
		void clear()
		{
			ar.Clear();
		}
		void pop_back()
		{
			ar.Pop();
		}
		bool empty()
		{
			return ar.count == 0;
		}
		void resize(u64 len, T&& val)
		{
			regrow(len);
		}
		u64 length()
		{
			return ar.length;
		}
		u64 size()
		{
			return ar.count;
		}

		void reserve(u64 len)
		{
			Init(len);
		}

		T* data()
		{
			return ar.start;
		}

		void remove(u64 idx)
		{
			u64 sz = ar.count - idx;
			ASSERT(sz > 0)
			memmove(ar.start + idx, ar.start + idx + 1, sz * sizeof(T));
			ar.count--;
		}
		void insert(T* at, T* start, T* end)
		{
			u64 a = at - ar.start;
			ASSERT(a >= 0 && a <= this->ar.count && ar.start != nullptr)
				u64 other_len = end - start;

			if ((ar.count + other_len) >= ar.length)
			{
				regrow(ar.count + other_len);
			}
			ar.count += other_len;
			ar.end += other_len;

			u64 diff = ar.count - a;
			char* aux_buffer = (char*)__lang_globals.alloc(__lang_globals.data, diff * sizeof(T));
			memcpy(aux_buffer, ar.start + a, sizeof(T) * diff);

			T* t = ar[a];

			memcpy(t + other_len, aux_buffer, sizeof(T) * (diff - other_len));
			memcpy(t, start, sizeof(T) * other_len);

			if (aux_buffer != nullptr)
				__lang_globals.free(__lang_globals.data, aux_buffer);
		}

		void insert(T* at, u64 count, T&& arg)
		{
			u64 a = at - ar.start;
			ASSERT(a >= 0 && a <= this->ar.count)

				for (u64 i = 0; i < count; i++)
				{
					T val = arg;
					insert(a, val);
				}
		}
		T* insert(u64 a, T& arg)
		{
			ASSERT(a >= 0 && a <= this->ar.count)
				ar.count += 1;
			TestSizeAndRegrow();


			u64 diff = ar.count - a;
			char* aux_buffer = (char*)__lang_globals.alloc(__lang_globals.data, diff * sizeof(T));
			memcpy(aux_buffer, ar.start + a, sizeof(T) * diff);

			T* t = this->ar.start + a;

			if (diff > 0)
			{
				diff = this->ar.count - a - 1;
				memcpy(t + 1, aux_buffer, sizeof(T) * diff);
				__lang_globals.free(__lang_globals.data, aux_buffer);

			}

			memcpy(t, &arg, sizeof(T));

			//ar.count += 1;
			return t;

		}

		void push_back(T& arg)
		{
			emplace_back(arg);
		}
		void TestSizeAndRegrow()
		{
			if ((ar.count) >= ar.length)
			{
				if (ar.length == 0)
				{
					regrow(1);
				}
				regrow(ar.length * 2);
			}
		}
		void emplace_back(T& arg)
		{
			TestSizeAndRegrow();
			ar.Add(&arg);
		}

		void push_back(T&& arg)
		{
			emplace_back(arg);
		}
		void emplace_back(T&& arg)
		{
			TestSizeAndRegrow();
			ar.Add(&arg);
		}
		void free()
		{
			if (ar.start != nullptr)
				__lang_globals.free(__lang_globals.data, ar.start);
		}
		~vector()
		{
			if (ar.start != nullptr)
				__lang_globals.free(__lang_globals.data, ar.start);
		}
	};
}


