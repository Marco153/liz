#pragma once
#include "Array.h"
#include <unordered_map>

#define CHUNK_FREE 1
//#define CHUNK_IN_USE 2
#define CHUNK_ALLOCATED 4

struct mem_chunk
{
    mem_chunk *next;
    mem_chunk *prev;
    int size;
    int flags;
    char *addr;
};

#define BYTES_PER_CHUNK  8
#define UNALLOCATED_BUFFER_ITEMS  8

struct heap_hash
{
	struct inner
	{
		void* key;
		void* value;
	};
	inner *data;
	unsigned int hash_table_size = (1024 * 1024);

	void Clear()
	{
		memset(data, 0, sizeof(inner) * hash_table_size);
	}

	void *Get(void* key)
	{
		int idx = ((long long)key) % hash_table_size;
		auto cur = &data[idx];

		if (cur->key == key)
		{
			return cur->value;
		}
		bool put = false;
		for (int i = 0; i < hash_table_size; i++)
		{
			int mod = (i + idx + 1) % hash_table_size;
			cur = &data[mod];

			if (cur->key == key)
			{
				return cur->value;
			}
		}
		return nullptr;
		ASSERT(put);
	}
	void Remove(void* key)
	{
		int idx = ((long long)key) % hash_table_size;
		auto cur = &data[idx];

		if (cur->key == key)
		{
			cur->key = nullptr;
			return;
		}
		bool removed = false;
		for (int i = 0; i < hash_table_size; i++)
		{
			int mod = (i + idx + 1) % hash_table_size;
			cur = &data[mod];

			if (cur->key == key)
			{
				cur->key = nullptr;
				removed = true;
				break;
			}
		}
		ASSERT(removed);
	}
	void Store(void *key, void *value)
	{
		int idx = ((long long)key) % hash_table_size;
		auto cur = &data[idx];

		if (cur->key == nullptr)
		{
			cur->key = key;
			cur->value = value;
			return;
		}
		bool put = false;
		for (int i = 0; i < hash_table_size; i++)
		{
			int mod = (i + idx + 1) % hash_table_size;
			cur = &data[mod];

			if (cur->key == nullptr)
			{
				cur->key = key;
				cur->value = value;
				put = true;
				break;
			}
		}
		ASSERT(put);
	}
};

struct mem_alloc
{
    char *buffer;
    mem_chunk *head_free;
    heap_hash in_use;
    mem_chunk *all;
    mem_chunk **probable_unallocated;
	unsigned int chunks_cap =(1024 * 1024 * 64);

	char* main_buffer;
};
void InitMemAlloc(mem_alloc* alloc);
void FreeMemAlloc(mem_alloc* alloc);
