#include "prCoreDefs.h"

#include "prDepthScaleUpdater.h"
#include "Core/Render/Data/Material/prMaterialResourceManager.h"

#define DEPTH_SCALE_SPEED 1.0f
#define DEPTH_SCALE_MIN 1.0f
#define DEPTH_SCALE_MAX 25.0f

prDepthScaleUpdater::prDepthScaleUpdater(const prWorld& world, const prInputState& inputState)
  : m_world(world),
	m_inputState(inputState),
	m_depthScale(1.0f) {
}

prDepthScaleUpdater::~prDepthScaleUpdater() {
}

prObjectDefs::GroupType prDepthScaleUpdater::GetType() const {
	return prObjectDefs::GT_UpdateAble;
}

void prDepthScaleUpdater::Update(float in_delta) {
	if (m_inputState.m_depthScale == 0.0f)
		return;
	m_depthScale = prMathf::Clamp(
			m_depthScale + in_delta * m_inputState.m_depthScale * DEPTH_SCALE_SPEED,
			DEPTH_SCALE_MIN, DEPTH_SCALE_MAX);

	prMaterialResource::tSP mat =
			GetMaterialResourceManager()->RequestMaterial("worldcell");
	prAssert(mat);
	//mat->GetParamFloat("u_depth")->SetValue(-m_world.max_depth() * m_depthScale);
}

