#ifndef __CASUALCORE_WIN32CASUALFILE_H_
#define __CASUALCORE_WIN32CASUALFILE_H_

#include "../CasualFile.h"

namespace CasualCore
{
	using std::ifstream;

	/**
	 * This class is for providing interfaces for reading and writing files on different platform
	 * On the win32 platform, the application use the orx file function
	 */
	class Win32CasualFile : public CasualFile
	{
	public:
		/**
		 * Open the file
		 */
		explicit Win32CasualFile(const char* filename);

		/**
		 * Close the file
		 */
		~Win32CasualFile();

		/**
		 * Override function GetFileSize(), read a file and get the how many bytes in the file
		 */
		virtual unsigned long GetFileSize(const string& filename);

		/**
		 * Override function Read(), read data from the file
		 */
		virtual void Read(const string& filename, void* outputBuffer);

		/**
		 * Override function Write(), write data into the file
		 */
		virtual void Write(const string& filename, const int elementsize, const int numberofelement, void* inputBuffer);
	};
}
#endif __CASUALCORE_WIN32CASUALFILE_H_