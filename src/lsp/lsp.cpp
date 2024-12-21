#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string>
#include "json.hpp"

void Write(HANDLE hFile, char *str, int sz)
{
	int dwBytesWritten = 0;
	auto bErrorFlag = WriteFile(
		hFile,           // open file handle
		str,      // start of data to write
		sz,  // number of bytes to write
		(LPDWORD)&dwBytesWritten, // number of bytes that were written
		NULL);           // no overlapped structure

	if (FALSE == bErrorFlag) {
		//DisplayError(TEXT("WriteFile"));
		printf("Terminal failure: Unable to write to file.\n");
	} 
}

int main()
{
	HANDLE hFile;
	char DataBuffer[] = "message from lsp";
	DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;

	// Create a new file or open an existing file for writing
	hFile = CreateFile("example.txt",                // name of the file
		    FILE_APPEND_DATA,                // open for writing
		    0,                            // do not share
		    NULL,                         // default security
		    CREATE_ALWAYS,                // create new file or overwrite existing
		    FILE_ATTRIBUTE_NORMAL,        // normal file
		    NULL);                        // no attribute template

	if (hFile == INVALID_HANDLE_VALUE) {
		//DisplayError(TEXT("CreateFile"));
		printf("Terminal failure: Unable to open file for write.\n");
		return 1;
	}

	bool first = true;
	printf("Writing %d bytes to example.txt.\n", dwBytesToWrite);
	 // Obtain the standard input handle with overlapped flag
    HANDLE hStdin = CreateFile(
        L"CONIN$",
        GENERIC_READ,
        0, // No sharing
        NULL, // Default security attributes
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL // No template file
    );
	if (hStdin == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening standard input with overlapped flag." << std::endl;
        return 1;
    }
	// Set up the input buffer
	const DWORD bufferSize = 1024;
	char buffer[bufferSize];
	DWORD bytesRead = 1;
	while (true) {
		std::string line;
		// Obtain the standard input handle

		if (first)
			Sleep(10000);
		while (bytesRead != 0)
		{
			// Read data from standard input
			BOOL result = ReadFile(hStdin, buffer, bufferSize - 1, &bytesRead, NULL);
			if (!result) {
				std::cerr << "Error reading from standard input." << std::endl;
				return 1;
			}
			line += buffer;

		}

		Write(hFile, (char*)line.data(), line.size());
		line = "";

        // Process the input
        std::cout << "You entered: " << line << std::endl;
    }
	// Write data to the file
	// Close the file handle
	if (FALSE == CloseHandle(hFile)) {
		//DisplayError(TEXT("CloseHandle"));
		printf("Error closing file handle.\n");
	}

	return 0;
	printf("hello from lsp");
}
