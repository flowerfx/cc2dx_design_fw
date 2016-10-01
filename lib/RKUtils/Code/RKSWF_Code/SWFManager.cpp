#include "CasualCoreConfig.h"
#if CC_USE_GAMESWF==1

#include <CasualCore.h>
#include <gameswf/gameswf.h>
#include <gameswf/ui/flashfx.h>
#include <RKThread.h>
#include <RKDevice.h>
#include <RKProfileManager.h>
#include <RKApplication.h>
#include "SWFManager.h"
#include "SWFHostInterface.h"
#include "RKDevice.h"
#include <gameswf/swf/text.h>
#include <gameswf/swf/environment.h>
#include <gameswf/swf/freetype.h>


namespace CasualCore
{
  RKCriticalSection* SWFManager::m_pLoadingMutex = 0;
  RKCriticalSection* SWFManager::m_pLoadedCheckMutex = 0;

  // define sort delegate to sort layers by draw order
  bool SWFLayerSortDelegate (const FlashFXScene& i, const FlashFXScene& j)
  { 
    //RKASSERT( i.DrawOrder != j.DrawOrder, "Trying to set a Flash file to render on the same layer as another flash file." );
    return (i.DrawOrder < j.DrawOrder);
  }

  SWFManager::SWFManager()
    :m_pHostInterface(0) 
    ,m_bInitialised(0)
    ,m_LockUserInputLeaks(false)
    ,m_pRestrictInputToScene(NULL)
  {
    // constructor, do nothing
  }

  SWFManager::~SWFManager()
  {
    UnsubscribeAllEvents();
    m_eventcallbacks.Deinit();

    if ( m_pHostInterface )
      m_pHostInterface->ReleaseAllTextures();

    // Shutting down.
    //Shutdown();


    m_eventcallbacks.Clear();
    m_eventcallbacks.Deinit();
    // remind us to shut down the manager
    assert(m_pHostInterface == 0);
  }

  void SWFManager::Initialize(SWFHostInterface* pHostInterface, const gameswf::Size& glyphTTFTxWH, int glyphBMPTxWH, int glyphLdCtxTTFTxWH, int glyphLdCtxBMPTxWH)
  {
    assert(pHostInterface);
    // test the function does return some output
    //		assert(pHostInterface->getFile("test.swf","rb").c_str() != 0);
    assert(m_bInitialised == 0);
    
    int fontscale = RKDevice_GetWidth() >= 2048 || RKDevice_GetHeight() >=2048 ? 2 : 1;
    gameswf::Size glyphTTFTSize;
    glyphTTFTSize.m_width = RKMIN(glyphTTFTxWH.m_width * fontscale, 2048);
    glyphTTFTSize.m_height = RKMIN(glyphTTFTxWH.m_height * fontscale, 2048);
    glyphLdCtxTTFTxWH = RKMIN(glyphLdCtxTTFTxWH * fontscale, 2048);

    m_pHostInterface = pHostInterface;

    m_eventcallbacks.Init(MAX_SWFEVENTCALLBACKS);

    // initialise GameSWF

#ifdef GAME_WIN32		//currently only updated W32 version of Freetype lib ... [rem: now called from CCPlatform_SetAllocators() ]
    //MOVED to CCPlatform_SetAllocators() in Platform.cpp --	FreeTypeLib_SetAllocators(FT_RK_malloc, FT_RK_sfree, FT_RK_calloc, FT_RK_realloc);
#endif
    // This inialization should be done only once, before any flash player is created
    gameswf::InitializationParameters params;
    params.m_glyphTextureCacheSize = glyphTTFTSize;
    params.m_bitmapGlyphTextureCacheSize = gameswf::Size(glyphBMPTxWH,glyphBMPTxWH);    //this is for a rgba bitmap - 4 bytes per pixel - (unused in PLS, set a dummy 8x8 texture ...)
    params.m_fontScale = fontscale;
    params.m_hostInterface = m_pHostInterface;
    gameswf::initialize(params);

    // set up background loading variables
    if (glyphLdCtxTTFTxWH != 0 || glyphLdCtxBMPTxWH !=0)  //[background loading context, not used in LPS]
    {
      m_pLoadingInitParameters = RKNEW(gameswf::InitializationParameters);
      m_pLoadingInitParameters->m_glyphTextureCacheSize = gameswf::Size(glyphLdCtxTTFTxWH, glyphLdCtxTTFTxWH);  //1024, 1024);
      m_pLoadingInitParameters->m_bitmapGlyphTextureCacheSize = gameswf::Size(glyphLdCtxBMPTxWH, glyphLdCtxBMPTxWH);  //1024, 1024);
      m_pLoadingInitParameters->m_fontScale =fontscale;
      m_pLoadingContext = gameswf::createContext(*m_pLoadingInitParameters);
    }
    else
    {
      m_pLoadingInitParameters = NULL;
      m_pLoadingContext = NULL;
    }

    m_pLoadingMutex = RKCriticalSection_Create("SWFManager_LoadingMutex");
    m_pLoadedCheckMutex = RKCriticalSection_Create("SWFManager_LoadedCheckMutex");

    m_bInitialised = true;
  }

  void SWFManager::Shutdown()
  {
    if (!m_bInitialised)
      return;

    if (m_pLoadingMutex)
    {
      // wait for the mutex, in case any flashFX was loading while we try to shut down
      RKCriticalSection_Enter(m_pLoadingMutex);

      // dont need to actually do anything in here
      RKCriticalSection_Leave(m_pLoadingMutex);

      RKCriticalSection_Destroy(&m_pLoadingMutex);
    }

    if (m_pLoadedCheckMutex)
    {
      // wait for the mutex, in case any flashFX was loading while we try to shut down
      RKCriticalSection_Enter(m_pLoadedCheckMutex);

      // dont need to actually do anything in here
      RKCriticalSection_Leave(m_pLoadedCheckMutex);

      RKCriticalSection_Destroy(&m_pLoadedCheckMutex);
    }

    if (m_pLoadingContext)
    {
      gameswf::destroyContext(m_pLoadingContext);
      m_pLoadingContext = 0;
      RKDELETE(m_pLoadingInitParameters);
    }

    // loop through and shut down all FlashFX objects
    unsigned int iSize = m_aFlashScenes.size(); 
    for (int i=iSize-1; i >= 0; --i)
    {
      if (m_aFlashScenes[i].FlashFX)
      {
        m_aFlashScenes[i].FlashFX->unload();
        RKDELETE(m_aFlashScenes[i].FlashFX);
      }

      m_aFlashScenes.pop_back();
    }

    // shut down GameSWF
    //ok now..without Elephant.. if (RKApplication_IsExiting()==false)   //TEMP? to check - now gameswf::finalize() crashing : avoid cleaning memory for now ...
    gameswf::finalize();

    // flag the manager as shutdown
    m_bInitialised = 0;
    m_pHostInterface = 0;
  }

  /**
  * BackgroundLoadFlashFX
  * Load a FlashFX file in a Background thread. The callback will be called when the loading is done.    
  */
  void SWFManager::BackgroundLoadFlashFX(const std::string& filename , gameswf::FlashFX* pFlashFX, FlashFXLoadedCallback pCallback, void* pCallbackData)
  {
    // we create a thread for each flash scene we want to background load.
    // This way there are no background threads running when a Flash is not being loaded
    // or if Background loading is never used.

    // create a flash scene struct
    BackgroundLoadFX* pScene = RKNEW(BackgroundLoadFX);
    pScene->Filename = filename;
    pScene->LoadingThread = RKThread_Create("FlashLoadThread", &BackgroundLoadThread, pScene);
    pScene->Callback = pCallback;
    pScene->CallbackData = pCallbackData;
    pScene->FlashFX = pFlashFX;
    pScene->LoadingContext = m_pLoadingContext;
    pScene->Loaded = false;

    // add to the list
    m_aLoadingFlashScenes.push_back(pScene);

    // start the thread
    RKThread_Start(pScene->LoadingThread);
    //  RKThread_WaitForExit(pScene->LoadingThread);
  }

  /**
  * BackgroundLoadThread
  * thread function for Background loading SWF's
  */
  uint32 SWFManager::BackgroundLoadThread(void* pThreadData)
  {
    BackgroundLoadFX* pScene = (BackgroundLoadFX*)pThreadData;

    // lock this section of code so that only one loading thread may run at a time
    RKCriticalSection_Enter(m_pLoadingMutex);

    // load the flash scene under a different context so it doesnt conflict
    // with scenes already running under the default context
    pScene->FlashFX->load( pScene->Filename.c_str() , pScene->LoadingContext );    

    // unlock the section
    RKCriticalSection_Leave(m_pLoadingMutex);

    RKCriticalSection_Enter(m_pLoadedCheckMutex);
    pScene->Loaded = true;
    RKCriticalSection_Leave(m_pLoadedCheckMutex);

    return 0;
  }


  void SWFManager::AddFlashFX(gameswf::FlashFX* pFlashFX, unsigned int iDrawOrder, bool bInjectInput)
  {
    // OJW - check that the FlashFX is not still Background Loading
    BackgroundLoadFXList::iterator it;
    for (it = m_aLoadingFlashScenes.begin(); it != m_aLoadingFlashScenes.end(); it++)
    {
      if (pFlashFX == (*it)->FlashFX)
      {
        // warning you not to Add a FlashFX to the manager until its loading is completed
        assert(0);
        return;
      }
    }

    pFlashFX->forceTexturesToVRAM(false);

    // create a flash scene struct
    FlashFXScene aScene;
    aScene.DrawOrder = iDrawOrder;
    aScene.FlashFX = pFlashFX;
    aScene.InjectInput = bInjectInput;

    // add to the list
    m_aFlashScenes.push_back(aScene);

    // re-sort the list by draw order
    std::sort( m_aFlashScenes.begin() , m_aFlashScenes.end() , SWFLayerSortDelegate );

    // GameSWF doesn't initiate the ActionScript classes that underlay a component until
    // the first update has been hit.  Forcing the update to occur here so that we can immediately
    // start calling the ActionScript class functions. (JW)
    pFlashFX->update( 0.0f );

	/// No point to call updates here, we don't want to destroy pending to die flashes
	//  and/or progress load background loading scenes.
//    UpdateLoadingScenes( 0.0f );

    ScaleAnchorNodes( pFlashFX );
  }

  void SWFManager::AdjustDrawOrder(gameswf::FlashFX* _pSwf, int iDrawOrder)
  {
    // find this FlashFX if it exists
    FlashFXList::iterator it;
    for (it = m_aFlashScenes.begin(); it != m_aFlashScenes.end(); it++)
    {
      if ((*it).FlashFX == _pSwf)
      {
        (*it).DrawOrder = iDrawOrder;
    
        std::sort( m_aFlashScenes.begin() , m_aFlashScenes.end() , SWFLayerSortDelegate );
      }
    }
  }


  void SWFManager::onLanguageChange( int language )
  {
    for (FlashFXList::iterator it = m_aFlashScenes.begin(); it != m_aFlashScenes.end(); it++)
    {
      gameswf::clearFonts( it->FlashFX->getPlayerContext() );
    }

    if(m_pHostInterface)
    {
      m_pHostInterface->onLanguageChange(language);
    }
  };

  void SWFManager::ScaleToAspectRatio(gameswf::FlashFX* _pSwf, float& xScale, float& yScale, float preWidth, float preHeight)
  {
    float fScreenHeight = float( RKDevice_GetHeight() );

    int iStageHeight = _pSwf->getViewportHeight();

// W8 added ??
	if (preWidth != 0 && preHeight != 0)
	{
		iStageHeight = 768;
	}
// end
    xScale = fScreenHeight/float(iStageHeight);
    yScale = float(iStageHeight)/fScreenHeight;

    _pSwf->SetScale(yScale);

    // define the inverse scale in flash
    gameswf::ASValue as_scale = float(iStageHeight)/float(fScreenHeight);
    _pSwf->getRootHandle().invokeMethod("SetFlashScale", &as_scale, 1);
  }

  void SWFManager::ScaleAnchorNodes(gameswf::FlashFX* _pswf, float preWidth, float preHeight)
  {
    // Get x and y scales.
    float xscale, yscale;
    ScaleToAspectRatio(_pswf, xscale, yscale, preWidth, preHeight);

    //get a list of all the nodes in the root of flash file
    gameswf::array<gameswf::CharacterHandle> pChildren;
    _pswf->getRootHandle().getChildren(pChildren);

    //get the total size of the array
    int iSize = pChildren.size();
    //iterate through all the nodes in scene root
    for(int i = 0; i < iSize; ++i)
    {
      gameswf::CharacterHandle pCurrentChild = pChildren[i];

      ScaleObject( pCurrentChild, xscale, yscale, preWidth, preHeight );
    }

    RecurseProcessAnchorNodes( pChildren,preWidth,preHeight );
  }

  void SWFManager::RecurseProcessAnchorNodes(gameswf::array<gameswf::CharacterHandle>& pChildren, float preWidth, float preHeight)
  {
    //get the total size of the array
    int iSize = pChildren.size();
    //iterate through all the nodes in scene root
    for(int i = 0; i < iSize; ++i)
    {
      gameswf::CharacterHandle pCurrentChild = pChildren[i];

      /// Perform scale and move.
      ScaleAnchorNode( pCurrentChild ,preWidth, preHeight);
      MoveToAnchor( pCurrentChild );

      gameswf::array<gameswf::CharacterHandle> pGrandChildren;
      pCurrentChild.getChildren(pGrandChildren);
      if( pGrandChildren.size() != 0 )
      {
        RecurseProcessAnchorNodes( pGrandChildren, preWidth,preHeight );
      }
    }
  }

  void 
  SWFManager::ScaleAnchorNode( gameswf::CharacterHandle _pObject, float preWidth, float preHeight )
  {
    float fScreenHeight = float( RKDevice_GetHeight() );
    float fScreenWidth  = float( RKDevice_GetWidth() );
    float AspectRatio = fScreenWidth / fScreenHeight;

    if( _pObject.isValid() )
    {
      RKString sObjectType;
      double scale_16_9 = 1.0;
      double scale_4_3  = 1.0;
      sObjectType = _pObject.getLocalVariable("className").toString();
      if (_pObject.isValid() && sObjectType.Compare("AnchorNode") == 0)
      {
        //get the anchors type e.g. Center/Left etc. these are set from a dropdown box within flash
        if( !_pObject.getLocalVariable("Scale_16_9").isUndefined() )
        {
          scale_16_9 = _pObject.getLocalVariable("Scale_16_9").toNumber();
        }
        if( !_pObject.getLocalVariable("Scale_4_3").isUndefined() )
        {
          scale_4_3 = _pObject.getLocalVariable("Scale_4_3").toNumber();
        }

        /// Rise over run to make a gradient
        if( scale_16_9 != scale_4_3 )
        {
          static const double ratio_4_3  = 4.0 / 3.0;
          static const double ratio_16_9 = 16.0 / 9.0;
          double rise_value = scale_16_9 - scale_4_3;
          double run_value  = ratio_16_9 - ratio_4_3;
          double gradient   = ( rise_value ) / ( run_value );
          ///
          double up_scale  = ( gradient * ( AspectRatio - ratio_4_3 ) ) + scale_4_3;
		  if (preWidth != 0 && preHeight != 0)
		  {
			  double up_scale_temp = (gradient * ((preWidth / preHeight) - ratio_4_3)) + scale_4_3;
			  RestoreScaleObject(_pObject, up_scale_temp, up_scale_temp);
		  }

          //scale the object
          ScaleObject( _pObject, up_scale, up_scale );
        }
      }
    }
  }
  
  void SWFManager::MoveToAnchor(gameswf::CharacterHandle _pObject)
  {
    //get the screen height and width
    float fScreenWidth  = float( RKDevice_GetWidth() );
    float fScreenHeight = float( RKDevice_GetHeight() );

    //divide it by viewportscale (this is for iphone 3gs, which returns iphone 4 res with a viewport scale of 2)
    float fScale = PLATFORM->GetViewportScale();
    fScreenWidth /= fScale;
    fScreenHeight /= fScale;

    //instantiate c plusplus variables to store the AS Objects data
    //object type, anchor type, x and y offsets
    RKString sObjectType;
    RKString sAnchorType;

    //get the class name : should be "AnchorNode" or something went wrong
    sObjectType = _pObject.getLocalVariable("className").toString();

    if (_pObject == NULL || sObjectType.Compare("AnchorNode"))
    {
      return;
    }

    //get the anchors type e.g. Center/Left etc. these are set from a dropdown box within flash
    sAnchorType = _pObject.getLocalVariable("m_eAnchorType").toString();

    //get the current position of the object
    gameswf::Point anchor_position = _pObject.getPosition();

    //if the object is centre aligned
    if (sAnchorType.Compare("Center") == 0)
    {
      anchor_position.m_x = fScreenWidth * 0.5f;
      anchor_position.m_y = fScreenHeight * 0.5f;
    }
    //if the object is top aligned
    else if (sAnchorType.Compare("Top") == 0)
    {
      anchor_position.m_x = fScreenWidth * 0.5f;
      anchor_position.m_y = 0.0f;
    }
    //if the object is Bottom aligned
    else if (sAnchorType.Compare("Bottom") == 0)
    {
      anchor_position.m_x = fScreenWidth * 0.5f;
      anchor_position.m_y = fScreenHeight;
    }
    //if the object is Left aligned
    else if (sAnchorType.Compare("Left") == 0)
    {
      anchor_position.m_x = 0.0f;
      anchor_position.m_y = fScreenHeight * 0.5f;
    }
    //if the object is Right aligned
    else if (sAnchorType.Compare("Right") == 0)
    {
      anchor_position.m_x = fScreenWidth;
      anchor_position.m_y = fScreenHeight * 0.5f;
    }
    //if the object is TopRight aligned
    else if (sAnchorType.Compare("TopRight") == 0)
    {
      anchor_position.m_x = fScreenWidth;
      anchor_position.m_y = 0.0f;
    }
    //if the object is TopLeft aligned
    else if (sAnchorType.Compare("TopLeft") == 0)
    {
      anchor_position.m_x = 0.0f;
      anchor_position.m_y = 0.0f;
    }
    //if the object is BottomLeft aligned
    else if (sAnchorType.Compare("BottomLeft") == 0)
    {
      anchor_position.m_x = 0.0f;
      anchor_position.m_y = fScreenHeight;
    }
    //if the object is BottomRight aligned
    else if (sAnchorType.Compare("BottomRight") == 0)
    {
      anchor_position.m_x = fScreenWidth;
      anchor_position.m_y = fScreenHeight;
    }

    gameswf::Matrix parent_matrix;
    parent_matrix.setIdentity();

    gameswf::CharacterHandle parent = _pObject.getParent();
    if( parent.isValid() )
      parent_matrix = parent.getWorldMatrix();

    gameswf::Matrix anchor_matrix;
    anchor_matrix.setIdentity();
    anchor_matrix.concatenateTranslation( anchor_position.m_x, anchor_position.m_y );

    gameswf::Matrix inverse_parent_matrix;
    inverse_parent_matrix.setInverse( parent_matrix );
    inverse_parent_matrix.concatenate( anchor_matrix );

    gameswf::Point relative_offset = gameswf::Point( inverse_parent_matrix.getX(), inverse_parent_matrix.getY() );

    //set the object to the new position
    _pObject.setPosition(relative_offset);
    _pObject.setMember("_x", relative_offset.m_x);
    _pObject.setMember("_y", relative_offset.m_y);
  }

  void SWFManager::ScaleObject(gameswf::CharacterHandle _pObject, float a_fXScale, float a_fYScale, float preWidth, float preHeight)
  {
    //get the objects intial width and height
    float fWidth = _pObject.getMember("_width").toFloat();
    float fHeight = _pObject.getMember("_height").toFloat();
	
	if (preWidth != 0 && preHeight != 0)
	{
		fWidth = fWidth / (preHeight / 768);
		fHeight = fHeight / (preHeight / 768);
	}

    //apply the scale the objects width and height
    gameswf::ASValue width = gameswf::ASValue(a_fXScale * fWidth);
    gameswf::ASValue height = gameswf::ASValue(a_fXScale * fHeight);

    //set the scaled width and height to the object
    _pObject.setMember("_width", width);
    _pObject.setMember("_height", height);
  }

  void SWFManager::RestoreScaleObject(gameswf::CharacterHandle _pObject, float a_fXScale, float a_fYScale)
  {
	  //get the objects intial width and height
	  float fWidth = _pObject.getMember("_width").toFloat();
	  float fHeight = _pObject.getMember("_height").toFloat();

	  //apply the scale the objects width and height
	  gameswf::ASValue width = gameswf::ASValue(fWidth / a_fXScale);
	  gameswf::ASValue height = gameswf::ASValue(fHeight / a_fXScale);

	  //set the scaled width and height to the object
	  _pObject.setMember("_width", width);
	  _pObject.setMember("_height", height);
  }

  void SWFManager::OnSWFSizeChange(int width, int height)
  {
	  int preWidth = RKDevice_GetWidth();
	  int preHeight = RKDevice_GetHeight();
	  RKDevice_SetWidth(width);
	  RKDevice_SetHeight(height);
	  RKDevice_SetViewport(0, 0, width, height);
	  unsigned int iSize = m_aFlashScenes.size();
	  for (size_t i = 0; i < iSize; ++i)
	  {
		  if (gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX)
		  {
			  ScaleAnchorNodes(flashFX, preWidth,preHeight);
		  }
	  }
  }

  void SWFManager::RemoveFlashFX(gameswf::FlashFX* pFlashFX)
  {
    if (pFlashFX == m_pRestrictInputToScene)
    {
      m_pRestrictInputToScene = 0;
    }
    SetOmitInputToScene(pFlashFX, false);

    // find this FlashFX if it exists
    FlashFXList::iterator it;
    for (it = m_aFlashScenes.begin(); it != m_aFlashScenes.end(); it++)
    {
      FlashFXScene& aScene = (*it);
      if (aScene.FlashFX == pFlashFX)
      {
        // and remove it
        m_aFlashScenes.erase(it);
        return;
      }
    }
  }

  int	SWFManager::GetFlashFXCount() const
  {
    return m_aFlashScenes.size();
  }

  gameswf::FlashFX* SWFManager::GetFlashFX( unsigned int idx ) const
  {
    assert( idx < m_aFlashScenes.size() );
    return m_aFlashScenes[idx].FlashFX;
  }

  gameswf::FlashFX* SWFManager::GetFlashFX( const std::string& filename ) const
  {
    // find this FlashFX if it exists
    FlashFXList::const_iterator it;
    for (it = m_aFlashScenes.begin(); it != m_aFlashScenes.end(); it++)
    {
      if (strcmp(filename.c_str(), (*it).FlashFX->getFileName()) == 0)
      {
        return (*it).FlashFX;
      }
    }

    return NULL;
  }

  SWFHostInterface* SWFManager::GetFlashInterface() const
  {
    RKASSERT(m_pHostInterface,"SWFHostInterface not found");
    return m_pHostInterface;
  }

  void SWFManager::Update(float fDT)
  {
    RKPROFILE();

    if (!m_bInitialised)
      return;

    UpdateLoadingScenes(fDT);

    // loop through and update all flash FX scenes
    //unsigned int iSize = m_aFlashScenes.size(); 
    for( int i = 0; i < m_aFlashScenes.size(); )
    {
      if (gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX)
      {
        // ANLA: Delete the SWF if was scheduled for deletion
        if (flashFX->WasDeleted())
        {
          RemoveFlashFX(flashFX);
          flashFX->flushBufferedText();
          flashFX->unloadTextures();
          flashFX->unload();
          RKDELETE(flashFX);
        }
        else
        {
          int iMilliseconds = (int)(fDT * 1000.f);

          flashFX->update(iMilliseconds);
          i++;
        }
      }
    }
  }

  void SWFManager::Render()
  {
    //#if !defined(GAME_IOS)
    //glBindTexture( GL_TEXTURE_2D, 0 );
    //glActiveTexture( GL_TEXTURE0 );
    //glBindTexture( GL_TEXTURE_2D, 0 );
    //glUniform1i(m_UniformHandle, 0);

    //#endif
	
    RKDevice_DirtyDeviceCache();
		
    if (!m_bInitialised)
      return;

    // the list will have been sorted already
    int iWidth = RKDevice_GetWidth();
    int iHeight = RKDevice_GetHeight();

    int iStageWidth = 0;
    int iStageHeight = 0;
    int StageWidth = 0;
    int StageHeight = 0;
    
#if !defined(RKRETAIL)
    static bool cache_was_reset = false;
    bool cache_is_reset = gameswf::getDefaultContext()->m_glyph_provider->autoreset_cache();
    RKASSERT(!(cache_was_reset && cache_is_reset), "Too much font variation on screen! Do not ignore. Tell Roy and Gregor!");
    cache_was_reset = cache_is_reset;
#else
    gameswf::getDefaultContext()->m_glyph_provider->autoreset_cache();
#endif
    gameswf::getRenderHandler()->clearFrameBuffer();
    
    // loop through and draw all flash FX scenes
    unsigned int iSize = m_aFlashScenes.size(); 
    for (size_t i=0; i < iSize; ++i)
    {
      if (gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX)
      {
        StageWidth = flashFX->getViewportWidth();
        StageHeight = flashFX->getViewportHeight();

        flashFX->SetStageDimensions(StageWidth, StageHeight);
        flashFX->GetStageDimensions(iStageWidth,iStageHeight);

        flashFX->setViewport(0, 0, iWidth, iHeight);
        flashFX->setBounds(0,0,iStageWidth,iStageHeight);

        flashFX->render();
      }
    }
    //#if !defined(GAME_IOS)
    RKDevice_DirtyDeviceCache();
    //glEnable( GL_TEXTURE_2D );
    //glDepthMask( true );
    //glDepthFunc( GL_LESS );
    RKDevice_SetDepthFunc(RK_LEQUAL);
    RKDevice_SetDepthWriteState(RKTRUE);
    //#endif
  }

  bool SWFManager::OnTouchUp(float x, float y)
  {
    if(m_LockUserInputLeaks)
    {
      return false;
    }
      
    bool isOverFlashObject = false;
    gameswf::Point p(x, y);
    gameswf::RenderFX::Cursor cursor;
    cursor.m_pos = p;
    cursor.m_pressed = false;

    // loop through and draw all flash FX scenes
    if(m_pRestrictInputToScene)
    {
      isOverFlashObject = m_pRestrictInputToScene->updateCursor(cursor);
    }
    else
    {
      for (int i=m_aFlashScenes.size(); i--;)
      {
        gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX;

        if( m_pOmittedInputToScene.Contains( flashFX ) )
          continue;

        if (flashFX && m_aFlashScenes[i].InjectInput)
        {
          isOverFlashObject = flashFX->updateCursor(cursor);
          if (isOverFlashObject)
            return true;
        }
      }
    }

    return isOverFlashObject;
  }

  bool SWFManager::OnTouchDrag(float x, float y)
  {
    if(m_LockUserInputLeaks)
    {
      return false;
    }
    
    bool isOverFlashObject = false;
    gameswf::Point p(x, y);
    gameswf::RenderFX::Cursor cursor;
    cursor.m_pos = p;
    cursor.m_pressed = true;

    // loop through and update all flash FX scenes
    if(m_pRestrictInputToScene)
    {
      isOverFlashObject = m_pRestrictInputToScene->updateCursor(cursor);
    }
    else
    {
      for (int i=m_aFlashScenes.size(); i--;)
      {
        gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX;

        if( m_pOmittedInputToScene.Contains( flashFX ) )
          continue;

        if (flashFX && m_aFlashScenes[i].InjectInput)
        {
          isOverFlashObject = flashFX->updateCursor(cursor);
          if (isOverFlashObject)
            return true;
        }
      }
    }

    return isOverFlashObject;
  }

  bool SWFManager::OnTouchDown(float x, float y)
  {
    if(m_LockUserInputLeaks)
    {
      return false;
    }
    /// because the flash objects of gameswf appear to be running an interface where all input is polled to swf.(?)
    /// we have had to add in multiple updateCursor calls to get GameSWF into a state where it can handle "events".
    bool isOverFlashObject = false;
    gameswf::Point p(x, y);
    gameswf::RenderFX::Cursor cursor;
    cursor.m_pos = p;
    cursor.m_pressed = true;

    if(m_pRestrictInputToScene)
    {
      cursor.m_pressed = false;
      m_pRestrictInputToScene->updateCursor(cursor);
    
      // We need to check this here as the updateCursor can callback and set this to NULL
      if ( m_pRestrictInputToScene )
      {
        cursor.m_pressed = true;
        isOverFlashObject = m_pRestrictInputToScene->updateCursor(cursor);
      }
    }
    else
    {
      unsigned int iSize = m_aFlashScenes.size(); 
      for (size_t i=iSize; i--;)
      {
        gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX;

        if( m_pOmittedInputToScene.Contains( flashFX ) )
          continue;

        if (flashFX && m_aFlashScenes[i].InjectInput)
        {
          // first we have to inform flash that cursor has moved to
          // new position, without mentioning it as pressed
          // if we won't do this, stupid flash will send 
          // onDragOver event instead of onPress event
          // (that happens if previous untouch position 
          //  was outside and current position is inside)
          cursor.m_pressed = false;
          flashFX->updateCursor(cursor);

          cursor.m_pressed = true;
          isOverFlashObject = flashFX->updateCursor(cursor);
          if (isOverFlashObject)
            return true;
        }
      }
    }
    // loop through and draw all flash FX scenes

    // if touch is not processed by flash, Scene won't send TouchUp
    // in such case swf will think that cursor is still pressed
    // which leads to incorrect handling of next touch
    // here we update cursor as "not pressed" to avoid this
    if (!isOverFlashObject)
    {
      cursor.m_pressed = false;
      for (size_t i = m_aFlashScenes.size(); i--;)
      {
        gameswf::FlashFX* flashFX = m_aFlashScenes[i].FlashFX;
        if (flashFX && m_aFlashScenes[i].InjectInput)
        {
          isOverFlashObject |= flashFX->updateCursor(cursor);
        }
      }
    }

    return isOverFlashObject || m_pRestrictInputToScene != NULL;
  }

  void SWFManager::OnKeyPress(int keycode)
  {

  }

  void SWFManager::RK2Swf(float &x, float &y)
  {
    x = (1.f + x) * RKDevice_GetWidth() * 0.5f;
    y = (1.f - y) * RKDevice_GetHeight() * 0.5f;
  }

  void SWFManager::Swf2RK(float &x, float &y)
  {
    x = 2.f * x / RKDevice_GetWidth() - 1.f;
    y = 1.f - 2.f * y / RKDevice_GetHeight();
  }

  void SWFManager::DisableAllEnabled(std::vector<std::string>& disabledNames)
  {
    disabledNames.clear();
    for (unsigned int i = 0; i < m_aFlashScenes.size(); ++i)
    {
      if (m_aFlashScenes[i].FlashFX->getRootHandle().isEnabled())
      {
        m_aFlashScenes[i].FlashFX->getRootHandle().setEnabled(false);
        disabledNames.push_back(m_aFlashScenes[i].FlashFX->getFileName().c_str());
      }
    }
  }

  void SWFManager::EnableByNames(const std::vector<std::string>& names)
  {
    for (unsigned int i = 0; i < m_aFlashScenes.size(); ++i)
    {
      std::string fileName = m_aFlashScenes[i].FlashFX->getFileName().c_str();
      if (std::find(names.begin(), names.end(), fileName) != names.end())
      {
        m_aFlashScenes[i].FlashFX->getRootHandle().setEnabled(true);
      }
    }
  }

  gameswf::face_entity* SWFManager::GetFontFace(const char* fontname, bool bold, bool italic)
  {
    gameswf::PlayerContext* context = gameswf::getDefaultContext();
    RKASSERT(context, "Missing GameSWF context");
    RKASSERT(context->m_glyph_provider, "Missing GameSWF glyph provider");
    return context->m_glyph_provider->get_face_entity(fontname, bold, italic);
  }

  bool SWFManager::RestrictTextToSingleLineBounds( gameswf::FlashFX& flash, gameswf::CharacterHandle textHandle, const char* text )
  {
    RKString rk_text = text;
    gameswf::ASValue text_param = text;
    textHandle.invokeMethod("setLabel", &text_param, 1);

    gameswf::CharacterHandle tf = flash.find( "tf", textHandle );
    gameswf::EditTextCharacter* textObj = gameswf::castTo<gameswf::EditTextCharacter>(tf.getCharacter());

    if( textObj != NULL )
    {
#ifndef WIDTH_FUDGE
#define WIDTH_FUDGE     TO_PIXELS(80.0f) /// from gameswf_text.cpp
#endif
      gameswf::Rect bounds;
      tf.getBound( &bounds );
      float  bounds_width = bounds.width();

      float zoneMarginLeft = 0, zoneMarginRight = 0;
      textObj->getCurrentZoneMargins(zoneMarginLeft, zoneMarginRight);

      bounds_width -= ( zoneMarginLeft + zoneMarginRight + textObj->m_right_margin + textObj->m_left_margin + WIDTH_FUDGE );

      uint32 i = 0;
      for( ; bounds_width > 0.0f && i < rk_text.LengthUTF(); ++i )
      {
        textObj->getCharBoundaries( i, &bounds, true );
        bounds_width -= bounds.width() + textObj->m_letter_spacing;
      }

      if( i >= rk_text.LengthUTF() - 1 )
      {
        return false;
      }

      rk_text.AssignUTF32( rk_text.CreateDecodedBufferUTF32(), i - 3 );
      rk_text.Append( "..." );

      gameswf::ASValue second_pass = rk_text.GetString();
      textHandle.invokeMethod("setLabel", &second_pass, 1);
      return true;
    }
    return false;
  }

  void SWFManager::DumpDynamicTextures()
  {
    for (unsigned int i = 0; i < m_aFlashScenes.size(); ++i)
    {
      gameswf::ASEnvironment asEnv(m_aFlashScenes[i].FlashFX->getPlayer());
      asEnv.FlushAllTextures();
    }
  }

  void SWFManager::ForceResetFocus()
  {
    for (unsigned int i = 0; i < m_aFlashScenes.size(); ++i)
    {
      if (m_aFlashScenes[i].FlashFX->getRootHandle().isEnabled())
      {
        for (int j = 0; j < gameswf::RenderFX::CONTROLLER_COUNT; ++j)
        {
          m_aFlashScenes[i].FlashFX->forceDragOffEvent(j); 
        }
      }
    }
  }

  void SWFManager::EventCallback(const gameswf::ASNativeEventState& eventState)
  {
    RKString* eventstr = (RKString*) eventState.m_userPtr;
    RKASSERT(eventstr, "No event string supplied");
    Game::GetInstance()->GetCurrentState()->OnGuiEvent(eventstr->GetString());
  }

  void SWFManager::SubscribeToEvent(const char* flashfile, const char* controlname, const char* eventname, const char* tag)
  {
    if (gameswf::FlashFX* swfref = GetFlashFX(flashfile))
    {
      gameswf::CharacterHandle chr = swfref->find(controlname);
      if (chr.isValid())
      {
        SWFEventCallback* cb = RKNEW(SWFEventCallback);
        cb->flashfile = flashfile;
        cb->control = controlname;
        cb->eventname = eventname;
        cb->tag = tag;

        m_eventcallbacks.Insert(cb, tag);

        chr.addEventListener(eventname, SWFManager::EventCallback, &cb->tag);
      }
    }
  }

  void SWFManager::UnsubscribeFromEvent(const char* tag)
  {
    RKASSERT(tag, "Invalid tag");
    RKHashTable<SWFEventCallback*>::Iterator itr = m_eventcallbacks.FindKey(tag);
    if (!itr.IsValid())
    {
      RKLOG_ERROR("Unsubscribing from invalid SWF event: %s", tag);
      return;
    }

    SWFEventCallback* cb = itr.Value();
    if (gameswf::FlashFX* swfref = GetFlashFX(cb->flashfile.GetString()))
    {
      gameswf::CharacterHandle chr = swfref->find(cb->control.GetString());
      if (chr.isValid())
      {
        chr.removeEventListener(cb->eventname.GetString(), SWFManager::EventCallback);
      }
    }
    m_eventcallbacks.RemoveKey(tag);
    RKDELETE(cb);
  }

  void SWFManager::UnsubscribeAllEvents()
  {
    for (RKHashTable<SWFEventCallback*>::Iterator itr = m_eventcallbacks.Begin(); itr.IsValid(); ++itr)
    {
      SWFEventCallback* cb = itr.Value();
      if (gameswf::FlashFX* swfref = GetFlashFX(cb->flashfile.GetString()))
      {
        gameswf::CharacterHandle chr = swfref->find(cb->control.GetString());
        if (chr.isValid())
        {
          chr.removeEventListener(cb->eventname.GetString(), SWFManager::EventCallback);
        }
      }
      RKDELETE(itr.Value());
    }
    m_eventcallbacks.Clear();
  }
  
  void SWFManager::SetLanguage( int language )
  {
    if( m_pHostInterface )
    {
      CasualCore::FlashFXList::iterator itr = m_aFlashScenes.begin();
      CasualCore::FlashFXList::iterator end = m_aFlashScenes.end();

      while( itr != end )
      {
        gameswf::clearFonts( itr->FlashFX->getPlayerContext() );
        ++itr;
      }

      m_pHostInterface->onLanguageChange( language );
      gameswf::clearGlyphTextureCaches(NULL);
    }
  }

  void SWFManager::UpdateLoadingScenes( float fDT )
  {
    int iMilliseconds = (int)(fDT * 1000.f);

    // loop through any flash scenes that are being background loaded
    // and complete them if they are finished loading
    BackgroundLoadFXList::iterator it;
    for (it = m_aLoadingFlashScenes.begin(); it != m_aLoadingFlashScenes.end(); it++)
    {
      if (BackgroundLoadFX* loadFX = (*it))
      {
        bool bLoaded = false;
        RKCriticalSection_Enter(m_pLoadedCheckMutex);
        bLoaded = loadFX->Loaded;
        RKCriticalSection_Leave(m_pLoadedCheckMutex);

        if (bLoaded)
        {
          m_aLoadingFlashScenes.erase(it);

          // clean up the thread
          if (loadFX->LoadingThread)
          {
            RKThread_Destroy(&loadFX->LoadingThread);
          }

          // call the loading complete callback
          if (loadFX->Callback)
          {
            loadFX->Callback(loadFX->FlashFX, loadFX->CallbackData);
          }
          RKDELETE(loadFX);

          break;
        }
      }
    }
  }

}

#endif //#if CC_USE_GAMESWF==1

