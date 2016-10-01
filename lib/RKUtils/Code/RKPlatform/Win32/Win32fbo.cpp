#include "Win32fbo.h"
#include "RKRenderTexture.h"
#include "RKRender.h"
#include "RKMaterial.h"
#include "RKVertexBuffer.h"
#include "RKDevice.h"
#include "RKCamera.h"
#include "VertexDeclarations.h"

namespace CasualCore
{
  Win32FBO::Win32FBO() :
    m_geometry(0),
    m_rendertexture(RKNEW(RKRenderTexture)),
    m_camera(RKNEW(RKCamera))
  {
  }

  Win32FBO::~Win32FBO()
  {
    Deinit();
    RKDELETE(m_camera);
    RKDELETE(m_rendertexture);
  }

  void Win32FBO::Init()
  {
		m_mainFramebuffer = RKDevice_GetFrameBuffer();

    *m_rendertexture = RKRenderTexture_CreateWin32Buffer();
    m_geometry = RKRender_CreateGeometryChunk(0);
    m_geometry->m_primitiveType = RKPT_TRIANGLESTRIP;
    m_geometry->m_primitiveCount = 2;
    m_geometry->m_pVB = RKVertexBuffer_Create(RKTileVertex::Create(), 4, RKBUFFER_ACCESS_WRITE, RKBUFFER_UPDATE_RARELY);
    m_geometry->m_pIB = 0;
    m_geometry->m_indexOffset = 0;
    m_geometry->m_pMaterial = RKMaterial_Create("RKWin32Color");

#ifdef RKDEVICE_DX11
		const float minV = 1;
		const float maxV = 0;
#else
		const float minV = 0;
		const float maxV = 1;
#endif

    if (RKTileVertex* pData = (RKTileVertex*) RKVertexBuffer_Lock(m_geometry->m_pVB))
    {
      pData->color.SetOne();
      pData->position.Set(-1.f, -1.f, 10.f);
      pData->uv.Set(0.f, minV);
      ++pData;

      pData->color.SetOne();
      pData->position.Set(-1.f, 1.f, 10.f);
      pData->uv.Set(0.f, maxV);
      ++pData;

      pData->color.SetOne();
      pData->position.Set(1.f, -1.f, 10.f);
      pData->uv.Set(1.f, minV);
      ++pData;

      pData->color.SetOne();
      pData->position.Set(1.f, 1.f, 10.f);
      pData->uv.Set(1.f, maxV);

      RKVertexBuffer_Unlock(m_geometry->m_pVB);
    }

    m_camera->SetOrthoCamera(-1.f, 1.f, -1.f, 1.f, 10.f, 100.f);
  }

  void Win32FBO::Deinit()
  {
    if (m_geometry)
    {
      RKVertexBuffer_Destroy(&m_geometry->m_pVB);
      RKMaterial_Destroy(&m_geometry->m_pMaterial);
      RKRender_DestroyGeometryChunk(&m_geometry);
      RKRenderTexture_Destroy(*m_rendertexture);
    }
  }

  void Win32FBO::BufferRenders()
  {
    RKRenderTexture_SetCurrent(*m_rendertexture);		
  }

#ifdef OS_WP8
extern bool gNeedRotateMatrix;
#endif

  void Win32FBO::BlitToScreen()
  {
    RKCamera* camera = RKCamera_GetCurrent();
    RKCamera_SetCurrent(m_camera);
    RKDevice_SetFrameBuffer(m_mainFramebuffer);
		RKDevice_Clear();

#ifdef OS_WP8
	gNeedRotateMatrix = true;
#endif
	void RKRender_DrawGeometryChunkInternal(const RKGeometryChunk& geometryChunk);
    RKRender_DrawGeometryChunkInternal(*m_geometry);
    RKCamera_SetCurrent(camera);
  }

  RKTexture* Win32FBO::GetDepthTexture()
  {
    return m_rendertexture->depthtexture;
  }

#ifdef OS_WP8
  void Win32FBO::OnResourceLost()
  {
	  if(m_rendertexture)	  m_rendertexture->OnResourceLost();
  }

  void Win32FBO::OnResourceRestore()
  {
	  if(m_rendertexture)	  m_rendertexture->OnResourceRestore();
  }
#endif
}
