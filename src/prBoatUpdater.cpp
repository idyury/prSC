#include "prCoreDefs.h"

#include "prBoatUpdater.h"

prBoatUpdater::prBoatUpdater(const prMeshPrimitive::tSP& boat,
		                     const prPositionProvider::tSP& provider)
  : m_boat(boat),
	m_provider(provider) {
}

prBoatUpdater::~prBoatUpdater() {
}

prObjectDefs::GroupType prBoatUpdater::GetType() const {
	return prObjectDefs::GT_UpdateAble;
}

void prBoatUpdater::Update(float in_delta) {
	prVector3f lat;
	prVector3f dir  = m_provider->GetDir();
	lat.FromCrossProduct(prVector3f::AXIS_Y, dir);
	prMatrix4f transform = prMatrix4f::IDENTITY;
	transform.SetDirection(dir);
	transform.SetLateral(lat);
	transform.SetPosition(m_provider->GetPosition());
	m_boat->SetTransform(transform);
}

