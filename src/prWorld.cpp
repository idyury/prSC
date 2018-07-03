#include "prCoreDefs.h"

#include "prWorld.h"
#include "prWorldCell.h"
#include "Core/Stream/prStreamFactory.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/Scene/prScene.h"
#include "Core/Render/Data/Material/prMaterialResourceManager.h"
#include "Core/System/FileSystem/prFileSystem.h"

namespace {
}

//---------------------------------------------------------------------------//
// static
prWorld::tSP prWorld::Load(const prChar* name) {
	prString vsPath = GetSystem()->GetRootPath();
	vsPath += _T("/world/");
	vsPath += name;
	vsPath += _T(".vfs");
	prRAInputStream::tSP stream = GetStreamFactory()->CreateRAFileInputStream(vsPath);
	if (stream == NULL) {
		prAssert(false);
		return NULL;
	}

	prVirtualFileSystem::tSP vs = new prVirtualFileSystem();
	if (!vs->Open(stream)) {
		return NULL;
	}

    prString path = _T("world/");
    path += name;
	tSP world = new prWorld(path, vs);
	if (!world->Load()) {
		prAssert(false);
		return NULL;
	}

	return world;
}

prWorld::prWorld(const prChar* path,
		         const prVirtualFileSystem::tSP& virtualFS)
    : path_(path),
	  m_virtualFS(virtualFS) {
	GetFileSystem()->Mount(m_virtualFS);
}

prWorld::~prWorld() {
	GetFileSystem()->Unmount(m_virtualFS);
}

bool prWorld::Load() {
	prString layout_path = path();
	layout_path += _T("/layout");

    prInputStream::tSP layout = GetFileSystem()->OpenFile(layout_path);
    if (!layout)
    	return false;


    if (!prStreamUtils::ReadStruct(layout, xmin_) ||
    	!prStreamUtils::ReadStruct(layout, ymax_) ||
    	!prStreamUtils::ReadStruct(layout, dimension_) ||
    	!prStreamUtils::ReadStruct(layout, step_) ||
    	!prStreamUtils::ReadStruct(layout, in_block_cnt_) ||
    	!prStreamUtils::ReadStruct(layout, x_block_cnt_) ||
    	!prStreamUtils::ReadStruct(layout, y_block_cnt_) ||
    	!prStreamUtils::ReadStruct(layout, max_depth_)) {
    	return false;
    }
    layout = NULL;

	prString indices_path = path();
	indices_path += _T("/indices");
	prUShortArray cell_indices;
    prInputStream::tSP input_indices = GetFileSystem()->OpenFile(indices_path);
    if (input_indices == NULL ||
       !prStreamUtils::ReadArray(input_indices, cell_indices)) {
    	prAssert(false);
    	return false;
    }
    prAssert(cell_indices.size() == 3 * cell_triangle_count());

    const size_t cell_indices_size = cell_indices.size() * sizeof(unsigned short);
    cell_indices_ = new IndicesKeeper(
    		new prRenderSoftwareIndexBuffer(
    				prRenderData::IndexType_USHORT,
                    cell_indices_size));
    prRenderIndexBuffer::MapHandle map_handle;
    cell_indices_->indices_->MapAllData(map_handle);
    memcpy(map_handle.m_pBuffer, &cell_indices.front(), cell_indices_size);
    cell_indices_->indices_->UnmapData(map_handle);

    ApplyMaterials();
    cells_.resize(x_block_cnt() * y_block_cnt());
    for (int y = 0; y < y_block_cnt(); ++y) {
        for (int x = 0; x < x_block_cnt(); ++x) {
        	cells_[x + y * x_block_cnt()] = prWorldCell::Load(*this, x, y);
        }
    }

    return true;
}

void prWorld::ApplyMaterials() {
	prMaterialResource::tSP mat =
			GetMaterialResourceManager()->RequestMaterial("worldcell");
	prAssert(mat);
	mat->GetParamV4("u_matambient")->SetValue(
			prVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	mat->GetParamV4("u_matdiffuse")->SetValue(
			prVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	mat->GetParamV4("u_matspecular")->SetValue(
			prVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	mat->GetParamFloat("u_matshininess")->SetValue(64.0f);
	mat->GetParamFloat("u_depth")->SetValue(-max_depth());

	const float step = (float)dimension() * 255.0f / in_block_cnt();
	mat->GetParamFloat("u_step")->SetValue(step);
}

void prWorld::Attach(prScene& scene) {
	for (size_t i = 0; i < cells_.size(); ++i) {
		if (cells_[i]) {
			scene.AddComponent(cells_[i]);
		}
	}
}

void prWorld::Detach(prScene& scene) {
	for (size_t i = 0; i < cells_.size(); ++i) {
		if (cells_[i]) {
			scene.RemoveComponent(cells_[i]);
		}
	}
}

bool prWorld::Raycast(float x, float y, float& depth) const {
	if (x < 0 || y < 0)
		return false;
	if (x >= dimension() * x_block_cnt())
		return false;
	if (y >= dimension() * y_block_cnt())
		return false;

	const int xcell = (int)(x / dimension());
	x = x - xcell * dimension();
	prAssert(x >= 0 && x < dimension());
	const int ycell = (int)(y / dimension());
	y = y - ycell * dimension();
	prAssert(y >= 0 && y < dimension());

	const int cell_index = xcell + ycell * x_block_cnt_;
	prWorldCell::tSP cell = cells_[cell_index];
	if (!cell)
		return false;

	const int xquad = (int)(x / step());
	const int yquad = (int)(y / step());
	x = (x - xquad * step()) / step();
	y = (y - yquad * step()) / step();

    const float d1 = cell->GetDepth(xquad, yquad);
    const float d2 = cell->GetDepth(xquad + 1, yquad);
    const float d3 = cell->GetDepth(xquad, yquad + 1);
    const float d4 = cell->GetDepth(xquad + 1, yquad + 1);

    const float dd1 = d1 * (1.0f - x) + d2 * x;
    const float dd2 = d3 * (1.0f - x) + d4 * x;
    depth = dd1 * (1.0f - y) + dd2 * y;
	return true;
}

prVector3f prWorld::FromGlobalPos(const prVector3f& global_pos) const {
	return prVector3f(global_pos.m_x - xmin(), global_pos.m_y, ymax() - global_pos.m_z);
}
