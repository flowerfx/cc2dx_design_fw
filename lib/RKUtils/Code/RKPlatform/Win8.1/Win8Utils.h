#ifndef WIN_8_UTILS_H
#define  WIN_8_UTILS_H

#include <Windows.h>
#include <string>
#include <agents.h>

// Template for performing IAsyncOperations synchronously. Use at own peril :)
template <typename TResult>
TResult PerformSynchronously(Windows::Foundation::IAsyncOperation<TResult>^ asyncOperation)
{
	// Agent that waits for a task to complete.
	class CustomAgent : public Concurrency::agent
	{
	public:
		CustomAgent(Windows::Foundation::IAsyncOperation<TResult>^ asyncOperation) :
			operation(asyncOperation)
		{}

		void run()
		{
			/* This is mostly done for exception catching, although it seems it's not perferct.
			 * It doesn't catch IAsyncOperation creation exceptions such as invalid parameters,
			 * the code calling this function should take care of this problem.
			 */
			operation->Completed = ref new Windows::Foundation::AsyncOperationCompletedHandler<TResult>
				([this](Windows::Foundation::IAsyncOperation<TResult>^ operation, Windows::Foundation::AsyncStatus status)
			{
				// When	the IAsyncOperation finishes notify the agent.
				this->done();
			});

		}
	private:
		Windows::Foundation::IAsyncOperation<TResult>^ operation;
	};
	// Agent successful creation checks.
	CustomAgent* ag = new CustomAgent(asyncOperation);

	if (ag == nullptr)
	{
		OutputDebugString(L"ERROR - Could not create the task agent.\n");
		return nullptr;
	}

	if (ag->start() == false)
	{
		OutputDebugString(L"ERROR - Could not start the task agent.\n");
		return nullptr;
	}

	// Wait for the task to complete.
	try
	{
		Concurrency::agent::wait(ag);
	}
	catch (Platform::Exception^ e)
	{
		// Error handling.
		std::wstring ws(L"ERROR - Task execution failed. Reason: ");
		ws += e->Message->Data();

		OutputDebugString(ws.c_str());
		return nullptr;
	}
	catch (std::exception e)
	{
		// Error handling.
		std::string s("ERROR - Task execution failed. Reason: ");
		s += e.what();

		std::wstring w(s.begin(), s.end());
		OutputDebugString(w.c_str());
		return nullptr;
	}

	// Process operation result.
	switch (asyncOperation->Status)
	{
	case Windows::Foundation::AsyncStatus::Completed:
		return asyncOperation->GetResults();

	case Windows::Foundation::AsyncStatus::Canceled:
		OutputDebugString(L"ERROR - The task was canceled.\n");

	case Windows::Foundation::AsyncStatus::Error:
	{
		std::wstring ws(L"ERROR - Task execution failed. Reason: ");
		Platform::Exception^ e = ref new Platform::Exception(asyncOperation->ErrorCode.Value);
		ws += e->Message->Data();

		OutputDebugString(ws.c_str());
	}

	default:
		return nullptr;
	}
}

/* Loads the contents of a file into a Microsoft specific XmlDocument.
 * Root should be "ms-appx:///" for the application package 
 *			   or "ms-appdata:///" for the application local folder.
 */
Windows::Data::Xml::Dom::XmlDocument^ GetXmlDocumentFromFile(const std::wstring& root, const std::wstring& file);


// If the function fails the output (wide)string is empty.
bool UTF16FromUTF8(const std::string& utf8, std::wstring& utf16);
bool UTF8FromUTF16(const std::wstring& utf16, std::string& utf8);

#endif