#include "Win8Utils.h"
#include <Windows.h>

using namespace Platform;
using namespace Windows::Storage;
using namespace Windows::Foundation;
using namespace Windows::UI::Notifications;
using namespace Windows::Data::Xml::Dom;
using namespace std;

Windows::Data::Xml::Dom::XmlDocument^ GetXmlDocumentFromFile(const wstring& root, const wstring& file)
{
	// Parameters checking.
	if (root.empty() || (root.compare(L"ms-appdata:///") != 0 && root.compare(L"ms-appx:///") != 0))
	{
		std::wstring w(L"ERROR - Empty/invalid root string.\n");

		OutputDebugString(w.c_str());

		return nullptr;
	}
	if (file.empty())
	{
		std::wstring w(L"ERROR - File string is empty.\n");

		OutputDebugString(w.c_str());

		return nullptr;
	}

	String^ psFile = ref new String(file.c_str());
	String^ psRoot = ref new String(root.c_str());

	// File retrieval.
	StorageFile^ storageFile;
	
	try
	{
		Uri^ xmlLocation = ref new Uri(psRoot, psFile);

		storageFile = PerformSynchronously(StorageFile::GetFileFromApplicationUriAsync(xmlLocation));
	}
	catch (Exception^ e)
	{
		// Error handling.
		std::wstring ws(L"ERROR - Can't open file. Reason: ");
		ws += e->Message->Data();

		OutputDebugString(ws.c_str());
		return nullptr;
	}

	if (storageFile == nullptr)
	{
		OutputDebugString(L"ERROR - Can't open file.\n");
		return nullptr;
	}

	// Content reading.
	String^ buff;

	try
	{
		buff = PerformSynchronously(FileIO::ReadTextAsync(storageFile));
	}
	catch (Exception^ e)
	{
		// Error handling.
		std::wstring ws(L"ERROR - Can't read file. Reason: ");
		ws += e->Message->Data();

		OutputDebugString(ws.c_str());
		return nullptr;
	}

	if (buff == nullptr)
	{
		OutputDebugString(L"ERROR - Can't read file.\n");
		return nullptr;
	}

	// Content conversion to XmlDocumnet.
	XmlDocument^ xmlDoc = ref new XmlDocument;

	try
	{
		xmlDoc->LoadXml(buff);

		if (xmlDoc == nullptr || xmlDoc->GetXml()->IsEmpty())
		{
			OutputDebugString(L"ERROR - Can't process file into xml.\n");
			return nullptr;
		}
	}
	catch (Exception^ e)
	{
		// Error handling.
		std::wstring ws(L"ERROR - Can't process file into xml. Reason: ");
		ws += e->Message->Data();

		OutputDebugString(ws.c_str());
		return nullptr;
	}

	return xmlDoc;
}

bool UTF16FromUTF8(const std::string& utf8, std::wstring& utf16)
{
	using namespace std;

	// Special case of empty input string
	if (utf8.empty())
	{
		OutputDebugString(L"WARNING - Input string is empty.\n");

		utf16 = L"";

		return true;
	}

	// Fail if an invalid input character is encountered
	const DWORD conversionFlags = MB_ERR_INVALID_CHARS;


	// Get length (in wchar_t's) of resulting UTF-16 string
	const int utf16Length = ::MultiByteToWideChar(
		CP_UTF8,            // convert from UTF-8
		conversionFlags,    // flags
		utf8.data(),        // source UTF-8 string
		utf8.length(),      // length (in chars) of source UTF-8 string
		NULL,               // unused - no conversion done in this step
		0                   // request size of destination buffer, in wchar_t's
		);
	if (utf16Length == 0)
	{
		// Error handling.
		char errorMessage[255];

		// Get the error code.
		DWORD errorCode = GetLastError();

		sprintf_s(errorMessage,
			(errorCode == ERROR_NO_UNICODE_TRANSLATION) ?
			"Invalid UTF-8 sequence found in input string." :
			"Can't get length of UTF-16 string (MultiByteToWideChar failed).");

		string s(errorMessage);
		wstring w(s.begin(), s.end());

		OutputDebugString(w.c_str());

		return false;
	}

	// Allocate destination buffer for UTF-16 string
	utf16.resize(utf16Length);

	// Do the conversion from UTF-8 to UTF-16
	if (!::MultiByteToWideChar(
		CP_UTF8,            // convert from UTF-8
		0,                  // validation was done in previous call, 
		// so speed up things with default flags
		utf8.data(),        // source UTF-8 string
		utf8.length(),      // length (in chars) of source UTF-8 string
		&utf16[0],          // destination buffer
		utf16.length()      // size of destination buffer, in wchar_t's
		))
	{
		// Error handling.
		char errorMessage[255];

		// Get the error code.
		DWORD errorCode = GetLastError();

		sprintf_s(errorMessage, "Can't convert string from UTF-8 to UTF-16 (MultiByteToWideChar failed). Error code: %ui", errorCode);

		string s(errorMessage);
		wstring w(s.begin(), s.end());

		OutputDebugString(w.c_str());

		utf16 = L"";

		return false;
	}

	// Success
	return true;
}

bool UTF8FromUTF16(const std::wstring& utf16, std::string& utf8)
{
	using namespace std;

	// Special case of empty input string
	if (utf16.empty())
	{
		OutputDebugString(L"WARNING - Input string is empty.\n");

		utf8 = "";

		return true;
	}


	// Get length (in chars) of resulting UTF-8 string
	const int utf8Length = ::WideCharToMultiByte(
		CP_UTF8,            // convert to UTF-8
		0,                  // default flags
		utf16.data(),       // source UTF-16 string
		utf16.length(),     // source string length, in wchar_t's,
		NULL,               // unused - no conversion required in this step
		0,                  // request buffer size
		NULL, NULL          // unused
		);
	if (utf8Length == 0)
	{
		// Error handling.
		char errorMessage[255];

		// Get the error code.
		DWORD errorCode = GetLastError();

		sprintf_s(errorMessage, "Can't get length of UTF-8 string (WideCharToMultiByte failed). Error code: %ui", errorCode);

		string s(errorMessage);
		wstring w(s.begin(), s.end());

		OutputDebugString(w.c_str());

		utf8 = "";

		return false;
	}

	// Allocate destination buffer for UTF-8 string
	utf8.resize(utf8Length);

	// Do the conversion from UTF-16 to UTF-8
	if (!::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		0,                      // default flags
		utf16.data(),           // source UTF-16 string
		utf16.length(),         // source string length, in wchar_t's,
		&utf8[0],               // destination buffer
		utf8.length(),          // destination buffer size, in chars
		NULL, NULL              // unused
		))
	{
		// Error handling.
		char errorMessage[255];

		// Get the error code.
		DWORD errorCode = GetLastError();

		sprintf_s(errorMessage, "Can't convert string from UTF-16 to UTF-8 (WideCharToMultiByte failed). Error code: %ui", errorCode);

		string s(errorMessage);
		wstring w(s.begin(), s.end());

		OutputDebugString(w.c_str());

		return false;
	}

	// Success
	return true;
}
