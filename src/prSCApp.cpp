
#include "Core/prCoreDefs.h"
#include "prSCApp.h"
#include "Core/Input/prInputManager.h"
#include "Core/Render/Model/prObjectRenderModelManager.h"
#include "Core/Render/Data/Material/prMaterialResourceManager.h"
#include "Core/Render/Data/Shader/prShaderResourceManager.h"
#include "Core/Render/Data/Texture/prTextureResourceManager.h"
#include "Core/Physics/Shape/prPhysicsShapeResourceManager.h"
#include "Core/Render/prRenderUtils.h"
#include "Core/System/FileSystem/prFileSystem.h"
#include "Core/Stream/prStreamFactory.h"
#include "Core/Render/Hardware/prRenderDevice.h"
#include "Utils/Scene/utZipTextureResolver.h"

#include "Core/Geometric/Mesh/Primitive/prSphereMeshOp.h"
#include "Core/Geometric/Mesh/Transform/prSixAxesScaleMeshOp.h"
#include "Core/Geometric/Mesh/Misc/prCalculateNormalSmoothMeshOp.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/Geometric/Mesh/Misc/prMeshEdgeMapper.h"
#include "Core/Geometric/Mesh/Misc/prCalculateNormalMeshOp.h"

#include "Core/Geometric/Mesh/IO/prMeshIO.h"
#include "Core/Stream/prStreamFactory.h"
#include "Core/Scene/Primitives/prMeshPrimitive.h"
#include "Core/System/FileSystem/prFileSystem.h"


#include "prBoatOp.h"
#include "prNavSpline.h"
#include "prNavSplinePositionProvider.h"
#include "prCameraUpdater.h"
#include "prBoatOp.h"
#include "prBoatUpdater.h"
#include "prDepthScaleUpdater.h"

class prSCApp::WorldTextureResolver : public prResourceManager::ResourceResolver {
public:
	prInputStream::tSP ResolveResourceStream(const prStringID& in_resourceID) {
		prString path = prStringConv::ToString(in_resourceID);
		path += _T(".dds");
		return GetFileSystem()->OpenFile(path);
	};
};

prSCApp::prSCApp( )
  : m_pDevice( NULL ) {

#ifdef ANDROID_NDK
    prString vsPath = GetSystem()->GetRootPath();
    vsPath += _T("/data.vfs");
    prRAInputStream::tSP stream = GetStreamFactory()->CreateRAFileInputStream(vsPath);
    prAssert(stream != NULL);

    m_virtualFS = new prVirtualFileSystem();
    if (!m_virtualFS->Open(stream)) {
        prAssert(false);
    }

    GetFileSystem()->Mount(m_virtualFS);
#endif

  m_camera = new prComponentCamera();
  m_viewPort.SetCamera(m_camera);

  m_camera->SetPosition( prVector3f( 0.0f, 0.0f, -2.0f ) );
  m_camera->SetUp(prVector3f::AXIS_Y);
  prVector3f dir(-1.0f, -1.0f, -1.0f);
  dir.Normalize();
  m_camera->SetDirection(dir);
  m_camera->SetPosition(-dir * 20.0f);

  m_viewPort.SetNear( 1.00f );
  m_viewPort.SetFar( 2500.0f );
  m_viewPort.SetFOV( prMathf::PI() / 3.0f );
  m_viewPort.SetAspect(1.3333f);

  m_scene.GetHierarhy().SetCellSize(prVector3f(50.0f, 50.0f, 50.0f));

  m_texResolver = new WorldTextureResolver();
  GetTextureResourceManager()->RegisterResourceResolver(m_texResolver);
}

prSCApp::~prSCApp()
{
  prAssert(m_pDevice == NULL);
#ifdef ANDROID_NDK
    GetFileSystem()->Unmount(m_virtualFS);
#endif
    GetTextureResourceManager()->UnregisterResourceResolver(m_texResolver);
}

void prSCApp::Switch( )
{
}

void prSCApp::Reload() {

	m_world = prWorld::Load(_T("santa_cruz"));
	if (!m_world)
		return;
	m_world->Attach(m_scene);

    prString splinePath = GetSystem()->GetRootPath();
    splinePath += _T("/world/santa_cruz_spline.txt");
    prNavSpline::tSP spline = prNavSpline::Load(*m_world, splinePath);
	prNavSplinePositionProvider::tSP provider = new prNavSplinePositionProvider(spline);
	provider->SetTime(500.0f);
	provider->SetScale(15.0f);
	m_position = provider;

	prCameraUpdater::tSP updater = new prCameraUpdater(
			m_world, m_inputState, m_camera, m_position);
	m_scene.AddComponent(updater);

	prMeshPrimitive::tSP boat = new prMeshPrimitive();
	boat->SetMaterialID("system/primitive_ztest_nonorm_blend");
	boat->SetColor(0x400000ff);
	boat->SetMeshOp(new prBoatOpf());
	m_scene.AddComponent(boat);

	prBoatUpdater::tSP boatUpdater = new prBoatUpdater(boat, m_position);
	m_scene.AddComponent(boatUpdater);

	m_scene.AddComponent(new prDepthScaleUpdater(*m_world, m_inputState));
}

#include <vector>

struct Sample {
	int mX;
	int mY;
	float mDepth;
	int mBottom;
	int mType;
};

prMeshf::tSP prSCApp::LoadMesh() {
	prMeshf::tSP mesh = new prMeshf();
	prInputStream::tSP input = GetStreamFactory()->CreateFileInputStream(_T("/home/khmel/Downloads/ac/sc/test1/data.msh"));
	if (!prMeshIOf::Load(*mesh, input)) {
		return NULL;
	}
	return mesh;
}

void prSCApp::Init(void* hWnd)
{
  // create graphic device
  m_pDevice = CreateGLDevice( );
  prAssert( m_pDevice != NULL );

  // start pipeline
  m_pDevice->SetWindow( hWnd );

  Reload();

  m_light = new prComponentLight();
  prMatrix4f light1_transform(prVector3f::AXIS_X, -0.5f * prMathf::PI());
  m_light->SetAmbientColor(prVector3f(0.4f, 0.4f, 0.4f));
  m_light->SetDiffuseColor(prVector3f(0.6f, 0.6f, 0.6f));
  m_light->SetTransform(light1_transform);
  m_scene.AddComponent(m_light);

  m_pipeLine.SetUseFrameBuffer(false);
  m_pipeLine.StartUpMT( &m_scene, &m_viewPort, m_pDevice );

  // start scene
  StartUp(&m_scene, 33);
  SetRenderDevice( m_pDevice );

  // register scene listener
  m_scene.AddListener(prScene::STATE_Update, this);
}

void prSCApp::Free()
{
  // deregister scene listener
  m_scene.RemoveListener(prScene::STATE_Update, this);
  // shut down scene
  ShutDown( );

  m_scene.Reset( );
  prRenderUtils::CleanUp( );

  // shut down pipeline
  m_pipeLine.ShutDown();

  // destroy graphic device
  delete m_pDevice;
  m_pDevice = NULL;
}

void prSCApp::SwitchEffect( )
{
}

void prSCApp::OnPreState(prScene* in_pScene, prScene::State in_state)
{
}
//---------------------------------------------------------------------------------------------//
void prSCApp::OnScenePipeLineStart()
{
}

void prSCApp::OnScenePipeLineEnd()
{
  m_pDevice->Lock();

  GetObjectRenderModelManager()->CleanUp();
  GetMaterialResourceManager()->CleanUp();
  GetShaderResourceManager()->CleanUp();
  GetTextureResourceManager()->CleanUp();
  GetPhysicsShapeResourceManager()->CleanUp();

  m_pDevice->Unlock();
}

void prSCApp::Tick(unsigned int revision) {
	m_position->Update(0.033f);
	prScenePipeline::Tick(revision);
}
