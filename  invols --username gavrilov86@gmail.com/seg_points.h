#ifndef SEG_POINTS_INC
#define SEG_POINTS_INC

#include <stdio.h>
#include <stdlib.h>
#include "AllInc.h"
#include "vec.h"
#include "vec3.h"

namespace CT
{
	void seg_DrawMesh();
	void seg_BuildBox(vec3 a,vec3 b);
	//void seg_BuildFromBC(short* bc_data, ivec3 bc_size);

	void seg_Draw();
	void seg_Save(wxFile& fs);
	void seg_Load(wxFile& fs);

	extern bool use_bounding_mesh;
//	extern int is_seg_points_moving;
//	extern std::vector<int> seg_sel_points;

	void seg_Cut(ivec2 a,ivec2 b);
};
#endif