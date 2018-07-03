#ifndef prCameraUpdater_INCLUDED
#define prCameraUpdater_INCLUDED

#include "Core/Scene/Object/prComponent.h"
#include "Core/Scene/Camera/prComponentCamera.h"
#include "prPositionProvider.h"
#include "prWorld.h"
#include "prInputState.h"

class prCameraUpdater : public prComponent {
public:
	prCameraUpdater(const prWorld::tSP& world,
				    const prInputState& inputState,
					const prComponentCamera::tSP& camera,
					const prPositionProvider::tSP& provider);
	virtual ~prCameraUpdater();

	typedef prSmartPointerT<prCameraUpdater> tSP;

	virtual prObjectDefs::GroupType GetType() const;

protected:
	virtual void Update(float in_delta);

private:
	prWorld::tSP m_world;
	const prInputState& m_inputState;
	prComponentCamera::tSP m_camera;
	prPositionProvider::tSP m_provider;

	bool m_freeDirection;
	float m_height;
	float m_hAngle;
	float m_vAngle;
	float m_distance;
};

#endif //prCameraUpdater_INCLUDED

