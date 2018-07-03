#ifndef prWorld_INCLUDED
#define prWorld_INCLUDED

#include "Core/Utils/prSmartPointer.h"
#include "Core/Utils/String/prChar.h"
#include "Core/Utils/String/prString.h"
#include "Core/Utils/Struct/prObjectArray.h"
#include "prWorldCell.h"
#include "Core/Render/Data/Buffer/prRenderSoftwareIndexBuffer.h"
#include "Core/System/FileSystem/prVirtualFileSystem.h"

class prScene;

// Represent Mesh 
class prWorld : public prSmartBase
{
public:
	virtual ~prWorld();

	typedef prSmartPointerT<prWorld> tSP;
	
	static tSP Load(const prChar* name);

	void Attach(prScene& scene);
	void Detach(prScene& scene);

	bool Raycast(float x, float y, float& depth) const;

	prVector3f FromGlobalPos(const prVector3f& global_pos) const;

	const prString& path() const { return path_; }

	int xmin() const { return xmin_; }
    int ymax() const { return ymax_; }
    int dimension() const { return dimension_; }
    int step() const { return step_; }
    int in_block_cnt() const { return in_block_cnt_; }
    int x_block_cnt() const { return x_block_cnt_; }
    int y_block_cnt() const { return y_block_cnt_; }
    float max_depth() const { return max_depth_; }

    unsigned int cell_triangle_count() const { return 2 * in_block_cnt_ * in_block_cnt_; }

    prRenderSoftwareIndexBuffer* cell_indices() {
		return cell_indices_->indices_;
	}
private:
    class IndicesKeeper : public prSmartBase {
      public:
        IndicesKeeper(prRenderSoftwareIndexBuffer* indices) : indices_(indices) {
        }
        ~IndicesKeeper() {
        	delete indices_;
        }

        typedef prSmartPointerT<IndicesKeeper> tSP;

        prRenderSoftwareIndexBuffer* indices_;
    };

	prWorld(const prChar* path, const prVirtualFileSystem::tSP& virtualFS);

	bool Load();
	void ApplyMaterials();

	const prString path_;
	prVirtualFileSystem::tSP m_virtualFS;

    int xmin_;
    int ymax_;
    int dimension_;
    int step_;
    int in_block_cnt_;
    int x_block_cnt_;
    int y_block_cnt_;
    float max_depth_;
    IndicesKeeper::tSP cell_indices_;

    prObjectArrayT<prWorldCell::tSP> cells_;
};

#endif //prWorld_INCLUDED

