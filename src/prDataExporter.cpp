#include "prCoreDefs.h"
#include "prCellVisual.h"
#include "Core/Geometric/Mesh/Topology/prTriangulationMeshOp.h"
#include "Core/Geometric/Mesh/Info/prMeshDimension.h"
#include "Core/Stream/prStreamFactory.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/Geometric/Mesh/Misc/prCalculateNormalSmoothMeshOp.h"
#include "Core/Geometric/Mesh/Misc/prMeshEdgeMapper.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/Geometric/Mesh/IO/prMeshIO.h"
#include "Core/Stream/prStreamFactory.h"
#include "Utils/Image/prImageWriter.h"
#include <algorithm>

namespace {

unsigned char TypeToColor(int type) {
	switch (type) {
	case 1:
	case 11:
	case 51:
		return 1;
	case 2:
	case 12:
	case 52:
		return 2;
	case 3:
	case 13:
	case 53:
		return 3;
	case 4:
	case 14:
	case 54:
		return 4;
	case 5:
	case 15:
	case 55:
		return 5;
	default:
		return 7;
	}
}


struct Sample {
	int mX;
	int mY;
	float mDepth;
	int mBottom;
	int mType;
};


struct RenderPoint {
	unsigned char x;
	unsigned char z;
	unsigned char light;
	unsigned char type;
	unsigned short depth;
	unsigned short reserved;
};

struct LightPoint {
	float 	m_total;
	float   m_min;
	int 	m_cnt;
};

typedef prDataArrayT<RenderPoint> tRenderPoints;

#define LOD_CNT 4

}

bool ExportGeoData(const prChar* dir, int dimension, int step) {
    prString data_path = dir;
    data_path += _T("/data.txt");

	prStringASCII data;
    if (!prStreamUtils::ReadStringFile(data_path, data)) {
		printf("Error! Failed to read data file\n");
		return false;
	}
	
	prStringTokenizerA tokenizer(data, "\n\r");
	prStringTokenizerA::Token line;
	prDataArrayT<Sample> samples;
	while (tokenizer.NextToken(line)) {
		prStringTokenizerA tk(line, " ");
		prStringTokenizerA::Token en;
		Sample s;
		if (!tk.NextToken(en) || !prStringUtils::Parse(en, s.mX) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, s.mY) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, s.mDepth) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, s.mBottom) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, s.mType)) {
			printf("Error! Failed to parse: %s\n", (const char*)line);
			return NULL;
		}
		samples.push_back(s);
	}

	int xmin = samples[0].mX;
	int xmax = samples[0].mX;
	int ymin = samples[0].mY;
	int ymax = samples[0].mY;

	for (size_t i = 0; i < samples.size(); ++i) {
		const Sample& s = samples[i];
		xmin = std::min(xmin, s.mX);
		xmax = std::max(xmax, s.mX);
		ymin = std::min(ymin, s.mY);
		ymax = std::max(ymax, s.mY);
	}

	printf("### DIM %d %d %d %d\n", xmin, xmax, ymin, ymax);

	int in_block_cnt = dimension / step;
	int x_total = (xmax - xmin) / step + 1;
	int y_total = (ymax - ymin) / step + 1;
	x_total = 1 + in_block_cnt * ((x_total + in_block_cnt - 1) / in_block_cnt);
	y_total = 1 + in_block_cnt * ((y_total + in_block_cnt - 1) / in_block_cnt);
	const int x_poly_total = x_total - 1;
	const int y_poly_total = y_total - 1;
	const int total = x_total * y_total;

	prMeshf mesh;
	prMeshf::tPoints3& points = mesh.GetVertices();
	prMeshf::tColors& colors = mesh.GetColors();

	points.resize(total);
	colors.resize(total);
	colors.memset(0);

	for (int y = 0; y < y_total; ++y) {
		for (int x = 0; x < x_total; ++x) {
			points[x + y * x_total].Set(x * step, 0.0f, y * step);
		}
	}

	for (size_t i = 0; i < samples.size(); ++i) {
		const Sample& s = samples[i];
		int x = (s.mX - xmin) / step;
		int y = (ymax - s.mY) / step;
		points[x + y * x_total].m_y = -s.mDepth;
		colors[x + y * x_total] = TypeToColor(s.mType);
	}

    float maxdepth = 0;
	for (int i = 0; i < total; ++i)
		maxdepth = prMathf::Max(maxdepth, -points[i].m_y);

	printf("Points allocated\n");


	prMeshEdgeMapperf builder(mesh);
	for (int y = 0; y < y_poly_total; ++y) {
		for (int x = 0; x < x_poly_total; ++x) {
			int xb = x_total * y + x;
			builder.AddPolygon(xb + x_total, xb + x_total + 1, xb + 1, xb);
		}
		if ((y % 50) == 0)
			printf("Polygons %d%c\n", 100 * (y + 1) / y_total, '%');
	}
	printf("\nPolygons done\n");


	prDataArrayT<LightPoint> light_points;
	light_points.resize(total);
	light_points.memset(0);
	for (size_t i = 0; i < total; ++i) {
		light_points[i].m_min = 1.0;
	}

	prMeshf::tIndices pindices;
	const int total_poly = x_poly_total * y_poly_total;
	for (int i = 0; i < total_poly; ++i) {
		prVector3f normal;
		prCalculateNormalMeshf::Calculate(mesh, i, normal);
		float light = prVector3f::AXIS_Y.DotProduct(normal);
		float angle = prMathf::aCos(prMathf::Clamp(light, 0.0f, 1.0f));
		angle = prMathf::Min(prMathf::HALF_PI(), 1.5f * angle);
		light = prMathf::Cos(angle);
 		mesh.GetPolygonVertices(i, pindices);
		for (size_t j = 0; j < pindices.size(); ++j) {
			light_points[pindices[j]].m_total += light;
			light_points[pindices[j]].m_min = prMathf::Min(light, light_points[pindices[j]].m_min);
			light_points[pindices[j]].m_cnt++;
		}
	}

	printf("\nLight done\n");

	const int x_block_cnt = (x_total - 1) / in_block_cnt;
	const int y_block_cnt = (y_total - 1) / in_block_cnt;
	prAssert(x_block_cnt * in_block_cnt + 1 == x_total);
	prAssert(y_block_cnt * in_block_cnt + 1 == y_total);

	prUShortArray indices;

	unsigned int block_index = 0;
	for (int y = 0; y < y_block_cnt; ++y) {
		for (int x = 0; x < x_block_cnt; ++x) {
			const int offsetx = x * dimension;
			const int offsetz = y * dimension;
			tRenderPoints render_points;
			prUShortArray render_triangles;
			prHashMapT<unsigned int, unsigned short> vertexmap;
			for (int iny = 0; iny < in_block_cnt; ++iny) {
				unsigned int polygon_start = (y * in_block_cnt + iny) * (x_total - 1) + x * in_block_cnt;
				for (int inx = 0; inx < in_block_cnt; ++inx) {
					mesh.GetPolygonVertices(polygon_start, pindices);
					for (size_t i = 0; i < pindices.size(); ++i) {
						unsigned int original = pindices[i];
						unsigned short local;
						if (!vertexmap.find(original, local)) {
							local = (unsigned short)render_points.size();
							vertexmap.put(original, local);
							const prVector3f point =  points[original];
							const int renderx = ((int)(point.m_x) - offsetx) / step;
							const int renderz = ((int)(point.m_z) - offsetz) / step;
							prAssert(renderx >= 0 && renderx <= in_block_cnt);
							prAssert(renderz >= 0 && renderz <= in_block_cnt);
							RenderPoint rp;
							rp.x = (unsigned char)renderx;
							rp.z = (unsigned char)renderz;
							const LightPoint& light_point = light_points[original];
							float light = light_point.m_min;//light_point.m_total / light_point.m_cnt;
							light = prMathf::Clamp(light, 0, 1.0f);
							rp.light = (unsigned char)(255.0f * light);
							const int depth = (int)(prMathf::Max(-point.m_y, 0.0f) * 65535.0f / maxdepth);
							prAssert(depth >= 0 && depth <= 65535);
							rp.depth = (unsigned short)depth;
							prAssert(colors[original] <= 7);
							rp.type = (unsigned char)colors[original];
							rp.reserved = 0;
							render_points.push_back(rp);
						}
						pindices[i] = local;
					}
					prAssert(pindices.size() == 4);
					render_triangles.push_back((unsigned short)pindices[0]);
					render_triangles.push_back((unsigned short)pindices[1]);
					render_triangles.push_back((unsigned short)pindices[2]);
					render_triangles.push_back((unsigned short)pindices[2]);
					render_triangles.push_back((unsigned short)pindices[3]);
					render_triangles.push_back((unsigned short)pindices[0]);
					++polygon_start;
				}
			}

			char block_name[32];
			sprintf(block_name, "/%.6d.mdl", block_index);

		    prString block_path = dir;
		    block_path += prStringConv::ToString(block_name);

		    prOutputStream::tSP output_block = GetStreamFactory()->CreateFileOutputStream(block_path);
		    prAssert(render_points.size() == (in_block_cnt + 1) * (in_block_cnt + 1));
		    prStreamUtils::WriteArray(output_block, render_points);
		    output_block = NULL;

		    prAssert(render_triangles.size() == in_block_cnt * in_block_cnt * 6);
		    if (indices.empty()) {
		    	indices = render_triangles;
		    } else {
		    	prAssert(!memcmp(&indices.front(),
   			                     &render_triangles.front(),
				                 in_block_cnt * in_block_cnt * 6 * sizeof(unsigned short)));
		    }

		    prUIntArray pixels;
		    int psize = in_block_cnt;
		    psize |= psize >> 1;
		    psize |= psize >> 2;
		    psize |= psize >> 4;
		    psize |= psize >> 8;
		    psize |= psize >> 16;
		    ++psize;
		    pixels.resize(psize * psize);
		    pixels.memset(0xff);

		    for (int py = 0; py <= in_block_cnt; ++py) {
			    for (int px = 0; px <= in_block_cnt; ++px) {
			    	int rindex = px + py * (in_block_cnt + 1);
			    	int pindex = px + py * psize;
			    	unsigned char depth = (render_points[rindex].depth & 0xff00) >> 8;
			    	pixels[pindex] = 0xff000000 | (depth << 16) | (depth << 8) | (depth);
			    }
		    }

			char block_dds[32];
			sprintf(block_dds, "/%.6d.dds", block_index);
		    prString block_dds_path = dir;
		    block_dds_path += prStringConv::ToString(block_dds);
		    prOutputStream::tSP pstream = GetStreamFactory()->
		    		CreateFileOutputStream(block_dds_path);
		    prImageWriter::WriteDDS(pstream, &pixels.front(), psize, psize);
		    pstream = NULL;

			++block_index;
		}
	}

    prString indices_path = dir;
    indices_path += prStringConv::ToString("/indices");
    prOutputStream::tSP output_indices = GetStreamFactory()->CreateFileOutputStream(indices_path);
    prStreamUtils::WriteArray(output_indices, indices);
    output_indices = NULL;

    prString layout_path = dir;
    layout_path += _T("/layout");

    prOutputStream::tSP output_layout = GetStreamFactory()->CreateFileOutputStream(layout_path);
    prStreamUtils::WriteStruct(output_layout, xmin);
    prStreamUtils::WriteStruct(output_layout, ymax);
    prStreamUtils::WriteStruct(output_layout, dimension);
    prStreamUtils::WriteStruct(output_layout, step);
    prStreamUtils::WriteStruct(output_layout, in_block_cnt);
    prStreamUtils::WriteStruct(output_layout, x_block_cnt);
    prStreamUtils::WriteStruct(output_layout, y_block_cnt);
    prStreamUtils::WriteStruct(output_layout, maxdepth);
    output_layout = NULL;

	printf("DONE %d points %d polygons %dx%d\n", (int)mesh.GetVertices().size(), (int)mesh.GetPolygons().size(), x_total, y_total);

	exit(10);
    return true;
}
