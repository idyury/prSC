
#ifndef xpApp_INCLUDED
#define xpApp_INCLUDED

#include "Core/Scene/prScene.h"
#include "Core/Scene/prScenePipeline.h"
#include "OpenGL/prGL.h"
#include "Core/Render/Pipeline/prDefaultRenderPipeline.h"
#include "Core/Scene/Camera/prComponentCamera.h"
#include "Core/Scene/Visual/prComponentSprite.h"
#include "Core/System/FileSystem/prVirtualFileSystem.h"

#include "prCameraUpdater.h"
#include "prCellVisual.h"
#include "prWorld.h"
#include "prPositionProvider.h"
#include "prInputState.h"

class prSCApp: private prScenePipeline
              , private prScene::Listener
{
public:
  prSCApp( );
  ~prSCApp( );

  void Init(void* hWnd);
  void Free();

  void Switch( );

  virtual void Tick(unsigned int revision);

  void SetDolly(float v) { m_inputState.m_dolly = v; }
  void SetOffset(float v) { m_inputState.m_offset = v; }
  void RotateLeftRight(float v) { m_inputState.m_leftRight = v; }
  void RotateUpDown(float v) { m_inputState.m_upDown = v; }
  void SetDepthScale(float v) { m_inputState.m_depthScale = v; }
private:
    class WorldTextureResolver;

    prGLRenderDevice*     	    m_pDevice;
    prDefaultRenderPipeline     m_pipeLine;
    prViewportf         		m_viewPort;
    prComponentCamera::tSP 	    m_camera;
    prComponentLight::tSP 	    m_light;
    prScene 					m_scene;
    prWorld::tSP				m_world;
    prPositionProvider::tSP	m_position;
    prObjectArrayT<prCellVisual::tSP> m_prims;
    prVirtualFileSystem::tSP m_virtualFS;
    WorldTextureResolver*		m_texResolver;

  prMeshf::tSP LoadMesh();
  void Reload();

  prInputState m_inputState;

  virtual void OnScenePipeLineStart();
  virtual void OnScenePipeLineEnd();

  virtual void OnPreState(prScene* in_pScene, prScene::State in_state);

  void SwitchEffect( );
};

#endif // xpApp_INCLUDED
