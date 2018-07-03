#include "prCoreDefs.h"

#include "prCameraUpdater.h"

#define HANGLE_SPEED 1.0f
#define VANGLE_SPEED 0.5f
#define VANGLE_MIN 0.0f
#define VANGLE_MAX prMathf::HALF_PI()
#define DISTANCE_SPEED 40.0f
#define DISTANCE_MIN 5.0f
#define DISTANCE_MAX 200.0f
#define HEIGHT_SPEED 1.0f

prCameraUpdater::prCameraUpdater(
		const prWorld::tSP& world,
		const prInputState& inputState,
		const prComponentCamera::tSP& camera,
		const prPositionProvider::tSP& provider)
  : m_world(world),
	m_inputState(inputState),
	m_camera(camera),
	m_provider(provider),
	m_freeDirection(true),
	m_hAngle(0.0f),
	m_vAngle(0.5f * prMathf::QUARTER_PI()),
	m_distance(100) {
}

prCameraUpdater::~prCameraUpdater() {
}

prObjectDefs::GroupType prCameraUpdater::GetType() const {
	return prObjectDefs::GT_UpdateAble;
}

void prCameraUpdater::Update(float in_delta) {
	m_distance = prMathf::Clamp(
			m_distance - in_delta * m_inputState.m_dolly * DISTANCE_SPEED,
			DISTANCE_MIN, DISTANCE_MAX);
	m_vAngle = prMathf::Clamp(
			m_vAngle + in_delta * m_inputState.m_upDown * VANGLE_SPEED,
			VANGLE_MIN, VANGLE_MAX);
	m_hAngle = m_hAngle - in_delta * m_inputState.m_leftRight * HANGLE_SPEED;
	while (m_hAngle < 0) m_hAngle += 2 * prMathf::PI();
	while (m_hAngle > 2 * prMathf::PI()) m_hAngle -= 2 * prMathf::PI();


	prVector3f pos = m_provider->GetPosition();
	prVector3f dir, lat;
	if (m_freeDirection) {
		dir = prVector3f::AXIS_Z;
	} else {
		dir = m_provider->GetDir();
	}
	prMatrix4f(prVector3f::AXIS_Y, m_hAngle).RotateVector3(dir);
	prVector3f view_pos = pos + dir * m_distance;

	lat.FromCrossProduct(prVector3f::AXIS_Y, dir);
	prMatrix4f(lat, m_vAngle).RotateVector3(dir);

	view_pos.m_y = m_height;

	prVector3f camera_dir = -dir;
	camera_dir.Normalize();
	m_camera->SetPosition(view_pos);
	m_camera->SetDirection(camera_dir);
}

