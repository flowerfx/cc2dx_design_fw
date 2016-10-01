#include "WebRequestDownloader.h"

#include "../../CasualCore/Tools/ImageUrl.h"
#include "DLC/Downloader.h"
#include "DownloadRequest.h"

#ifndef GLWT_DOWNLOADER
#include "../../IceAgeAdventure/Utils/jpge/jpgd.h"
#include "../../IceAgeAdventure/Utils/jpge/pngwriter.h"
#endif

#include "RKFile.h"
#include "RKThread.h"

using namespace CasualCore;

RKThreadCondition* WebRequestDownloader::s_ThreadSleepCondition = 0;
RKCriticalSection* WebRequestDownloader::s_SleepCriticalSection = 0;
RKInterlock* WebRequestDownloader::s_ThreadCompleteLock = 0;

static char _PNG_StartTag[8] = {137,  80,  78,  71,  13,  10,  26,  10}; //Start Tag
static char _PNG_EndTag[4] = {'I', 'E', 'N', 'D'}; //End chunk tag to search for.

DEFINE_SERVICE(WebRequestDownloader);

WebRequestDownloader::WebRequestDownloader()
	: CasualCore::CasualService()
	, m_iTotalHandles(0)
	, m_iActiveDownloads(0)
	, m_pNewMutex(NULL)
	, m_pCompletedMutex(NULL)
	, m_bCanWake(false)
	, m_pGLWebTools(NULL)
	, m_pThread(NULL)
{
}


WebRequestDownloader::~WebRequestDownloader()
{

}


bool WebRequestDownloader::Initialize(bool UseThread)
{
	CasualService::Initialize();

	//Create Mutex's
	m_pNewMutex = RKCriticalSection_Create("WBF_New");
	m_pCompletedMutex = RKCriticalSection_Create("WBF_Completed");

	//Create Archive Map
	m_pArchivedRequests.Init(200);

	s_ThreadSleepCondition = RKThreadCondition_Create("WebRequestDownloader::s_ThreadSleepConditionVariable");
	s_ThreadCompleteLock = RKInterlock_Create("WebRequestDownloader::s_ThreadCompleteLock");
	s_SleepCriticalSection = RKCriticalSection_Create("WebRequestDownloader::s_SleepCriticalSection");
	if (UseThread)
	{
		m_pThread = RKThread_Create("DownloadThread", _UpdateThreadStatic, this, RKThreadPriority_Normal);
		RKThread_Start(m_pThread);
	}
	else
	{
		m_pThread = NULL;
	}

#ifdef GLWT_DOWNLOADER
	m_pGLWebTools = new glwebtools::GlWebTools();
	RKASSERT(m_pGLWebTools, "Failed to create GLWebTools");
	glwebtools::GlWebTools::CreationSettings cs = glwebtools::GlWebTools::CreationSettings();
	bool bGLWT = m_pGLWebTools->Initialize(cs) == glwebtools::E_SUCCESS;
	RKASSERT(bGLWT, "[WFD] - ERROR - Could not Initialize GLWebTools");	
#endif

	return true;
}


void WebRequestDownloader::Shutdown()
{
	//Wake so we can shut down
	if (s_ThreadCompleteLock)
		RKInterlock_Lock(s_ThreadCompleteLock);
	m_bCanWake = true;
	if (s_ThreadSleepCondition)
		RKThreadCondition_WakeAll(s_ThreadSleepCondition);
	if (m_pThread)
		RKThread_WaitForExit(m_pThread);

	// Destroy global locks
	if (s_ThreadSleepCondition)
		RKThreadCondition_Destroy(&s_ThreadSleepCondition);
	if (s_ThreadCompleteLock)
		RKInterlock_Destroy(&s_ThreadCompleteLock);
	if (s_SleepCriticalSection)
		RKCriticalSection_Destroy(&s_SleepCriticalSection);

	if (m_pThread)
		RKThread_Destroy(&m_pThread);
	if (m_pNewMutex)
		RKCriticalSection_Destroy(&m_pNewMutex);
	if (m_pCompletedMutex)
		RKCriticalSection_Destroy(&m_pCompletedMutex);

	m_pArchivedRequests.Clear();
	m_pArchivedRequests.Deinit();

	CasualService::Shutdown();
}


void WebRequestDownloader::Update(float dt_s)
{
	//Wake
	if (m_pThread)
	{
		RKThreadCondition_WakeAll(WebRequestDownloader::s_ThreadSleepCondition);
		m_bCanWake = true;
	}
	else
	{
		UpdateThread();
	}
}

//Request a file
//Returns a handle that can be used to access the result
int WebRequestDownloader::RequestFile(const char* a_strURL, const char* a_strFilePath, int iHandle)
{
	int iReturnHandle = FileAlreadyRequested(a_strURL);
	if (iReturnHandle != -1)
		return iReturnHandle;

	//Handle bad symbols in file path names
	std::string strFilePath = a_strFilePath;
	while (strFilePath.find(":") != std::string::npos)
		strFilePath.replace(strFilePath.find(":"), 1, "_");
	while (strFilePath.find("//") != std::string::npos)
		strFilePath.replace(strFilePath.find("//"), 1, "_");

	RKCriticalSection_Enter(m_pNewMutex);
	
	//Create Request
	WebFileJobRequest requestFile;
	requestFile.strURL = a_strURL;
	requestFile.strFilePath = strFilePath;
	requestFile.iHandle = iReturnHandle = m_iTotalHandles;
	if (iHandle != -1)
	{
		requestFile.iHandle = iHandle;
	}
	m_iTotalHandles++;
	m_pNewRequests.push_back(requestFile);

	//Create Archive Entry
	ArchivedRequest* pNewArchive = new ArchivedRequest;
	pNewArchive->iHandle = requestFile.iHandle;
	pNewArchive->iRefCount = 1;
	m_pArchivedRequests.Insert(pNewArchive, a_strURL);


	RKCriticalSection_Leave(m_pNewMutex);
	return iReturnHandle;
}

//Gets the completed request and leaves it on the list
//Mainly useful for cases when multiple sources need the same file/request (Social avatars...)
//Returns null if still processing
WebRequestDownloader::WebFileJob* WebRequestDownloader::CheckRequestComplete(int a_iFileHandle)
{
	if (a_iFileHandle < 0)
		return NULL;

	WebFileJob* pResult = NULL;

	RKCriticalSection_Enter(m_pCompletedMutex);
	for (unsigned int i = 0; i < m_pCompletedRequests.size(); i++)
	{
		if (m_pCompletedRequests[i]->iHandle == a_iFileHandle /*&& m_pCompletedRequests[i]->bSuccess*/)
		{
			pResult = m_pCompletedRequests[i];
			break;
		}
	}
	RKCriticalSection_Leave(m_pCompletedMutex);

	return pResult;
}

//Gets the completed request and pops it off the list
//Returns null if still processing
WebRequestDownloader::WebFileJob* WebRequestDownloader::PopRequestComplete(int a_iFileHandle)
{
	WebFileJob* pResult = NULL;

	//TODO
	//RKCriticalSection_Enter(m_pCompletedMutex);
	//for (unsigned int i = 0; i < m_pCompletedRequests.size(); i++)
	//{
	//	if (m_pCompletedRequests[i]->iHandle == a_iFileHandle)
	//	{
	//		pResult = m_pCompletedRequests[i];
	//		m_pCompletedRequests[i] = m_pCompletedRequests.back();
	//		m_pCompletedRequests.pop_back();
	//		break;
	//	}
	//}
	//RKCriticalSection_Leave(m_pCompletedMutex);

	return pResult;
}


//Check if a file is already requested
//MAIN THREAD ONLY
int WebRequestDownloader::FileAlreadyRequested(const char* a_strURL)
{
	ArchivedRequest* pRequest = m_pArchivedRequests.Query(a_strURL);
	if (pRequest)
	{
		pRequest->iRefCount++;
		return pRequest->iHandle;
	}
	return -1;
}

//Threaded Update
uint32 WebRequestDownloader::_UpdateThreadStatic(void* a_pParam)
{
	RKTHREAD_ENTER

	WebRequestDownloader* pService = (WebRequestDownloader*)a_pParam;
	if (pService)
	{
		while (s_ThreadCompleteLock && RKInterlock_Query(s_ThreadCompleteLock) == 0)
		{
			//RKCriticalSection_Enter(WebRequestDownloader::s_SleepCriticalSection);
			//while (pService->IsSleeping())
			//	RKThreadCondition_Sleep(WebRequestDownloader::s_ThreadSleepCondition, WebRequestDownloader::s_SleepCriticalSection);
			//RKCriticalSection_Leave(WebRequestDownloader::s_SleepCriticalSection);

			pService->UpdateThread();
			pService->m_bCanWake = false;
		}
	}

	RKTHREAD_EXIT

	return 0;
}


//ON THREAD
void WebRequestDownloader::UpdateThread()
{
	//Check for new requests
	RKCriticalSection_Enter(m_pNewMutex);
	for (unsigned int i = 0; i < m_pNewRequests.size(); i++)
		BeginJob(&m_pNewRequests[i]);
	m_pNewRequests.clear();
	RKCriticalSection_Leave(m_pNewMutex);

	//Update Jobs
	for (unsigned int i = 0; i < m_pActiveRequests.size(); i++)
		UpdateJob(m_pActiveRequests[i]);

	//Handle completed jobs
	for (unsigned int i = 0; i < m_pActiveRequests.size(); i++)
	{
		if (m_pActiveRequests[i]->eState == WEB_FILE_STATE_COMPLETED)
		{
			//Get off list
			WebFileJob* pDone = m_pActiveRequests[i];
			m_pActiveRequests[i] = m_pActiveRequests.back();
			m_pActiveRequests.pop_back();
			i--;

			//Push onto completed list
			RKCriticalSection_Enter(m_pCompletedMutex);
			m_pCompletedRequests.push_back(pDone);
			RKCriticalSection_Leave(m_pCompletedMutex);
		}
	}
}

//ON THREAD
void WebRequestDownloader::BeginJob(WebFileJobRequest* a_pRequest)
{
	WebFileJob* pNewJob = new WebFileJob;
	pNewJob->iHandle = a_pRequest->iHandle;
	pNewJob->strURL = a_pRequest->strURL;
	pNewJob->strFilePath = a_pRequest->strFilePath;
	pNewJob->eState = WEB_FILE_STATE_NEW;
	pNewJob->bSuccess = false;
	m_pActiveRequests.push_back(pNewJob);
}


//ON THREAD
void WebRequestDownloader::UpdateJob(WebFileJob* a_pJob)
{
	switch (a_pJob->eState)
	{
	case WEB_FILE_STATE_NEW:
		UpdateJob_StateNew(a_pJob);
		break;
	case WEB_FILE_STATE_WAITING_TO_DOWNLOAD:
		UpdateJob_StateWaitingToDownload(a_pJob);
		break;
	case WEB_FILE_STATE_DOWNLOADING:
		UpdateJob_StateDownloading(a_pJob);
		break;
	}
}

//ON THREAD
void WebRequestDownloader::UpdateJob_StateNew(WebFileJob* a_pJob)
{
	//Check if already exists on disk...
	RKString strAbsPath = RKFile_GetDocumentsPath();
	RKString strLowerCaseName = a_pJob->strFilePath.c_str();
	strLowerCaseName.ToLowerCase();

	//We always convert jpgs to pngs..
	char strExtension[16];
	memset(strExtension, 0, sizeof(char) * 16);
	RKString_ExtractFileExtension(strLowerCaseName.GetString(), strExtension);
	if (strcmp(strExtension, "jpg") == 0)
		strLowerCaseName.ReplaceFirst(".jpg", ".png");

	//Check
	strAbsPath.Append(strLowerCaseName);
	if (RKFile_ExistsAbs(strAbsPath.GetString()))
	{
		if (IsValidPNG(strLowerCaseName.GetString()))
		{
			a_pJob->strFilePath = strLowerCaseName.GetString();
			a_pJob->bSuccess = true;
			a_pJob->eState = WEB_FILE_STATE_COMPLETED;
			return;
		}
		else
			RKFile_Remove(strLowerCaseName.GetString());
	}

	//Ready to download
	a_pJob->eState = WEB_FILE_STATE_WAITING_TO_DOWNLOAD;
}

//ON THREAD
void WebRequestDownloader::UpdateJob_StateWaitingToDownload(WebFileJob* a_pJob)
{
	//Ready?
	if (m_iActiveDownloads < MAX_ACTIVE_FILE_DOWNLOADS)
	{
#ifdef GLWT_DOWNLOADER
		a_pJob->m_Connection = m_pGLWebTools->CreateUrlConnection();
		if (a_pJob->m_Connection.IsHandleValid())
		{
			glwebtools::UrlRequest::CreationSettings cs;
			//cs.m_outFilename = "c:\\download.tmp";
			glwebtools::UrlRequest Request = m_pGLWebTools->CreateUrlRequest(cs);
			if (!Request.IsHandleValid())
			{
				RKASSERT(false, "[WFD] - ERROR - Could not Create Request");
			}

			Request.SetUrl(a_pJob->strURL);
			Request.SetMethod(glwebtools::UrlRequest::M_GET);
			glwebtools::Error error = a_pJob->m_Connection.StartRequest(Request);
			Request.Release();
		}
#else
		//Start the request
		a_pJob->pDownloader = new Downloader();
		a_pJob->pDownloader->Initialize();
		a_pJob->pDownloadRequest = new CasualCore::DownloadRequest(-1);
		*a_pJob->pDownloadRequest = a_pJob->pDownloader->GetURL(a_pJob->strURL.c_str(), a_pJob->strFilePath.c_str());
#endif
		a_pJob->eState = WEB_FILE_STATE_DOWNLOADING;
		m_iActiveDownloads++;
	}
}

//ON THREAD
void WebRequestDownloader::UpdateJob_StateDownloading(WebFileJob* a_pJob)
{
#ifdef GLWT_DOWNLOADER
	glwebtools::UrlConnection::State currentState = a_pJob->m_Connection.GetState();

	//if(currentState != glwebtools::UrlConnection::State::S_IDLE)
	//{
		if(currentState != glwebtools::UrlConnection::State::S_RUNNING)
		{
			bool bSuccess = false;
			int statusCode = 0;

			// read response data
			glwebtools::UrlResponse Response = a_pJob->m_Connection.GetUrlResponse();

			if (Response.IsHandleValid())
			{
				// read repsonse data
				statusCode = static_cast<int>(Response.GetResponseCode());
				if (statusCode == 304)
				{
					//a_pJob->eState = WEB_FILE_STATE_WAITING_TO_DOWNLOAD;
					//m_iActiveDownloads--;
					//return;
				}
				bSuccess = (Response.GetResponseCode() == 200);
				size_t uSize=0;
				const void* pData=0;
				a_pJob->eState = WEB_FILE_STATE_COMPLETED;
				if (bSuccess && Response.GetData(pData, uSize) == 0)
				{
					
					//////////////////////
					//std::string strTargetFileName = a_pJob->strFilePath;
					RKString str = a_pJob->strFilePath.c_str();
					RKList<RKString> list_str = str.Split("\\");
					const char * strTargetFileName = list_str.GetAt(list_str.Size() - 1).GetString();
					RKFile* pFile = RKFile_Open(strTargetFileName, RKFM_WRITE);
					if (pFile)
					{
						RKFile_Write(pFile, (void*)pData, uSize);
						RKFile_Close(&pFile);
					}
					a_pJob->bSuccess = true;
					m_iActiveDownloads--;
					a_pJob->m_Connection.Release();
					return;
				}
				else
				{
					// we failed
					HandleJobFailed(a_pJob, statusCode);
					m_iActiveDownloads--;
					a_pJob->m_Connection.Release();
					return;
				}
			}
		}
	//}
#else
	DownloadRequest::DownloadRequestState currentState = a_pJob->pDownloadRequest->GetState();

	if (currentState != DownloadRequest::S_IDLE)
	{
		if (currentState != DownloadRequest::S_RUNNING)
		{
			// request finish, either success or failure
			bool bSuccess = false;
			int statusCode = 0;

			// read response data
			statusCode = a_pJob->pDownloadRequest->GetResponseCode();
			a_pJob->bSuccess = a_pJob->pDownloadRequest->IsSuccess();
			size_t uSize=0;
			const void* pData=0;

			a_pJob->eState = WEB_FILE_STATE_COMPLETED;
			if (!a_pJob->bSuccess || statusCode >= 400LL || currentState == DownloadRequest::S_ERROR)
			{
				// we failed
				HandleJobFailed(a_pJob, statusCode);
				m_iActiveDownloads--;
				a_pJob->pDownloader->Shutdown();
				delete a_pJob->pDownloader;
				a_pJob->pDownloader = NULL;
				return;
			}
			else 
			{
				// succeeded
				if (!HandleJobSuccess(a_pJob))
					a_pJob->bSuccess = false;
				m_iActiveDownloads--;
				a_pJob->pDownloader->Shutdown();
				delete a_pJob->pDownloader;
				a_pJob->pDownloader = NULL;
				return;
			}        
		}
	}
#endif
}


//ON THREAD
bool WebRequestDownloader::HandleJobSuccess(WebFileJob* a_pJob)
{
#ifndef GLWT_DOWNLOADER
	//Download was successful, get file name info
	std::string strTargetFileName = a_pJob->strFilePath;
	
	RKString sFileBase = "";
	RKString sTempFile = "";
	RKString sHeaderFile = "";
	RKString strLocal ="";
	RKString strFinalRename = "";
	char sTmpPath[256];

	memset(sTmpPath,0,sizeof(char)*256);
	RKString sExtension;
	RKString_ExtractFileBase(strTargetFileName.c_str(), sTmpPath);
	sTempFile = sTmpPath;
	sFileBase = sTmpPath;
	memset(sTmpPath,0,sizeof(char)*256);
	RKString_ExtractFileExtension(strTargetFileName.c_str(), sTmpPath);
	sExtension = sTmpPath;
	sTempFile.Append("_");
	sTempFile.Append(sExtension);
	sHeaderFile = sTempFile;
	sTempFile.Append(".tmp");
	sHeaderFile.Append(".hdr");

	//Open the temp file and extract the data
	if (RKFile_Exists(sTempFile.GetString()))
	{
		//Get Data
		RKFile* pFile = RKFile_Open(sTempFile.GetString(), RKFM_READ);
		uint32 uiSize = RKFile_GetSize(pFile);
		unsigned char* cBuffer = new unsigned char[uiSize];
		RKFile_Read(pFile, cBuffer, uiSize);
		RKFile_Close(&pFile);

		//Convert from JPEG?
		if (stricmp(sExtension.GetString(), "jpg") == 0)
		{
			int width = 0;
			int height = 0;
			int actual_comps = 0;  

			//Decompress JPEG into memory
			unsigned char *pImage = jpgd::decompress_jpeg_image_from_memory(cBuffer, uiSize, &width, &height, &actual_comps, 3);
			if (!pImage || uiSize == 0 || width == 0 || height == 0)
			{
				delete cBuffer;
				if (RKFile_Exists(sTempFile.GetString()))
					RKFile_Remove(sTempFile.GetString());
				if (RKFile_Exists(sHeaderFile.GetString()))
					RKFile_Remove(sHeaderFile.GetString());
				return false;
			}

			//Switch to
			strFinalRename = sFileBase;
			strFinalRename.Append(".png");
			strFinalRename.ToLowerCase();
			a_pJob->strFilePath = strFinalRename.GetString();

			strLocal = strFinalRename;
			strLocal.Append("_rename.tmp");
			RKFile* f = RKFile_Open(strLocal.GetString(), RKFM_WRITE); // open file to properly set iOS backup file attributes
			RKASSERT(f, "Unable to open file for writing");
			if(f)
			{
				RKFile_Close(&f);
			}

			//Write out PNG
			pngwriter writer(width, height, 0, strLocal.GetString());
			int offs = 0;
			for (int y=height-1; y>=0; --y)
			{
				for (int x=0; x<width; ++x)
				{
					writer.plot(x,y,(unsigned char)pImage[offs] * 256,(unsigned char)pImage[offs+1] * 256,pImage[offs+2] * 256);
					offs +=3;
				}
			}

			writer.scale_k(0.80);
			writer.write_png();
			free(pImage);
		}
		else
		{
			//Switch to lowercase
			strFinalRename = strTargetFileName;
			strFinalRename.ToLowerCase();
			strLocal = strFinalRename;
			strLocal.Append("_rename.tmp");
			a_pJob->strFilePath = strFinalRename.GetString();

			//Save as new file
			pFile = RKFile_Open(strLocal.GetString(), RKFM_WRITE);
			if (pFile)
			{
				RKFile_Write(pFile, cBuffer, uiSize);
				RKFile_Close(&pFile);
			}
		}

		//Clear Buffer
		delete cBuffer;
	}
	else
	{
		if (RKFile_Exists(sTempFile.GetString()))
			RKFile_Remove(sTempFile.GetString());
		if (RKFile_Exists(sHeaderFile.GetString()))
			RKFile_Remove(sHeaderFile.GetString());
		return false;
	}

	//Rename the final result
	RKString strFinalRenamePath = RKFile_GetSupportFilesPath();
	RKString strLocalPath = RKFile_GetSupportFilesPath();
	strFinalRenamePath.Append(strFinalRename);
	strLocalPath.Append(strLocal);

	if (RKFile_Exists(strFinalRenamePath.GetString()))
		RKFile_Remove(strFinalRenamePath.GetString());
	RKFile_RenameAbs(strLocalPath.GetString(), strFinalRenamePath.GetString());

	//Remove the temp files
	if (RKFile_Exists(sTempFile.GetString()))
		RKFile_Remove(sTempFile.GetString());
	if (RKFile_Exists(sHeaderFile.GetString()))
		RKFile_Remove(sHeaderFile.GetString());

	//Success
	return true;
#else
	return false;
#endif
}


//ON THREAD
void WebRequestDownloader::HandleJobFailed(WebFileJob* a_pJob, int iErrorCode)
{
	//Remove the temp files
	std::string strTargetFileName = a_pJob->strFilePath;
	RKString sTempFile = "";
	RKString sHeaderFile = "";
	char sTmpPath[256];

	memset(sTmpPath,0,sizeof(char)*256);
	RKString sExtension;
	RKString_ExtractFileBase(strTargetFileName.c_str(), sTmpPath);
	sTempFile = sTmpPath;
	memset(sTmpPath,0,sizeof(char)*256);
	RKString_ExtractFileExtension(strTargetFileName.c_str(), sTmpPath);
	sExtension = sTmpPath;
	sTempFile.Append("_");
	sTempFile.Append(sExtension);
	sHeaderFile = sTempFile;
	sTempFile.Append(".tmp");
	sHeaderFile.Append(".hdr");

	//Remove the temp files
	if (RKFile_Exists(sTempFile.GetString()))
		RKFile_Remove(sTempFile.GetString());
	if (RKFile_Exists(sHeaderFile.GetString()))
		RKFile_Remove(sHeaderFile.GetString());
}


bool WebRequestDownloader::IsValidPNG(const char* a_strFileName)
{
	//Check it
	RKFile* pFile = RKFile_Open(a_strFileName, RKFM_READ);
	if (!pFile)
		return false;

	//Check Size is large enough for us to check tags
	uint32 uiSize = RKFile_GetSize(pFile);
	if (uiSize < 24)
	{
		RKFile_Close(&pFile);
		return false;
	}

	//Read in file
	char* pData = new char[uiSize];
	RKFile_Read(pFile, pData, uiSize);
	RKFile_Close(&pFile);

	//Check front tag
	if (memcmp(pData, _PNG_StartTag, 8) != 0)
	{
		delete[] pData;
		return false;
	}

	//Check for end tag
	bool bEndTagFound = false;
	char* pEndCheck = &pData[uiSize - 5];
	for (unsigned int i = 0; i < uiSize - 4; i++)
	{
		if (memcmp(pEndCheck, _PNG_EndTag, 4) == 0)
		{
			bEndTagFound = true;
			break;
		}
		pEndCheck--;
	}

	delete[] pData;
	return bEndTagFound;
}