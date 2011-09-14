#include "output.h"
#include "CT.h"
#include "MainFrame.h"
#include "mat34.h"
#include "DicomReader.h"
#include "CPU_VD.h"

#include "Progress.h"
#include "vec.h"
#include "Draw.h"
#include "Img3DProc.h"
#include "RenderToTexture.h"
#include "DrawList.h"

#include "Geometry.h"
#include "Figures.h"
#include "MeshSection.h"
#include "Camera.h"

namespace CT
{
	
	Geometry bmesh;
	//std::vector<vec3> seg_points;
	//std::vector<ivec3> seg_id;
	//std::vector<int> seg_sel_points;

	//int is_seg_points_moving=-1; //ID of point we are moving
	
	void seg_BuildBox(vec3 a,vec3 b)
	{
		bmesh.renull();
		//AddBox(&bmesh,a,b,ivec3(10));
		AddBox(&bmesh,a,b);
		bmesh.RebuildTrBB();
		//bmesh.RebuildTrBB2();

	}
	#define CUT_NUM 10
	void seg_Cut(ivec2 a,ivec2 b)
	{
		Ray ray[4];
		ray[0] = CT::cam.GetRay(a.x,a.y);
		ray[1] = CT::cam.GetRay(b.x,a.y);
		ray[2] = CT::cam.GetRay(b.x,b.y);
		ray[3] = CT::cam.GetRay(a.x,b.y);
		vec3 vv[CUT_NUM*4];
		for(int i=0;i<4;i++)
		for(int j=0;j<CUT_NUM;j++)
		{
			vv[i+4*j] = ray[i].pos + ray[i].nav*(0.2f+j*0.5);
		}

		Geometry sect,res;
		Geometry*g = &sect;
		for(int i=0;i<4;i++)
		for(int j=0;j<CUT_NUM-1;j++)
			g->AddQuad(vv[i+4*j],vv[i+4*j+4],vv[(i+1)%4+4*j+4],vv[(i+1)%4+4*j]);

		g->AddQuad(vv[0],vv[1],vv[2],vv[3]);
		g->AddQuad(vv[CUT_NUM*4-1],vv[CUT_NUM*4-2],vv[CUT_NUM*4-3],vv[CUT_NUM*4-4]);
		g->RebuildTrBB();
		//g->RebuildTrBB2();

		GetAnotB(&bmesh,g,&res);
		bmesh.Copy(res);


		bmesh.BuildRep2();
		bmesh.BuildRep1();
		bmesh.RebuildTrBB();
		//bmesh.RebuildTrBB2();

	}
	

	void seg_Draw()
	{
		bmesh.color.set(1-CT::bg_color.x,1-CT::bg_color.y,1-CT::bg_color.z,0.6f);
		bmesh.Draw();

	}
	void seg_DrawMesh()
	{
		bmesh.color.set(1,1,1,1);
		bmesh.Draw();
	}
	void seg_Save(wxFile& fs)
	{
		SaveVector(fs,bmesh.tr);
	}
	void seg_Load(wxFile& fs)
	{
		bmesh.renull();
		OpenVector(fs,bmesh.tr);
	
	}
};