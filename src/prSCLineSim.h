#ifndef prSCLineSim_INCLUDED
#define prSCLineSim_INCLUDED

#include "Core/Scene/prScene.h"
#include "Core/Scene/prScenePipeline.h"
#include "OpenGL/prGL.h"
#include "Core/Render/Pipeline/prDefaultRenderPipeline.h"
#include "Core/Scene/Camera/prComponentCamera.h"
#include "Core/Physics/prPhysics.h"

class prSCLineSim: private prScenePipeline
                 , private prScene::Listener
{
public:
  prSCLineSim();
  ~prSCLineSim();

  void Init(void* hWnd);
  void Free();

  virtual void Tick(unsigned int revision);
private:
  prGLRenderDevice*     	m_pDevice;
  prDefaultRenderPipeline   m_pipeLine;
  prViewportf         		m_viewPort;
  prComponentCamera::tSP 	m_camera;
  prComponentLight::tSP 	m_light;
  prScene 					m_scene;
  prPhysicsWorld::tSP		m_world;
  void Reload();

  virtual void OnScenePipeLineStart();
  virtual void OnScenePipeLineEnd();

  virtual void OnPreState(prScene* in_pScene, prScene::State in_state);
};

#endif // prSCLineSim_INCLUDED
