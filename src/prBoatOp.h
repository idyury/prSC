#ifndef prBoatOp_INCLUDE_
#define prBoatOp_INCLUDE_

#include "Core/Geometric/Mesh/prMeshOp.h"
#include "Core/Geometric/Mesh/Primitive/prCylinderMeshOp.h"
#include "Core/Geometric/Mesh/Transform/prOffsetMeshOp.h"
#include "Core/Geometric/Mesh/Transform/prScaleMeshOp.h"
#include "Core/Geometric/Mesh/Topology/prMergeMeshOp.h"

template <typename T>
struct prBoatOpT : public prMeshOpT<T> {
public:
  typedef prSmartPointerT<prBoatOpT<T>> tSP;

  prBoatOpT() {
  }

  ~prBoatOpT() {
  }

  void Apply(prMeshT<T>& inout_mesh) {
	  prCylinderMeshOpT<T> op_cyl;
	  op_cyl.SetLevel(4);
	  op_cyl.Apply(inout_mesh);

	  prScaleMeshOpT<T>(
			  prVector3T<T>(
					  (T)(1.0 / 2.0 * prCylinderMeshOpT<T>::R),
					  (T)(0.5 / 2.0 * prCylinderMeshOpT<T>::R),
					  (T)(5.0 / 2.0 * prCylinderMeshOpT<T>::R))).Apply(inout_mesh);

	  typename prMeshT<T>::tSP vert = new prMeshT<T>();
	  op_cyl.Apply(*vert);
	  prOffsetMeshOpT<T>(prVector3T<T>(0, -prCylinderMeshOpT<T>::H, 0)).Apply(*vert);
	  prScaleMeshOpT<T>(prVector3T<T>(
			  (T)(0.25 / 2.0 * prCylinderMeshOpT<T>::R),
			  100,
			  (T)(0.25 / 2.0 * prCylinderMeshOpT<T>::R))).Apply(*vert);

	  prMergeMeshOpT<T>(vert).Apply(inout_mesh);
  }

private:
};

typedef prBoatOpT<float> prBoatOpf;
typedef prBoatOpT<double> prBoatOpd;

#endif /* prBoatOp_INCLUDE_ */
