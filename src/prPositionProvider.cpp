#include "prCoreDefs.h"
#include "prPositionProvider.h"

prPositionProvider::prPositionProvider()
  : m_time(0.0f), m_dirTime(2.0f) {
}

prPositionProvider::~prPositionProvider() {
}

prObjectDefs::GroupType prPositionProvider::GetType() const {
	return prObjectDefs::GT_UpdateAble;
}

void prPositionProvider::Update(float in_delta) {
	m_time += in_delta;
	Point pt;
	pt.m_time = m_time;
	pt.m_position = ReadPosition();
	m_history.insert(0, pt);
	while (m_history.back().m_time < (m_time - m_dirTime)) {
		m_history.pop_back();
	}
}

prVector3f prPositionProvider::GetPosition() const {
	return m_history.empty() ? prVector3f::ZERO : m_history.front().m_position;
}

prVector3f prPositionProvider::GetDir() const {
	prVector3f dir = m_history.size() < 2 ?
			prVector3f::AXIS_Z : m_history.front().m_position - m_history.back().m_position;
	dir.Normalize();
	return dir;
}


