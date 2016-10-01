#ifndef _SWF_MANAGER_H
#define _SWF_MANAGER_H

#include "CasualCoreConfig.h"
#if CC_USE_GAMESWF==1

#include <vector>
#include <gameswf/gameswf.h>
#include "RKHashTable.h"

//now using CC_USE_GAMESWF set to 0 to 1 ...  #ifndef USE_GAMESWF
//#define USE_GAMESWF // change this to disable GameSWF functionality
//#endif // USE_GAMESWF

namespace gameswf
{
  class FlashFX;
  class CharacterHandle;
  struct ASNativeEventState;
  struct PlayerContext;
  struct InitializationParameters;
  struct Size;
  struct face_entity;
}

struct RKCriticalSection;
struct RKThread;

#define MAX_SWFEVENTCALLBACKS 64

namespace CasualCore
{
  class SWFHostInterface;
  struct SWFEventCallback
  {
    RKString flashfile;
    RKString control;
    RKString eventname;
    RKString tag;
  };

  // callback for background loading
  typedef void (*FlashFXLoadedCallback) (gameswf::FlashFX* pFlashFX, void* pCallbackData);

  struct BackgroundLoadFX
  {
    std::string Filename;
    gameswf::FlashFX*  FlashFX;             // the flashfx file to load
    RKThread* LoadingThread;                // the loading thread for the FlashFX
    bool Loaded;                            // was the flashFX loaded
    FlashFXLoadedCallback Callback;         // the callback to call when done
    void* CallbackData;                     // a data paramater to pass to the callback
    gameswf::PlayerContext* LoadingContext; // the flash context to load under
  };

  typedef std::vector<BackgroundLoadFX*> BackgroundLoadFXList;


  struct FlashFXScene
  {
    gameswf::FlashFX*  FlashFX;
    unsigned int    DrawOrder;
    bool        InjectInput;
    bool        WasDeleted;
  };

  typedef std::vector<FlashFXScene> FlashFXList;

  /**
  * This class is manages updating and rendering of FlashFX scenes.
  *
  * \author owen.wigg@gameloft.com
  */
  class SWFManager
  {
  public:
    SWFManager();
    virtual ~SWFManager();

    /**
    * Initialize instance
    * Must provide an implementation of SWFHostInerface. Games should derive from this to provide custom file path handling.
    * remark : Glyph texture cache default sizes are big and may be unused - ex: LPS is ok with 512,8,0,0
    */
    void      Initialize(SWFHostInterface* pHostInterface, const gameswf::Size& glyphTTFTxWH, int glyphBMPTxWH=1024, int glyphLdCtxTTFTxWH=1024, int glyphLdCtxBMPTxWH=1024);

    /**
    * Shutdown instance
    * Shuts down GameSWF and cleans up any FlashFX files not removed by their owners.
    */
    void      Shutdown();

    /**
    * BackgroundLoadFlashFX
    * Load a FlashFX file in a Background thread. The callback will be called when the loading is done.    
    */
    void      BackgroundLoadFlashFX(const std::string& filename , gameswf::FlashFX* pFlashFX, FlashFXLoadedCallback pCallback, void* pCallbackData);

    /**
    * AddFlashFX
    * Add a FlashFX player to be maintained - can also specify a draw order, lower will be drawn first.
    * User must call ->load(filename) on the player themselves
    */
    void      AddFlashFX(gameswf::FlashFX* pFlashFX, unsigned int iDrawOrder=0, bool bInjectInput=true);

    /**
    * AdjustDrawOrder
    * Changes the draw order of an existing FlashFX player.  Resorts the flash scene list.
    */
    void      AdjustDrawOrder(gameswf::FlashFX* _pSwf, int iDrawOrder);

    /**
    * RemoveFlashFX
    * Remove a FlashFX player from management. Does not clean up or unload the player.
    */
    void      RemoveFlashFX(gameswf::FlashFX* pFlashFX);

    /**
    * GetFlashFXCount
    * 
    */
    int				GetFlashFXCount() const;

    /**
    * GetFlashFX
    * Grab a FlashFX player from management.
    */
    gameswf::FlashFX* GetFlashFX( const std::string& filename ) const;

    /**
    * GetFlashFX
    * Grab a FlashFX player from management.
    */
    gameswf::FlashFX* GetFlashFX( unsigned int idx ) const;

    /**
    * GetFlashInterface
    * Grab the SWFHostInterface from management.
    */
   SWFHostInterface* GetFlashInterface() const;

    /**
    * Update
    * Updates all FlashFX players currently being managed
    */
    void			Update(float fDT);

    void UpdateLoadingScenes( float fDT );

    /**
    * Render
    * Renders all FlashFX players currently being managed
    */
    void      Render();

    void LockUserInputLeaks(bool lock) { m_LockUserInputLeaks = lock; }

    void DumpDynamicTextures();

    void ForceResetFocus(); 

    // Input Handling functions
    bool      OnTouchDown(float x, float y);
    bool      OnTouchUp(float x, float y);
    bool      OnTouchDrag(float x, float y);
    void      OnKeyPress(int keycode);

    // Coordinate Conversion
    static void RK2Swf(float &x, float &y);
    static void Swf2RK(float &x, float &y);

    inline bool      IsInitialized() { return m_bInitialised; }

    static void EventCallback(const gameswf::ASNativeEventState& eventState);
    void SubscribeToEvent(const char* flashfile, const char* controlname, const char* eventname, const char* tag);
    void UnsubscribeFromEvent(const char* tag);
    void UnsubscribeAllEvents();

    // fills vector with names of disabled movies
    // usually used together with EnableByNames (used to restore original state)
    void DisableAllEnabled(std::vector<std::string>& disabledNames);

    void EnableByNames(const std::vector<std::string>& names);

    gameswf::face_entity* GetFontFace(const char* fontname, bool bold, bool italic);
    bool RestrictTextToSingleLineBounds( gameswf::FlashFX& flash, gameswf::CharacterHandle textHandle, const char* text );

    /**
    * BackgroundLoadThread
    * thread function for Background loading SWF's
    */
    static uint32    BackgroundLoadThread(void* pThreadParam);

    /************************************************************************/
    /* This will make swf manager send input to the this scene                */
    /************************************************************************/
    void SetRestrictInputToScene(gameswf::FlashFX* a_pFx, bool a_bRestrict) 
    { 
        //TODO: this could potentially stomp each other scene input stealing if more than one scene uses this
        //FOr now we allow stealing but not unlocking
        if(a_bRestrict)
        {
          if( m_pRestrictInputToScene != NULL )
          {
            int current_priority = INT_MAX;
            int new_priority = INT_MAX;
            for( uint32 i = 0; i < m_aFlashScenes.size(); ++i )
            {
              const FlashFXScene& scene = m_aFlashScenes[i];
              if( scene.FlashFX == a_pFx )
              {
                new_priority = scene.DrawOrder;
              }
              if( scene.FlashFX == m_pRestrictInputToScene )
              {
                current_priority = scene.DrawOrder;
              }
            }
            if( new_priority < current_priority )
            {
              RKASSERT( false, "Another ui layer is already rendering at a higher layer in the flash enironment that has a lock on scene input, Performing the current input lock now will cause a soft lock" )
              return;
            }
          }
          m_pRestrictInputToScene = a_pFx;
        }
        else if(m_pRestrictInputToScene == a_pFx)
        {
          m_pRestrictInputToScene = NULL;
        }
    } 

    /****************************************************************************/
    /* This will remove a scene from the list of scenes to send touchs through  */
    /****************************************************************************/
    void SetOmitInputToScene( gameswf::FlashFX* a_pFx, bool a_bOmmit )
    {
      if( a_bOmmit )
      {
        m_pOmittedInputToScene.Append( a_pFx );
      }
      else
      {
        m_pOmittedInputToScene.EraseAll( a_pFx );
      }
    }

    ///
    /// Call through to the SWFHostInterface to invoke the language change on all 
    /// Registered localisation objects.
    ///
    void SetLanguage( int language );
  public:
    void onLanguageChange( int language );
	// Change Size SWF
	void OnSWFSizeChange(int width, int height);
  private:
  /// 
  /// Functions which deal with scaling flash and moving Anchornodes.
  /// 
	void ScaleToAspectRatio(gameswf::FlashFX* _pSwf, float& xScale, float& yScale, float preWidth = 0, float preHeight = 0);
	void ScaleAnchorNodes(gameswf::FlashFX* _pswf, float preWidth = 0, float preHeight = 0);
	void ScaleAnchorNode(gameswf::CharacterHandle _pObject, float preWidth = 0, float preHeight = 0);
    void ScaleObject( gameswf::CharacterHandle _pObject, float xScale, float yScale, float preWidth = 0 , float preHeight = 0 );
	void RecurseProcessAnchorNodes(gameswf::array<gameswf::CharacterHandle>& pChildren, float preWidth = 0, float preHeight = 0);
    void MoveToAnchor(gameswf::CharacterHandle _pObject);
	void RestoreScaleObject(gameswf::CharacterHandle _pObject, float xScale, float yScale);

    FlashFXList      m_aFlashScenes;    // vector list of all FlashFX players under management

    SWFHostInterface*  m_pHostInterface;  // pointer to the HostInterface implementation that manages engine specific file handling and logging for gameswf

    bool        m_bInitialised;
    bool m_LockUserInputLeaks;

    uint32                              m_inputRestrictionPriority;
    gameswf::FlashFX*                   m_pRestrictInputToScene;
    RKList<gameswf::FlashFX*>           m_pOmittedInputToScene;

    // Background Loading
    static RKCriticalSection*           m_pLoadingMutex;   // shared mutex that controls access to the BackgroundLoadThread function, see that function for details
    static RKCriticalSection*           m_pLoadedCheckMutex;   // shared mutex that controls access to writing load completed statuses
    gameswf::PlayerContext*             m_pLoadingContext; // the loading context used by renderfx::load calls

    gameswf::InitializationParameters*  m_pLoadingInitParameters; 

    BackgroundLoadFXList                m_aLoadingFlashScenes;


    RKHashTable<SWFEventCallback*>                       m_eventcallbacks;

  };
}

#endif //#if CC_USE_GAMESWF==1

#endif //!_SWF_MANAGER_H
