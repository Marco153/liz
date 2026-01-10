#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#define LINUX
char* ReadEntireFileMalloc(char* name, unsigned int* read_out)
{
#ifdef LINUX
	int fh, n;
	struct stat v;

	fh = open(name, O_RDONLY);
	if (fh == -1) {
		perror("open");
		printf("file not found\"%s\"", name);
		close(fh);
		return nullptr;
	}

	/* first find the size of file .. use stat() system call */
	stat(name, &v);

	char* string = (char*)malloc(v.st_size + 1);
	int res=read(fh, string, v.st_size);
	if (res == -1) {
		perror("read");
		close(fh);
		return nullptr;
	}
	close(fh);

	string[v.st_size] = 0;
	*read_out = v.st_size + 1;
	return string;
#else
	HANDLE file = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (file == nullptr )
	{
		printf("file \"%s\" not found", name);
		ExitProcess(0);
	}
	if (file == INVALID_HANDLE_VALUE)
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, buffer);
		//TCHAR full_file_path[MAX_PATH] = { 0 };
		//GetFullPathName(filename, MAX_PATH, fullFilename, nullptr);


		printf("file \"%s\" was invalid handle, last error %d, full exe path %s", name, GetLastError(), buffer);

		ExitProcess(0);
	}

	LARGE_INTEGER file_size;
	GetFileSizeEx(file, &file_size);
	char* f = (char*)__lang_globals.alloc(__lang_globals.data, file_size.QuadPart + 1);

	int bytes_read;
	ReadFile(file, (void*)f, file_size.QuadPart, (LPDWORD)&bytes_read, 0);
	f[file_size.QuadPart] = 0;

	*read = bytes_read;

	CloseHandle(file);
	return f;
#endif
}
void make_mips()
{
#define CHUNK_SZ 128
  int WIDTH = 8192;

  u32 file_sz;

  char *file = ReadEntireFileMalloc("/home/marco/terrain/map1.data", &file_sz);

  ASSERT(file_sz == (WIDTH * WIDTH))

  char file_name[128];
  char buffer[];
  int cur_ch_sz = CHUNK_SZ;
  for(int m = 0; m < 5; m++)
  {
    int chunks_axis_total = WIDTH / cur_ch_sz;
    for(int y = 0; y < chunks_axis_total; y++)
    {
      for(int x = 0; x < chunks_axis_total; x++)
      {

      }
    }
    cur_ch_sz *= 2;
  }
  WriteFileLang(vw, void *data, int size)
}
