#ifndef prCellVisual_INCLUDED
#define prCellVisual_INCLUDED

#include "Core/Scene/Primitives/prChangeViewPrimitive.h"
#include "Core/Geometric/Mesh/prMeshOp.h"

// Represent Mesh 
class prCellVisual : public prChangeViewPrimitive
{
public:
	prCellVisual(int x, int z, int dimension, int step);
	virtual ~prCellVisual();
	
	typedef prSmartPointerT<prCellVisual> tSP;
	
	void UpdateFromCameraPosition(double x, double z);
	bool IsInside(double x, double z) const;

	virtual prPrimitiveBuffer* Calculate();

	virtual prStringID GetTrianglesMaterialID();
	virtual prStringID GetLinesMaterialID();

	virtual void MarkDirty( );
private:
	bool LoadMesh(prMeshf& mesh);

	int mX;
	int mZ;
	int mDimension;
	int mStep;
	prVector2d m_center;
	int m_update;

	prStringID m_trianglesMaterial;
	prStringID m_linesMaterial;
};

#endif //prCellVisual_INCLUDED

