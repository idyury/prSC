#ifndef prNavSpline_INCLUDED
#define prNavSpline_INCLUDED

#include "Game/Utils/gaSpline.h"
#include "Core/Stream/prInputStream.h"
#include "Core/Utils/Struct/prDataArray.h"
#include "Core/Geometric/Mesh/prMesh.h"

class prWorld;

// Represent Mesh 
class prNavSpline : public prSmartBase
{
public:
	virtual ~prNavSpline();

	typedef prSmartPointerT<prNavSpline> tSP;

	static tSP Load(const prWorld& world, const prChar* data_path);

	prMeshf::tSP CreateMesh();

	const prWorld& world() const { return world_; }
	const gaSpline* geom_spline() const { return m_geomSpline; }
	const prDataArrayT<float>& times() const { return m_times; }
private:
	prNavSpline(const prWorld& world);


	const prWorld& world_;
	gaSpline::tSP m_geomSpline;
	prDataArrayT<float> m_times;
	prDataArrayT<float> m_accuracies;
};

#endif //prNavSpline_INCLUDED

