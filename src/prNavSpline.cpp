#include "prCoreDefs.h"
#include "prNavSpline.h"
#include "Core/Stream/prStreamUtils.h"
#include "Core/Utils/String/prStringTokenizer.h"
#include "Core/Geometric/Mesh/Misc/prMeshEdgeMapper.h"
#include "prWorld.h"

prNavSpline::prNavSpline(const prWorld& world) : world_(world) {
}

prNavSpline::~prNavSpline() {

}

prMeshf::tSP prNavSpline::CreateMesh() {
	prMeshf::tSP mesh = new prMeshf();

	prMeshf::tPoints3& points = mesh->GetVertices();
	float step = 1.0f / m_geomSpline->GetLenght();
	float t = 0;
	prVector3f prev_point;
	while (true) {
		if (t == 1.0f)
			break;
		const prVector3f point = m_geomSpline->Evaluate(t);
		prVector3f lat;
		prVector3f dir;
		if (t == 0) {
			dir = m_geomSpline->Evaluate(step) - point;
		} else {
			dir = point - prev_point;
		}
		dir.Normalize();
		lat.FromCrossProduct(prVector3f::AXIS_Y, dir);
		lat.Normalize();
		prVector3f p1 = point - lat * 2.5f;
		world_.Raycast(p1.m_x, p1.m_z, p1.m_y);
		prVector3f p2 = point + lat * 2.5f;
		world_.Raycast(p2.m_x, p2.m_z, p2.m_y);
		p1.m_y = prMathf::Min(0.0, 1.0f - p1.m_y);
		p2.m_y = prMathf::Min(0.0, 1.0f - p2.m_y);
		points.push_back(p1);
		points.push_back(p2);
		prev_point = point;
		t = prMathf::Min(1.0f, t + step);
	}
	const size_t pcnt = points.size() / 2 - 1;

	prMeshEdgeMapperf builder(*mesh);
	for (size_t i = 0; i < pcnt; ++i) {
		builder.AddPolygon(i * 2 + 2, i * 2 + 3, i * 2 + 1, i * 2);
	}
	return mesh;
}

prNavSpline::tSP prNavSpline::Load(const prWorld& world, const prChar* data_path) {
	prNavSpline::tSP obj = new prNavSpline(world);

	prStringASCII data;
    if (!prStreamUtils::ReadStringFile(data_path, data)) {
		printf("Error! Failed to read data file\n");
		return NULL;
	}

    prStringTokenizerA tokenizer(data, "\n\r");
	prStringTokenizerA::Token line;
	obj->m_geomSpline = new gaSpline();
	while (tokenizer.NextToken(line)) {
		prStringTokenizerA tk(line, " ");
		prStringTokenizerA::Token en;
		float time, lat, lon, accuracy, x, y;
		if (!tk.NextToken(en) || !prStringUtils::Parse(en, time) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, lat) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, lon) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, accuracy) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, x) ||
			!tk.NextToken(en) || !prStringUtils::Parse(en, y)) {
			printf("Error! Failed to parse: %s\n", (const char*)line);
			return NULL;
		}
		x = x - world.xmin();
		y = world.ymax() - y;
		obj->m_geomSpline->AddPoint(prVector3f(x, 0, y));
		obj->m_times.push_back(time);
		obj->m_accuracies.push_back(accuracy);
	}
	obj->m_geomSpline->Rebuild();

	printf("Spline created: %f\n", obj->m_geomSpline->GetLenght());

	return obj;
}

prMeshf::tSP CreateMesh();
