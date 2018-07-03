#include "prCoreDefs.h"
#include "prNavSplinePositionProvider.h"

prNavSplinePositionProvider::prNavSplinePositionProvider(
		const prNavSpline::tSP& spline)
  : m_spline(spline),
	m_trackTime(0.0f),
	m_scale(1.0f),
	m_pointInd(0) {
}

prNavSplinePositionProvider::~prNavSplinePositionProvider() {
}

void prNavSplinePositionProvider::SetTime(float time) {
	m_trackTime = time;
	m_pointInd = 0;
}

void prNavSplinePositionProvider::SetScale(float scale) {
	m_scale = scale;
}

void prNavSplinePositionProvider::Update(float in_delta) {
	m_trackTime += in_delta * m_scale;
	if (m_trackTime >= m_spline->times().back()) {
		m_trackTime = 0.0f;
		m_pointInd = 0;
	}
	prPositionProvider::Update(in_delta);
}

prVector3f prNavSplinePositionProvider::ReadPosition() {
	while (m_spline->times()[m_pointInd + 1] <= m_trackTime)
		++m_pointInd;
	prAssert(m_spline->times()[m_pointInd] <= m_trackTime);
	prAssert(m_spline->times()[m_pointInd +1] > m_trackTime);
	float t = (m_trackTime - m_spline->times()[m_pointInd]) /
			(m_spline->times()[m_pointInd +1] - m_spline->times()[m_pointInd]);
	const prVector3f pt1 = m_spline->geom_spline()->GetPoint(m_pointInd).m_point;
	const prVector3f pt2 = m_spline->geom_spline()->GetPoint(m_pointInd + 1).m_point;
	return pt1 * (1.0f - t) + pt2 * t;
}

