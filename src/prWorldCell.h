#ifndef prWorldCell_INCLUDED
#define prWorldCell_INCLUDED

#include "Core/Scene/Visual/prComponent3D.h"
#include "Core/Scene/Primitives/prPrimitiveBuffer.h"

class prWorld;

class prWorldCell : public prComponent3D {
public:
	virtual ~prWorldCell();
	
	typedef prSmartPointerT<prWorldCell> tSP;
	
	static tSP Load(prWorld& world, int x, int y);

	float GetDepth(int inx, int iny) const;

	int x() const { return x_; }
	int y() const { return y_; }
private:
	prWorldCell(prWorld& world, int x, int y);

	bool Load();

	prWorld& world_;
	const int x_;
	const int y_;
	prPrimitiveBuffer::tSP buffer_;
};

#endif //prWorldCell_INCLUDED

