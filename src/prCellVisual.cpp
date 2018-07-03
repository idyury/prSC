#include "prCoreDefs.h"
#include "prCellVisual.h"
#include "Core/Geometric/Mesh/Topology/prTriangulationMeshOp.h"
#include "Core/Geometric/Mesh/Info/prMeshDimension.h"
#include "Core/Stream/prStreamFactory.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/Geometric/Mesh/Misc/prCalculateNormalSmoothMeshOp.h"
#include "Core/Geometric/Mesh/Misc/prMeshEdgeMapper.h"

namespace {

struct Sample {
	float mDepth;
	unsigned short mBottom;
	unsigned char mType;
	unsigned char mDummy;
};

}
//---------------------------------------------------------------------------//
prCellVisual::prCellVisual(int x, int z, int dimension, int step)
    : mX(x),
	  mZ(z),
	  mDimension(dimension),
	  mStep(step),
	  m_center(mX + mDimension * 0.5, mZ + mDimension * 0.5),
	  m_update(0),
	  m_trianglesMaterial("triview"), m_linesMaterial("lineview") {
}
//---------------------------------------------------------------------------//
prCellVisual::~prCellVisual() {
}
//---------------------------------------------------------------------------//
prStringID prCellVisual::GetTrianglesMaterialID() {
    return m_trianglesMaterial;
}

prStringID prCellVisual::GetLinesMaterialID() {
	return m_linesMaterial;
}
//---------------------------------------------------------------------------//
bool prCellVisual::IsInside(double x, double z) const {
	if (x < mX) return false;
	if (x > mX + mDimension) return false;
	if (z < mZ) return false;
	if (z > mZ + mDimension) return false;
	return true;
}
//---------------------------------------------------------------------------//
void prCellVisual::UpdateFromCameraPosition(double x, double z) {
	prMatrix4f matrix = prMatrix4f::IDENTITY;
	matrix.SetPosition(prVector3f((float)(m_center.m_x - x), 0.0f, (float)(m_center.m_y - z)));

	m_update++;
	SetTransform(matrix);
	m_update--;
}
//---------------------------------------------------------------------------//
void prCellVisual::MarkDirty() {
	if (m_update) {
		return;
	}

	prChangeViewPrimitive::MarkDirty();
}
//---------------------------------------------------------------------------//
bool prCellVisual::LoadMesh(prMeshf& mesh) {
	char file_name[256];
	sprintf(file_name, "/home/khmel/Downloads/ac/sc/data/%d_%d.data", mX, mZ);
	prInputStream::tSP stream = GetStreamFactory()->CreateFileInputStream(prStringConv::ToString(file_name));
	if (!stream) {
		return false;
	}

	const int cnt = mDimension / mStep + 1;

	prDataArrayT<Sample> samples;
	samples.resize(cnt * cnt);

	const size_t to_read = cnt * cnt * sizeof(Sample);
	if (prStreamUtils::ReadData(stream, &samples.front(), to_read) != to_read) {
		prAssert(false);
		return false;
	}
	prAssert(!stream->IsAvailable());

	prMeshf::tPoints3& points = mesh.GetVertices();
	prMeshf::tColors& colors = mesh.GetColors();

	points.resize(cnt * cnt);
	colors.resize(cnt * cnt);

	prVector3f* p = &points.front();
	unsigned int* c = &colors.front();
	const Sample* s = &samples.front();

	const float div = 1.0f / (cnt - 1);
	for (int y = 0; y < cnt; y++) {
		for (int x = 0; x < cnt; x++) {
			p->Set((float)mDimension * (x * div - 0.5f),
				   -s->mDepth,
				   (float)mDimension * (y * div - 0.5f));
			switch (s->mType) {
			case 1:
			case 11:
			case 51:
				*c = 0xffb3d7ef;
				break;
			case 2:
			case 12:
			case 52:
				*c = 0xff92b4df;
				break;
			case 3:
			case 13:
			case 53:
				*c = 0xff4b63b5;
				break;
			case 4:
			case 14:
			case 54:
				*c = 0xffdccf2c;
				break;
			case 5:
			case 15:
			case 55:
				*c = 0xff9755a1;
				break;
			default:
				*c = 0xffff0000;
			}
			++p;
			++c;
			++s;
		}
	}

	prMeshEdgeMapperf builder(mesh);
	for (int y = 0; y < cnt - 1; ++y) {
		for (int x = 0; x < cnt - 1; ++x) {
			int xb = cnt * y + x;
			builder.AddPolygon(xb + cnt, xb + cnt + 1, xb + 1, xb);
		}
	}

	return true;
}
//---------------------------------------------------------------------------//
prPrimitiveBuffer* prCellVisual::Calculate() {
	prMeshf mesh;
	if (!LoadMesh(mesh)) {
		return NULL;
	}

	prCalculateNormalSmoothMeshOpf().Apply(mesh);
	prTriangulationMeshOpf().Apply(mesh);

	prDataArrayT<unsigned int> l_indices;

    const prRenderBaseBuffer::BufferType btype =
        CheckComponentFlag(PRIM_FLAG_HW_BUFFER) ?
            prRenderBaseBuffer::BT_Hardware : prRenderBaseBuffer::BT_Software;

    unsigned int vcnt = (unsigned int)mesh.GetVertices().size();
	unsigned int tcnt = (unsigned int)mesh.GetPolygons().size();
	unsigned int ecnt = (unsigned int)mesh.GetEdges().size();

	prPrimitiveBuffer* buffer = new prPrimitiveBuffer(
        vcnt, tcnt * 3, ecnt * 2, 0,
		prPrimitiveBuffer::sm_vertex_decl_normal, btype);

	prDataArrayT<unsigned int> indices;

	prDataArrayT<unsigned int> tindices;
	for(unsigned int t = 0; t < tcnt; ++t) {
		mesh.GetPolygonVertices(t, tindices);
		indices.push_back(tindices[0]);
		indices.push_back(tindices[1]);
		indices.push_back(tindices[2]);
	}

	for (unsigned int i = 0; i < ecnt; ++i) {
		unsigned int indexA, indexB;
		mesh.GetEdgeVertices(i, indexA, indexB );
		indices.push_back(indexA);
		indices.push_back(indexB);
	}

	prDimension dim;
	prMeshDimensionf::Calculate(mesh, dim);

	void* pV = buffer->StartVerticesWrite();

	prRenderData::DataGetterT<prVector3f> setter(pV, buffer->GetVertexDecl(), prRenderData::ItemType_Vertex);
	prRenderData::DataGetterT<unsigned int> setterClr(pV, buffer->GetVertexDecl(), prRenderData::ItemType_Color);
	prRenderData::DataGetterT<prVector3f> setterN(pV, buffer->GetVertexDecl(), prRenderData::ItemType_Normal);
	
	const prVector3f* pVertex = &mesh.GetVertices().front();
	const unsigned int* pColor = &mesh.GetColors().front();
	const prVector3f* pNormal = &mesh.GetNormals().front();

	while(vcnt) {
		*setter = *pVertex;
		*setterClr = *pColor;
		*setterN = *pNormal;
		++pVertex; ++setter;
		++pColor; ++setterClr;
		++pNormal; ++setterN;
		--vcnt;
	}
	buffer->EndVerticesWrite();
	buffer->SetIndices(indices);

	SetUpDIPs(buffer, dim);

	return buffer;
}
//---------------------------------------------------------------------------//
