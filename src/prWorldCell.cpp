#include "prCoreDefs.h"
#include "prWorldCell.h"

#include "prWorld.h"
#include "Core/Stream/prStreamFactory.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/System/FileSystem/prFileSystem.h"
#include "Core/Render/Data/Material/prMaterialResourceManager.h"
#include "Core/Render/Data/Texture/prTextureResourceManager.h"

namespace {

struct RenderPoint {
	unsigned char x;
	unsigned char z;
	unsigned char light;
	unsigned char type;
	unsigned short depth;
	unsigned short reserved;
};

prRenderData::VertexDecl BuldPrimitiveVertexDecl() {
	prRenderData::VertexDeclBuilder builder;
	builder.AddMainSlot( prRenderData::ItemType_Channel1, prRenderData::DataType_Vec4UC );
	builder.AddMainSlot( prRenderData::ItemType_Channel2, prRenderData::DataType_Vec2US );
    return builder.GetResult();
}

prRenderData::VertexDecl sm_vertex_decl(BuldPrimitiveVertexDecl());
}

//---------------------------------------------------------------------------//
prWorldCell::tSP prWorldCell::Load(prWorld& world, int x, int y) {
	prWorldCell::tSP cell = new prWorldCell(world, x, y);

	if (!cell->Load())
		return NULL;

	return cell;
}

prWorldCell::prWorldCell(prWorld& world, int x, int y)
    : world_(world), x_(x), y_(y) {
}

prWorldCell::~prWorldCell() {
}

bool prWorldCell::Load() {
	char mat_name[32];
	sprintf(mat_name, "/%.6d", x_ + y_ * world_.x_block_cnt());


	prString mat_path = world_.path();
	mat_path += prStringConv::ToString(mat_name);
	prString cell_path = mat_path;
	cell_path += _T(".mdl");

	prStringID matId = (const char*)prStringConv::ToStringUTF8(mat_path);

	prInputStream::tSP input = GetFileSystem()->OpenFile(cell_path);
	if (!input)
		return false;

	prDataArrayT<RenderPoint> vertices;
	if (!prStreamUtils::ReadArray(input, vertices)) {
		prAssert(false);
		return false;
	}

	buffer_ = new prPrimitiveBuffer(
			vertices.size(), 0, 0, 0,
			sm_vertex_decl,
			prRenderBaseBuffer::BT_Software);

	memcpy(buffer_->StartVerticesWrite(),
			&vertices.front(),
			vertices.size() * sizeof(RenderPoint));
	buffer_->EndVerticesWrite();

	const prDimension dim(
			prVector3f(0.0f, -world_.max_depth(), 0.0f),
			prVector3f(world_.dimension(), 0.0f, world_.dimension()));
	buffer_->SetDimension(dim);

	prAssert(!input->IsAvailable());

	prMaterialResource::tSP baseMat = GetMaterialResourceManager()->RequestMaterial("worldcell");
	prAssert(baseMat != NULL);
	prMaterialResource::tSP mat = GetMaterialResourceManager()->CloneMaterial(matId, baseMat);
	mat->SetTexture(prMaterialResource::TexChannel_1,
			GetTextureResourceManager()->RequestTexture(matId));
	prRenderLOD& lod = buffer_->AllocateLOD();
	prRenderDIP& dip = lod.AllocateDIP();
	dip.m_materialID = matId;
	dip.m_topology = prRenderDIP::TOPO_TriList;
	dip.m_elementCnt = 3 * world_.cell_triangle_count();
	dip.m_center = dim.GetSphere().m_pos;
	dip.m_elementOffset = 0;
	dip.m_pVertexBuffer = &buffer_->GetVertexBuffer();
	dip.m_pIndexBuffer = world_.cell_indices();

	prMatrix4f transform = prMatrix4f::IDENTITY;
	transform.SetPosition(prVector3f(
			x() * world_.dimension(), 0.0f, y() * world_.dimension()));
	SetTransform(transform);

	SetRenderModel(buffer_);

	return true;
}

float prWorldCell::GetDepth(int inx, int iny) const {
	prAssert(inx >= 0 && inx <= world_.in_block_cnt());
	prAssert(iny >= 0 && iny <= world_.in_block_cnt());

	prRenderBaseBuffer::MapHandle out_handle;
	buffer_->GetVertexBuffer().MapAllData(out_handle);
	const RenderPoint* rp = (const RenderPoint*)out_handle.m_pBuffer;
	const unsigned short depth = rp[inx + iny * (world_.in_block_cnt() + 1)].depth;
	buffer_->GetVertexBuffer().UnmapData(out_handle);
	return world_.max_depth() * depth / 65535.0f;
}
