#ifndef prDepthScaleUpdater_INCLUDED
#define prDepthScaleUpdater_INCLUDED

#include "Core/Scene/Object/prComponent.h"
#include "prInputState.h"
#include "prWorld.h"

class prDepthScaleUpdater : public prComponent {
public:
	prDepthScaleUpdater(const prWorld& world, const prInputState& inputState);
	virtual ~prDepthScaleUpdater();

	typedef prSmartPointerT<prDepthScaleUpdater> tSP;

	virtual prObjectDefs::GroupType GetType() const;
protected:
	virtual void Update(float in_delta);

private:
	const prWorld& m_world;
	const prInputState& m_inputState;
	float m_depthScale;
};

#endif //prDepthScaleUpdater_INCLUDED

