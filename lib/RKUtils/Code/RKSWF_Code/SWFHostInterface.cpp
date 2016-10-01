#include "CasualCoreConfig.h"


#include <CasualCore.h>
#include <gameswf/gameswf.h>
#include <gameswf/core/file.h>
#include <gameswf/core/mutex.h>
#include <RKThread.h>
#include <RKFile.h>
#include <RKHeap.h>
#include <RKPNG.h>
#include <RKPVRT.h>
#include <gameswf/core/types.h>
#include "SWFHostInterface.h"
#include "RKTexture.h"
#include "Graphics/TextureAtlas.h"

namespace RKUtils
{

  //------------------------------
  // gameswf Host Interface
  //------------------------------

  class VideoTextureImpl : public gameswf::VideoTexture
  {
  public:
    VideoTextureImpl(RKTexture* texture, const RKVector& pos, const RKVector& size)
      : m_texture((RKTexture*)texture->GetRef())
      , m_width((int)size.x)
      , m_height((int)size.y)
    {
      m_pos.x = float(pos.x) / m_texture->width;
      m_pos.y = float(pos.y) / m_texture->height;
      m_size.x = float(size.x) / m_texture->width;
      m_size.y = float(size.y) / m_texture->height;
    }
    ~VideoTextureImpl()
    {
      RKTexture_Destroy(&m_texture);
    }

    virtual int width()  const { return m_width; }
    virtual int height() const { return m_height; }

    virtual RKTextureHandle getTextureId()      const { return m_texture->handle; }
    virtual RKTextureHandle getSplitTextureId() const { return m_texture->pSplitAlpha->handle; }

    virtual bool hasSplitAlpha()       const { return m_texture->pSplitAlpha != NULL; }
    virtual bool splitAlphaHasAlpha()  const { return m_texture->pSplitAlpha->pixelFormat != RKPF_RGB_ETC1; }

    virtual void* GetRawTexture() { return m_texture; }
    virtual void fixupUV(gameswf::vector2df& uv) {
      uv.x = m_pos.x + uv.x * m_size.x;
      uv.y = m_pos.y + uv.y * m_size.y;
    }

  private:
    RKTexture* m_texture;
    int m_width;
    int m_height;
    gameswf::vector2df m_pos;
    gameswf::vector2df m_size;
  };

#pragma region "SWFHostInterface"

  SWFHostInterface::~SWFHostInterface()
  {
    ReleaseAllTextures();
    for (unsigned int i = 0; i < m_textureAtlases.size(); ++i)
    {
      RKDELETE(m_textureAtlases[i]);
    }
    m_textureAtlases.clear();
  }

  bool SWFHostInterface::LoadAtlas(const char* fpath)
  {
    TextureAtlas* atlas = RKNEW(TextureAtlas);
    if (atlas->Load(fpath))
    {
      m_textureAtlases.push_back(atlas);
      return true;
    }
    else
    {
      RKDELETE(atlas);
      return false;
    }
  }

  void SWFHostInterface::ReleaseAllTextures()
  {
    std::map<std::string, gameswf::VideoTexturePtr>::iterator it; 
    for (it = m_mapTextures.begin(); it != m_mapTextures.end(); ++it)
    {
      if (it->second)
      {
        RKASSERT(it->second->refCount() == 1, "Someone holds the texture");
        it->second->releaseRef();
      }
    }
    m_mapTextures.clear();
  }

  // interface for finding FONTS in the game filesystem, Games should derive from this class and override this
  bool SWFHostInterface::getFont(const gameswf::FontDescriptor& desc, gameswf::FontInfos& infos)
  {
    const char* style = "";
    if(desc.m_bold && desc.m_italic)
    style = "BI";
    else if(desc.m_bold)
    style = "B";
    else if(desc.m_italic)
    style = "I";
    #ifdef GAME_IOS
    gameswf::String folder = "fonts";
    #else
    gameswf::String folder = "../data/fonts";//GetConfigValue("WorkingFolder", "./data");
    #endif
    char path[256];

#ifdef _XBOX
    sprintf(path, "game:\\%s\\%s%s.ttf", folder, desc.m_font.c_str(), style);
#else
    sprintf(path, "%s/%s%s.ttf", folder.c_str(), desc.m_font.c_str(), style);
    if (!RKFile_Exists(path))
    {
      sprintf(path, "fonts/celestia redux.ttf");
    }
#endif

    /*gameswf::File file(path, "rb");
    if(!file.isOpen())
    {
    #ifdef WIN32
    sprintf(path, "C:/Windows/Fonts/%s%s.ttf", desc.m_font.c_str(), style);
    gameswf::File winlib(path, "rb");
    if(!winlib.isOpen())
    {
    // fallback to arial
    sprintf(path, "%s/arial.ttf", folder.c_str());
    }
    #endif
    }
    */
    infos.m_fileName = path;
    //infos.m_loadInMemory = false;
    return true;
  }

  gameswf::VideoTexturePtr SWFHostInterface::_TryLoadTexture(const char* exportName)
  {
    RKString exportNameLower = exportName;
    exportNameLower.ToLowerCase();
  
    RKString baseName = exportNameLower;
    RKString_SetExtension(baseName, NULL);
    RKString pngPath = exportNameLower;
    RKString_SetExtension(pngPath, "png");
    RKString tgaPath = exportNameLower;
    RKString_SetExtension(tgaPath, "tga");
    RKString pvrPath = exportNameLower;
    RKString_SetExtension(pvrPath, "pvr");
    
    if (RKFile_Exists(pvrPath.GetString()) || RKFile_Exists(pngPath.GetString()) || RKFile_Exists(tgaPath.GetString()) || RKTexture_DoesExist(baseName.GetString()))
    {
      RKTexture* rkTexture = RKTexture_CreateByName(baseName.GetString(), false, false, false, RK_CLAMP);
      //RKASSERT(rkTexture, "Unable to load texture for injection: %s", exportName);
      VideoTextureImpl* pVidTex = RKNEW(VideoTextureImpl(rkTexture, RKVector(0.0f, 0.0f), RKVector((float)rkTexture->width, (float)rkTexture->height)));
      RKTexture_Destroy(&rkTexture);
      return pVidTex;
    }
    
    { // trying to fetch from atlases
      for (unsigned int i = 0; i < m_textureAtlases.size(); ++i)
      {
        TextureAtlasSprite* sprite = m_textureAtlases[i]->GetSprite(exportNameLower.GetString());
        if (sprite)
        {
          return RKNEW(VideoTextureImpl(sprite->texture, sprite->pos, sprite->size));
        }
      }
    }

    return NULL;
  }

  gameswf::VideoTexturePtr SWFHostInterface::getTexture(const char* exportName, gameswf::Size& displaySize)
  {
    // check map, return if already allocated.
//    std::map<std::string, gameswf::VideoTexturePtr>::iterator it = m_mapTextures.find(exportName);
//    if (it != m_mapTextures.end())
//      return it->second ? it->second->getRef() : NULL;

    RKString externalImagePath = RKString::MakeFormatted("swf_images/%s", exportName);
    gameswf::VideoTexturePtr texture = _TryLoadTexture(externalImagePath.GetString());

    if (!texture) // trying to load as regular texture
    {
      texture = _TryLoadTexture(exportName);
    }

//    m_mapTextures[exportName] = texture;
    return texture; // ? texture->getRef() : NULL;    << This is not necessary. We are passing ownership
  }

  // interface for finding assets in the game filesystem, Games should derive from this class and override this
  gameswf::String SWFHostInterface::getFile(const char* fileName, const char* mode)
  {
    char buffer[512] = {0};

    if(strstr(fileName, ".swf"))
      sprintf(buffer, "swf/%s", fileName);
    else if(strstr(fileName, ".ttf"))
      sprintf(buffer, "%s", fileName);
    else if(strstr(fileName, ".ogg") || strstr(fileName, ".flv"))
      sprintf(buffer, "video/%s", fileName);
    else if(strstr(fileName, ".glsl"))
      sprintf(buffer, "shaders/%s", fileName);

    //char fullFileNameAndPath[512];
    //RKFile* pFile = RKFile_Open(buffer, RKFM_READ);
    //if(pFile != 0)
    //{
    //  RKFile_CreatePath(fullFileNameAndPath, buffer);
    //  RKFile_Close(&pFile);
    //  return gameswf::String(fullFileNameAndPath);
    //}

    //if (RKFile_CreatePath(fullFileNameAndPath, buffer))
    {
      LOG(buffer);
      return gameswf::String(buffer);
    }

    return gameswf::String(buffer);
  }

  // interface for adding a log message
  void SWFHostInterface::logMessage(bool error, const char* text)
  {
		RKLogLevel logLevel = error ? RKLL_ERROR : RKLL_INFO;
		const int textLength = strlen(text);
    if (textLength > 0 && text[textLength-1] == '\n') // stripping trailing \n, which is not necessary for RKLog
    {
			_RKLOG(logLevel, "SWF", "%.*s", textLength - 1, text);
    }
    else
    {
			_RKLOG(logLevel, "SWF", "%s", text);
    }
  }

  // interface for adding an assert
  void SWFHostInterface::assertMessage(const char* message, const char* file, int line)
  {
		RKLOGt_FATAL("SWF", "Assert happened at %s:%d : %s", file, line, message);
		RKASSERT(false, "SWF assert");
  }

  //@note [asif.memon 06.11.2014] Brought over changes from PP to fix bug 6739179. This is a hack fix but unfortunately it is the only one available at the moment.
  //                              GameSWF needs to be refactored to know the game language in order to avoid hacks like these

  bool SWFHostInterface::languageIsJapanese()
  {
	  // [Update3] HaiNgo: Check if the current language is Japanese
	#if defined(OS_W8) || defined(OS_WP8)
	  return CGAME->GetLanguage() == LANGUAGE::JAPANESE;
	#endif
	  return false;
  }


  //------------------------------
  // gameswf Host Interface
  //------------------------------

#pragma endregion

#pragma region "SWFCondition"

  SWFCondition::SWFCondition(gameswf::Mutex& rMutex)
    : m_rMutex(rMutex)
    , m_pSemaphore(0)
    , m_pWaitCounterLock(0)
    , m_iWaitCount(0)
  {
    //m_pSemaphore = RKSemaphore_Create("swfcondition",0,100);
    //m_pWaitCounterLock = RKCriticalSection_Create("swfcond_waitlock");
  }

  SWFCondition::~SWFCondition()
  {
    //RKSemaphore_Destroy(&m_pSemaphore);
    //RKCriticalSection_Destroy(&m_pWaitCounterLock);
  }

  // precondition: thread must have locked m_rMutex
  void SWFCondition::wait()
  {
    int iLastWait;

    // increment the wait count
    //RKCriticalSection_Enter(m_pWaitCounterLock);
    m_iWaitCount++;
    //RKCriticalSection_Leave(m_pWaitCounterLock);

    // unlock the mutex
    m_rMutex.unlock();
    // wait on the semaphore
    //RKSemaphore_Wait(m_pSemaphore, true);

    // increment the wait count
    //RKCriticalSection_Enter(m_pWaitCounterLock);
    m_iWaitCount--;
    iLastWait = m_iWaitCount == 0;
    //RKCriticalSection_Leave(m_pWaitCounterLock);

    //m_rMutex.lock();

  }

  void SWFCondition::signal()
  {
    int haveWaiters;

    //RKCriticalSection_Enter(m_pWaitCounterLock);
    haveWaiters = m_iWaitCount > 0;
    //RKCriticalSection_Leave(m_pWaitCounterLock);

    //if (haveWaiters) {
    //	RKSemaphore_Signal(m_pSemaphore);
    //}
  }

#pragma endregion

} // end namespace CasualCore


//------------------------------
// gameswf Allocators
//------------------------------
#if RKHEAP_ELEPHANT_DEBUGHEAPSIZE != 0   //  1    //1 or 0  [Goldfish doesn't seems to show correct stack when using multiple heaps.. see also RKHeap.cpp to disable debug heap]
#define HEAP_PUSHDEBUG RKHeap_PushDebugHeap();
#define HEAP_POPDEBUG RKHeap_PopDebugHeap();
#else
#define HEAP_PUSHDEBUG ;
#define HEAP_POPDEBUG ;
#endif

void* SwfAlloc(size_t size, gameswf::MemHint swfMemHint)
{
  HEAP_PUSHDEBUG
    void* pMem = RKHeap_Alloc(size, "SWF Generic");
  HEAP_POPDEBUG
    return pMem;
}

void* SwfAlloc(size_t size, gameswf::MemHint hint, const char* filename, int line)
{
  HEAP_PUSHDEBUG
    static char buff[256];
    snprintf(buff, 256, "%s:%d", filename, line);
    void* pMem = RKHeap_Alloc(size, buff);
  HEAP_POPDEBUG
    return pMem;
}

void SwfFree(void* p)
{	
  HEAP_PUSHDEBUG
    if (p) 
      RKHeap_Free(p, "SWF");
  HEAP_POPDEBUG
}

//------------------------------
// gameswf Threading
//------------------------------
#if GAMESWF_ENABLE_MULTITHREAD
namespace gameswf
{
  // THREAD

  Thread::Thread(FunctionPtr fn, void* data)  :
m_impl(0)
{
  //LOG("SWF::Thread::Constructor()");
  RKThread* pThread = RKThread_Create("swfthread",(RKThreadCallback*)fn,data);
  m_impl = pThread;		
  RKThread_Start(pThread);		
}

Thread::~Thread()
{
  //LOG("SWF::Thread::Destructor()");
  m_impl = 0;
}

void Thread::wait()
{
  //LOG("SWF::Thread::Wait()");
  //RKThread_Yield();
  RKThread_WaitForExit((RKThread*)m_impl);		
}

void Thread::kill()
{
  //LOG("SWF::Thread::Kill()");
  if (m_impl != 0)
  {
    //RKThread* pThread = (RKThread*)m_impl;
    //RKThread_Destroy(&pThread);
    RKThread_Destroy((RKThread**)&m_impl);
  }
  m_impl = 0;
}

// MUTEX

Mutex::Mutex() :
m_impl(0)
{
  //LOG("SWF::Mutex::Constructor()");
  //m_impl = RKInterlock_Create("swfmutex");
  m_impl = RKCriticalSection_Create("swfmutex");
}

Mutex::~Mutex()
{
  //LOG("SWF::Mutex::Destructor()");
  if (m_impl != 0)
  {
    //RKInterlock* pInterlock = (RKInterlock*)m_impl;
    //RKInterlock_Destroy(&pInterlock);
    //RKCriticalSection* pMutex = (RKCriticalSection*)m_impl;
    RKCriticalSection_Destroy((RKCriticalSection**)&m_impl);
  }
  m_impl = 0;
}

void Mutex::lock()
{
  //LOG("SWF::Mutex::Lock()");
  if (m_impl != 0)
    RKCriticalSection_Enter((RKCriticalSection*)m_impl);
  //RKInterlock_Lock((RKInterlock*)m_impl);
}

void Mutex::unlock()
{
  //LOG("SWF::Mutex::Unlock()");
  if (m_impl != 0)
    RKCriticalSection_Leave((RKCriticalSection*)m_impl);
  //RKInterlock_Unlock((RKInterlock*)m_impl);
}

// CONDITION

Condition::Condition() :
m_impl(0)
{
  //LOG("SWF::Condition::Constructor()");
  //m_impl = RKSemaphore_Create("swfcondition",1,10);
  m_impl = RKNEW(CasualCore::SWFCondition(m_cond_mutex));
}

Condition::~Condition()
{
  //LOG("SWF::Condition::Desctructor()");
  if (m_impl != 0)
  {
    //RKSemaphore* pSemaphore = (RKSemaphore*)m_impl;
    //RKSemaphore_Destroy((RKSemaphore**)&m_impl);
    CasualCore::SWFCondition* pCondition = (CasualCore::SWFCondition*)m_impl;
    RKDELETE(pCondition);
  }
  m_impl = 0;
}

void Condition::wait()
{
  //LOG("SWF::Condition::Wait()");
  //m_cond_mutex.unlock();
  //RKSemaphore_Wait((RKSemaphore*)m_impl,true);
  ((CasualCore::SWFCondition*)m_impl)->wait();
}

void Condition::signal()
{
  //LOG("SWF::Condition::Signal()");
  //RKSemaphore_Signal((RKSemaphore*)m_impl);
  ((CasualCore::SWFCondition*)m_impl)->signal();
}

}

#endif
//*****************************************************************
//*  Custom allocators for FreeType lib
//*  set from SWFManager, with FreeTypeLib_SetAllocators(..)
//*  [need updated lib - currently only for Win32], 
//*  rem: default allocators were std malloc, free, etc
//*****************************************************************

extern "C" void *FT_RK_malloc(size_t size);
extern "C" void FT_RK_sfree(void *p);
extern "C" void *FT_RK_calloc(unsigned int cnt, size_t size);
extern "C" void *FT_RK_realloc(void *p, size_t newSize);

//---------------------------------------------------------------------------------------------------------------------------------------
void *FT_RK_calloc(unsigned int cnt, size_t size0)
  //---------------------------------------------------------------------------------------------------------------------------------------
{
  size_t size = size0 * cnt;
  char *p = (char*)SwfAlloc(size, gameswf::MEMHINT_NONE,  __FILE__, __LINE__);
  memset(p, 0, size);
  return p;
}

//---------------------------------------------------------------------------------------------------------------------------------------
void FT_RK_sfree(void *p)
  //---------------------------------------------------------------------------------------------------------------------------------------
{
  SwfFree(p);
}

//---------------------------------------------------------------------------------------------------------------------------------------
void *FT_RK_malloc(size_t size)
  //---------------------------------------------------------------------------------------------------------------------------------------
{
  return SwfAlloc(size, gameswf::MEMHINT_NONE,  __FILE__, __LINE__);
}

//---------------------------------------------------------------------------------------------------------------------------------------
void *FT_RK_realloc(void *p, size_t newSize)
  //---------------------------------------------------------------------------------------------------------------------------------------
{
  return RKHeap_Realloc(p, newSize);
}


