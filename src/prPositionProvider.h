#ifndef prPositionProvider_INCLUDED
#define prPositionProvider_INCLUDED

#include "Core/Scene/Object/prComponent.h"
#include "Core/Utils/Struct/prDataArray.h"
#include "Core/Math/prVector3.h"

class prPositionProvider : public prComponent {
public:
	prPositionProvider();
	virtual ~prPositionProvider();

	typedef prSmartPointerT<prPositionProvider> tSP;

	virtual prObjectDefs::GroupType GetType() const;

	prVector3f GetPosition() const;
	prVector3f GetDir() const;

	virtual void Update(float in_delta);
protected:
	virtual prVector3f ReadPosition() = 0;

private:
	struct Point {
		prVector3f m_position;
		float m_time;
	};

	prDataArrayT<Point> m_history;
	float m_time;
	float m_dirTime;
};

#endif //prPositionProvider_INCLUDED

