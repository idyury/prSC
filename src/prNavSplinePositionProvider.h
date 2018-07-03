#ifndef prNavSplinePositionProvider_INCLUDED
#define prNavSplinePositionProvider_INCLUDED

#include "prPositionProvider.h"
#include "prNavSpline.h"

// Represent Mesh 
class prNavSplinePositionProvider : public prPositionProvider {
public:
	prNavSplinePositionProvider(const prNavSpline::tSP& spline);
	virtual ~prNavSplinePositionProvider();

	typedef prSmartPointerT<prNavSplinePositionProvider> tSP;

	void SetTime(float time);
	void SetScale(float scale);

protected:
	virtual void Update(float in_delta);
	virtual prVector3f ReadPosition();

private:
	prNavSpline::tSP m_spline;
	float m_trackTime;
	float m_scale;
	size_t m_pointInd;
};

#endif //prNavSplineCamera_INCLUDED

