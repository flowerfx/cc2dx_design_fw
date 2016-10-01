#ifndef __CASUALCORE_WIN32FBO_H__
#define __CASUALCORE_WIN32FBO_H__

#include "RKResource.h"

class RKGeometryChunk;
class RKCamera;
class RKTexture;
struct RKRenderTexture;

namespace CasualCore
{
  class Win32FBO
  {
    private:
      RKGeometryChunk* m_geometry;
      RKRenderTexture* m_rendertexture;
      RKCamera* m_camera;
	  RKFramebufferHandle m_mainFramebuffer;

    public:
      Win32FBO();
      ~Win32FBO();

      void Init();
      void Deinit();
      void BufferRenders();
      void BlitToScreen();
	  void UpdateMainFBO(RKFramebufferHandle fbo){
		  m_mainFramebuffer = fbo;
	  }
      RKTexture* GetDepthTexture();

#ifdef OS_WP8
	  void OnResourceLost();
	  void OnResourceRestore();
#endif
  };
}

#endif