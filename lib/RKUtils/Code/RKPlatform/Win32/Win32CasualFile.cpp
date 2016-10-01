#include "Win32CasualFile.h"
#include "RKHeap.h"
#include <assert.h>
#include <iostream>
#include <fstream>

namespace CasualCore
{
	Win32CasualFile::Win32CasualFile(const char* filename) : CasualFile(filename)
	{
	}

	Win32CasualFile::~Win32CasualFile()
	{
	}

	unsigned long Win32CasualFile::GetFileSize(const string& filename)
	{
		// Open file
		ifstream filestream(filename);

		filestream.seekg(0, std::ios::end);
		std::streampos length = filestream.tellg();
		// Close file
		filestream.close();
		return static_cast<unsigned long>(length);
	}

	void Win32CasualFile::Read(const string& filename, void* outputBuffer)
	{
		ifstream filestream(filename);

		// Get the length of the file
		filestream.seekg(0, std::ios::end);
		std::streampos length = filestream.tellg();
		filestream.seekg(0, std::ios::beg);

		// Read data from the opened file
		if(0 < length)
		{
			char* buffer = RKNEWARRAY(char, static_cast<unsigned int>(length)); 
			filestream.read(buffer, length); 
			outputBuffer = reinterpret_cast<void*>(buffer); 
		}
		// Should not read an empty file
		else
			assert(false);

		filestream.close();
	}

	void Win32CasualFile::Write(const string& filename, const int elementsize, const int numberofelement, void* inputBuffer)
	{
		assert(NULL != filename.size() > 0);
		std::ofstream filestream(filename.c_str());

		// Dump the input data out to the file
		const char* buffer = reinterpret_cast<const char*>(inputBuffer);
		long size = static_cast<long>(elementsize) * static_cast<long>(numberofelement);
		filestream.write (buffer, size);

		filestream.close();
	}

};