#ifndef __WEB_REQUEST_DOWNLOADER_H__
#define __WEB_REQUEST_DOWNLOADER_H__

/*
	CLASS:
	WebFileDownloader // WebRequestDownloader

	AUTHOR:
	Michael Bell

	PURPOSE: 
	System to background download files outside the DLC system.

*/

#include "CasualCore.h"
#include "../../CasualCore/Services/ServiceManager.h"
#include "../../CasualCore/Services/CasualService.h"
#include "../../lib/CasualCoreOnline/Internal/Include/DLC/Downloader.h"
#include "RK.h"
//#include "RKPointerList.h"

using namespace CasualCore;
using namespace CasualCoreOnline;

#define GLWT_DOWNLOADER

#define MAX_ACTIVE_FILE_DOWNLOADS 1 //Max number of files that can sequentially download at the same time

class WebRequestDownloader : public CasualCore::CasualService
{
	DECLARE_SERVICE(WebRequestDownloader);
public:

	enum WebFileJobType
	{
		WEB_FILE_UNKNOWN,
		WEB_FILE_IMAGE,
		WEB_FILE_XML,
	};

	enum WebFileJobState
	{
		WEB_FILE_STATE_NEW,
		WEB_FILE_STATE_WAITING_TO_DOWNLOAD,
		WEB_FILE_STATE_DOWNLOADING,
		WEB_FILE_STATE_COMPLETED
	};

	struct WebFileJobRequest
	{
		int iHandle;
		std::string strURL;
		std::string strFilePath;
	};

	struct WebFileJob
	{
		WebFileJob()
		{
			iHandle = -1;
			pDownloadRequest = NULL;
		}
		int iHandle;
		WebFileJobState eState;
		std::string strURL;
		std::string strFilePath;
		DownloadRequest* pDownloadRequest;
		Downloader* pDownloader;
		bool bSuccess;

		// GLWebTools objects for downloading files
		glwebtools::UrlConnection   m_Connection;
	};

	WebRequestDownloader();
	virtual ~WebRequestDownloader();

	virtual bool Initialize(bool UseThread = true);
	virtual void Shutdown();

	//Thread safe
	int RequestFile(const char* a_strURL, const char* a_strFilePath, int iHandle = -1);
	WebFileJob* CheckRequestComplete(int a_iFileHandle);
	WebFileJob* PopRequestComplete(int a_iFileHandle);

	// Service update
	virtual void Update(float dt_s);

protected:

	static RKThreadCondition* s_ThreadSleepCondition;
	static RKCriticalSection* s_SleepCriticalSection;
	static RKInterlock* s_ThreadCompleteLock;

	volatile bool	m_bCanWake;
	inline bool IsSleeping() { return !m_bCanWake; }

	RKCriticalSection*  m_pNewMutex;
	RKCriticalSection*  m_pCompletedMutex;
	RKThread*						m_pThread;

	virtual void UpdateThread();
	static uint32 _UpdateThreadStatic(void* a_pParam);

	int m_iTotalHandles;
	int m_iActiveDownloads;
	std::deque<WebFileJobRequest> m_pNewRequests;
	std::deque<WebFileJob*> m_pActiveRequests;
	std::deque<WebFileJob*> m_pCompletedRequests;

	struct ArchivedRequest
	{
		int iHandle;
		int iRefCount;
	};
	RKHashTable<ArchivedRequest*> m_pArchivedRequests;
	int FileAlreadyRequested(const char* a_strURL);

	void BeginJob(WebFileJobRequest* a_pRequest);

	void UpdateJob(WebFileJob* a_pJob);
	void UpdateJob_StateNew(WebFileJob* a_pJob);
	void UpdateJob_StateWaitingToDownload(WebFileJob* a_pJob);
	void UpdateJob_StateDownloading(WebFileJob* a_pJob);

	bool HandleJobSuccess(WebFileJob* a_pJob);
	void HandleJobFailed(WebFileJob* a_pJob, int iErrorCode);

	//Image file functions
	bool IsValidPNG(const char* a_strFileName);

	//nttri
	glwebtools::GlWebTools*     m_pGLWebTools;

};



#endif //__WEB_REQUEST_DOWNLOADER_H__