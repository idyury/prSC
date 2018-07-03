
#include "Core/prCoreDefs.h"
#include "prSCLineSim.h"

#include "Core/Render/Model/prObjectRenderModelManager.h"
#include "Core/Render/Data/Material/prMaterialResourceManager.h"
#include "Core/Render/Data/Shader/prShaderResourceManager.h"
#include "Core/Render/Data/Texture/prTextureResourceManager.h"
#include "Core/Physics/Shape/prPhysicsShapeResourceManager.h"
#include "Core/Render/prRenderUtils.h"
#include "Core/Scene/Primitives/prMeshPrimitive.h"
#include "Core/Geometric/Mesh/Primitive/prCubeMeshOp.h"
#include "Core/Geometric/Mesh/Transform/prScaleMeshOp.h"
#include "Core/Geometric/Mesh/Transform/prOffsetMeshOp.h"
#include "Core/Scene/Physics/prComponentPhysics.h"
#include "Core/Geometric/Mesh/Primitive/prCylinderMeshOp.h"
#include "Core/Geometric/Mesh/Primitive/prSphereMeshOp.h"
#include "Core/Geometric/Mesh/Transform/prTransformMeshOp.h"

namespace {

prBoxf groundDim(prVector3f(-30.0f, -0.1f, -30.0f),
		         prVector3f(30.0f, 0.1f, 30.0f));
prCylinderf cylDim(prVector3f::ZERO, prVector3f::AXIS_Y, 0.2f, 0.025f);
prSpheref lockPointDim(prVector3f::ZERO, 0.2f);
const double leadDensity = 11300.;
const double waterDensity = 1000.;

const float erp = 0.5f;
const float cfm = 0.0f;
const float dumping = 1.0f;

const int bodyCnt = 100;
const float height = 13.0f;
const float water_height = 10.0f;
const float segmentWeight = 0.00025f;
const float currentSpeed = 1.0f;

const prSpheref sinkerDim(prVector3f::ZERO, 0.25f);

const double lineDiameter = 0.2 * 0.001;
const float sinkerWeight = 1.0f * 0.028f;

prMeshf::tSP GetBoxMesh(const prBoxf& box) {
  prMeshf::tSP mesh = new prMeshf();
  prCubeMeshOpf().Apply(*mesh);
  prOffsetMeshOpf(prVector3f(box.GetCenter())).Apply(*mesh);
  prScaleMeshOpf(prVector3f(
			0.5f * box.GetWidth() / prCubeMeshOpf::HALF_LENGTH,
			0.5f * box.GetHeight() / prCubeMeshOpf::HALF_LENGTH,
			0.5f * box.GetDepth() / prCubeMeshOpf::HALF_LENGTH)).Apply(*mesh);
  return mesh;
}

prMeshf::tSP GetCylMesh(const prCylinderf& cylinder) {
  prMeshf::tSP mesh = new prMeshf();
  prCylinderMeshOpf(3).Apply(*mesh);
  prOffsetMeshOpf(prVector3f(cylinder.m_position)).Apply(*mesh);
  prScaleMeshOpf(prVector3f(
			cylinder.m_radius / prCylinderMeshOpf::R,
			0.5f * cylinder.m_height / prCylinderMeshOpf::H,
			cylinder.m_radius / prCylinderMeshOpf::R)).Apply(*mesh);
  prAssert(cylinder.m_up == prVector3f::AXIS_Y);
  return mesh;
}

prMeshf::tSP GetSphereMesh(const prSpheref& sphere) {
  prMeshf::tSP mesh = new prMeshf();
  prSphereMeshOpf(2).Apply(*mesh);
  prOffsetMeshOpf(prVector3f(sphere.m_pos)).Apply(*mesh);
  prScaleMeshOpf(prVector3f(sphere.m_radius, sphere.m_radius, sphere.m_radius)).Apply(*mesh);
  return mesh;
}

class WeightPhysics : public prComponentPhysics {
 public:
	typedef prSmartPointerT<WeightPhysics> tSP;

	void OnPhysicsReady() {
		const prVector3f pos = GetTransform().GetPosition();
		if (pos.m_y > water_height)
			return;
		prVector3f speed = GetActor()->GetLinearSpeed();
		speed -= (prVector3f::AXIS_X * currentSpeed);
		const float ls = speed.Length();
		if (ls <= 0.00001f)
			return;
		const double v = sinkerWeight / leadDensity;
		const double r = pow(v * 3.0 / (4.0 * prMathd::PI()), 0.33333f);
		const double sq = prMathd::PI() * r * r;
		const double f = 0.5 * waterDensity * 0.5 * sq * ls * ls;
		prVector3f force = -speed;
		force.Normalize(f);
		force += prVector3f::AXIS_Y * (10.0f * v * waterDensity);
		GetActor()->ApplyForce(force, prVector3f::ZERO);
	}
};

class SegmentPhysics : public prComponentPhysics {
 public:
	typedef prSmartPointerT<SegmentPhysics> tSP;

	void OnPhysicsReady() {
		const prVector3f pos = GetTransform().GetPosition();
		const prVector3f up = GetTransform().GetUp();
		if (pos.m_y > water_height)
			return;
		prVector3f speed = GetActor()->GetLinearSpeed();
		speed -= (prVector3f::AXIS_X * currentSpeed);
		const float ls = speed.Length();
		if (ls <= 0.00001f)
			return;
		speed.Normalize();
		double sq = lineDiameter * cylDim.m_height;
		const float dp = prMathf::Clamp(speed.DotProduct(up), -1.0f, 1.0f);
		sq = sq * pow(1.0 - dp * dp, 0.5);
		const double f = 0.75 * waterDensity * 0.5 * sq * ls * ls;
		prVector3f force = -speed;
		force.Normalize(f);
		GetActor()->ApplyForce(force, prVector3f::ZERO);
	}
};

}

prSCLineSim::prSCLineSim()
  : m_pDevice( NULL ) {

  m_camera = new prComponentCamera();
  m_viewPort.SetCamera(m_camera);

  m_camera->SetPosition( prVector3f( 0.0f, height * 0.15f, -25.0f ) );
  m_camera->SetUp(prVector3f::AXIS_Y);
  m_camera->SetDirection(prVector3f::AXIS_Z);

  m_viewPort.SetNear( 1.00f );
  m_viewPort.SetFar( 100.0f );
  m_viewPort.SetFOV( prMathf::PI() / 3.0f );
  m_viewPort.SetAspect(1.3333f);
}

prSCLineSim::~prSCLineSim() {
  prAssert(m_pDevice == NULL);
}

void prSCLineSim::Reload() {
  prMeshPrimitive::tSP ground = new prMeshPrimitive();
  ground->SetColor(0xffd0ffff);
  ground->SetMesh(GetBoxMesh(groundDim));

  prComponentPhysics::tSP groundPh = new prComponentPhysics();
  groundPh->SetTransformable(ground->GetTransformable());
  prMatrix4f groundTransform = prMatrix4f::IDENTITY;
  groundPh->SetTransform(groundTransform);
  groundPh->SetActor(
      GetPhysics()->CreateStaticActor(GetPhysics()->CreateShape(groundDim)));
  m_scene.AddComponent(ground);
  m_scene.AddComponent(groundPh);

  prComponentPhysics::tSP lastActor;
  prVector3f lastActorConnectPoint;

  prMeshPrimitive::tSP lockPoint = new prMeshPrimitive();
  lockPoint->SetColor(0xffffffd0);
  lockPoint->SetMesh(GetSphereMesh(lockPointDim));

  prComponentPhysics::tSP lockPointPh = new prComponentPhysics();
  lockPointPh->SetTransformable(lockPoint->GetTransformable());
  prMatrix4f lockPointTransform = prMatrix4f::IDENTITY;
  lockPointTransform.SetPosition(prVector3f(0, height, 0));
  lockPointPh->SetTransform(lockPointTransform);
  lockPointPh->SetActor(
      GetPhysics()->CreateStaticActor(GetPhysics()->CreateShape(lockPointDim)));

  m_scene.AddComponent(lockPoint);
  m_scene.AddComponent(lockPointPh);
  lastActor = lockPointPh;
  lastActorConnectPoint = prVector3f::ZERO;

  prVector3f posCur(0.0f, cylDim.m_height * 0.5f, 0.0f);
  for (int i = 0; i < bodyCnt; ++i) {
    prMeshPrimitive::tSP box = new prMeshPrimitive();
    box->SetColor(0xffffffd0);
    box->SetMesh(GetCylMesh(cylDim));

    SegmentPhysics::tSP boxPh = new SegmentPhysics();
    boxPh->SetTransformable(box->GetTransformable());
    prMatrix4f bodyTransform(prVector3f::AXIS_Z, -prMathf::HALF_PI());
    bodyTransform.SetPosition(prVector3f(cylDim.m_height * i, height, 0));
    boxPh->SetTransform(bodyTransform);
    boxPh->SetActor(
        GetPhysics()->CreateGhostActor(GetPhysics()->CreateShape(cylDim), segmentWeight));
    if (lastActor) {
    	prPhysicsConnection::tSP connection = GetPhysics()->
    			CreatePointConnection(lastActor->GetActor(),
    					boxPh->GetActor(),
						lastActorConnectPoint,
						posCur,
						erp,
						cfm,
						dumping);
    	prComponentPhysics::tSP compCon = new prComponentPhysics();
        compCon->SetConnection(connection);
    	m_scene.AddComponent(compCon);
    }

    m_scene.AddComponent(box);
    m_scene.AddComponent(boxPh);
    lastActor = boxPh;
    lastActorConnectPoint = prVector3f(0.0f, -cylDim.m_height * 0.5f, 0.0f);
  }

  prMeshPrimitive::tSP sinker = new prMeshPrimitive();
  sinker->SetColor(0xff0000ff);
  sinker->SetMesh(GetSphereMesh(sinkerDim));

  WeightPhysics::tSP sinkerPh = new WeightPhysics();
  sinkerPh->SetTransformable(sinker->GetTransformable());
  prMatrix4f sinkerTransform(prVector3f::AXIS_Z, -prMathf::HALF_PI());
  sinkerTransform.SetPosition(prVector3f(cylDim.m_height * bodyCnt, height, 0));
  sinkerPh->SetTransform(sinkerTransform);
  sinkerPh->SetActor(
      GetPhysics()->CreateDynamicActor(GetPhysics()->CreateShape(sinkerDim), sinkerWeight));
  {
  	prPhysicsConnection::tSP connection = GetPhysics()->
  			CreatePointConnection(lastActor->GetActor(),
  					sinkerPh->GetActor(),
						lastActorConnectPoint,
						prVector3f::ZERO,
						erp,
						cfm,
						dumping);
  	prComponentPhysics::tSP compCon = new prComponentPhysics();
    compCon->SetConnection(connection);
  	m_scene.AddComponent(compCon);
  }
  m_scene.AddComponent(sinker);
  m_scene.AddComponent(sinkerPh);
}

void prSCLineSim::Init(void* hWnd) {
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
  StartUp(&m_scene, 1);
  SetRenderDevice( m_pDevice );

  // register scene listener
  m_scene.AddListener(prScene::STATE_Update, this);
}

void prSCLineSim::Free() {
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

void prSCLineSim::OnPreState(prScene* in_pScene, prScene::State in_state) {
}
//---------------------------------------------------------------------------------------------//
void prSCLineSim::OnScenePipeLineStart() {
}

void prSCLineSim::OnScenePipeLineEnd()
{
  m_pDevice->Lock();

  GetObjectRenderModelManager()->CleanUp();
  GetMaterialResourceManager()->CleanUp();
  GetShaderResourceManager()->CleanUp();
  GetTextureResourceManager()->CleanUp();
  GetPhysicsShapeResourceManager()->CleanUp();

  m_pDevice->Unlock();
}

void prSCLineSim::Tick(unsigned int revision) {
	//m_world->Simulate(0.016f);
	prScenePipeline::Tick(revision);
}
