#ifndef prBoatUpdater_INCLUDED
#define prBoatUpdater_INCLUDED

#include "Core/Scene/Object/prComponent.h"
#include "Core/Scene/Primitives/prMeshPrimitive.h"
#include "prPositionProvider.h"

class prBoatUpdater : public prComponent {
public:
	prBoatUpdater(const prMeshPrimitive::tSP& boat,
				  const prPositionProvider::tSP& provider);
	virtual ~prBoatUpdater();

	typedef prSmartPointerT<prBoatUpdater> tSP;

	virtual prObjectDefs::GroupType GetType() const;
protected:
	virtual void Update(float in_delta);

private:
	prMeshPrimitive::tSP m_boat;
	prPositionProvider::tSP m_provider;
};

#endif //prBoatUpdater_INCLUDED

