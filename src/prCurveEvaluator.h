#ifndef prCurveEvaluator_INCLUDED
#define prCurveEvaluator_INCLUDED

#include "Core/Utils/Struct/prDataArray.h"
#include "Core/Utils/Struct/prObjectArray.h"
#include "Core/Geometric/Math/prBezierCurve.h"

template <typename T>
struct prCurveEvaluatorT : public prSmartBase
{
public:
  typedef prSmartPointerT< prCurveEvaluatorT<T> > tSP;

  prCurveEvaluatorT() {
  }

  ~prCurveEvaluatorT() {
  }

  void Add(const prDataArrayT< prVector2T<T> >& data, T from, T to) {
	  if (!m_curves.empty()) {
		  prAssert(m_curves.back().m_from < from);
		  prAssert(m_curves.back().m_to < to);
		  prAssert(m_curves.back().m_to >= from);
	  }

	  Curve c;
	  c.m_from = from;
	  c.m_to = to;
	  c.m_data = data;
	  m_curves.push_back(c);
  }

  bool IsValid() const {
	  if (m_curves.empty()) {
		  return false;
	  }
	  return true;
  }

  prVector2T<T> EvaluatePoint(T t) {
	  prAssert(m_curves.size() == 1);
	  return EvaluatePoint(t, m_curves.front().m_data);
  }

  prVector2T<T> EvaluatePoint(T t1, T t2) {
	  prAssert(m_curves.size() >= 1);
	  for (size_t i = 0; i < m_curves.size(); ++i) {
		  const Curve& c1 = m_curves[i];
		  if (c1.m_to < t1)
			  continue;
		  prAssert(c1.m_from <= t1);
		  if (i == m_curves.size() - 1 ||
		      m_curves[i + 1].m_from >= t1) {
			  return EvaluatePoint(t2, c1.m_data);
		  }
		  const Curve& c2 = m_curves[i + 1];
		  prAssert(c2.m_from < t1);
		  prAssert(c2.m_to > t1);
		  const T d = c1.m_to - c2.m_from;
		  prAssert(d > 0.);
		  const T t = (t1 - c2.m_from) / d;;
		  prAssert(t >= 0.);
		  prAssert(t <= 1.);
		  return EvaluatePoint(t2, c1.m_data) * (1. - t) +
		      EvaluatePoint(t2, c2.m_data) * t;
	  }
	  prAssert(false);
	  return prVector2T<T>::ZERO;
  }
private:
  struct Curve {
	  T m_from;
	  T m_to;
	  prDataArrayT< prVector2T<T> > m_data;
  };

  prObjectArrayT< Curve > m_curves;

  static prVector2T<T>
  EvaluatePoint(T t, const prDataArrayT< prVector2T<T> >& points) {
    const size_t segment_cnt = (points.size() -1) / 3;
    t *= (T)segment_cnt;
    int index = (int)t;
    if (index == segment_cnt)
      return points.back();
    t = t - (T)index;
    prAssert(t >= 0.);
    prAssert(t <= 1.);
    index *= 3;
    return prBezierCurveT< prVector2T<T>, T >::GetPoint(points[index],
                                                        points[index + 1],
                                                        points[index + 2],
                                                        points[index + 3],
                                                        t);
  }
};
//--------------------------------------------------------------------------------------------------------//

typedef prCurveEvaluatorT<float> prCurveEvaluatorf;
typedef prCurveEvaluatorT<double> prCurveEvaluatord;

#endif /* prCurveEvaluator_INCLUDED */
