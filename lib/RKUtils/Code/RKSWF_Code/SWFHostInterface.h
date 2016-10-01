#ifndef SWFHOSTINTERFACE_H
#define SWFHOSTINTERFACE_H




#include <map>
#include <vector>
#include <string>
#include <gameswf/gameswf.h>
#include <gameswf/core/mutex.h>
namespace RKUtils
{
  struct RKSemaphore;
  struct RKCriticalSection;

  class TextureAtlas;

  class SWFHostInterface : public gameswf::HostInterface
  {
  public:
    SWFHostInterface() : m_bLanguageIsAuto(false) {}
    virtual ~SWFHostInterface();

    virtual bool getFont(const gameswf::FontDescriptor& desc, gameswf::FontInfos& infos);
    virtual gameswf::VideoTexturePtr getTexture(const char* exportName, gameswf::Size& displaySize);
    virtual gameswf::String getFile(const char* fileName, const char* mode);
    virtual void logMessage(bool error, const char* text);
    virtual void assertMessage(const char* message, const char* file, int line);
    virtual void onLanguageChange( int language ) = 0;

    bool LoadAtlas(const char* fpath);
    void ReleaseAllTextures();		

    void SetLanguageIsAuto(bool bIsAuto){ m_bLanguageIsAuto = bIsAuto; }
    virtual bool languageIsAuto(){ return m_bLanguageIsAuto; }
    //@note [asif.memon 06.11.2014] Brought over changes from PP to fix bug 6739179. This is a hack fix but unfortunately it is the only one available at the moment.
    //                              GameSWF needs to be refactored to know the game language in order to avoid hacks like these

    virtual bool languageIsJapanese();

  private:
    gameswf::VideoTexturePtr _TryLoadTexture(const char* path);

    std::map<std::string, gameswf::VideoTexturePtr> m_mapTextures;
    std::vector<TextureAtlas*> m_textureAtlases;
    bool m_bLanguageIsAuto;
  };

  // this class provides an interface between GameSWF's condition and RK's condition
  class SWFCondition
  {
  public:
    SWFCondition(gameswf::Mutex& rMutex);
    virtual ~SWFCondition();

    void wait();
    void signal();
  protected:
    RKSemaphore*		m_pSemaphore;
    gameswf::Mutex&		m_rMutex;
    RKCriticalSection*	m_pWaitCounterLock;
    int					m_iWaitCount;
  };

}


#endif  //SWFHOSTINTERFACE_H
