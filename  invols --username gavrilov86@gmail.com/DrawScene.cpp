#include <stdio.h>
#include <stdlib.h>
#include "AllDef.h"
#include "Mouse.h"
#include "Camera.h"
#include "Draw.h"
#include "Render.h"
#include "Draw2D.h"

#include <Math.h>
#include <ivec3.h>
#include "output.h"
#include "CT.h"
#include "MainFrame.h"
#include "GLCanvas.h"
#include "DicomReader.h"
#include "CPU_VD.h"
#include "RenderToTexture.h"
#include "seg_points.h"
#include "bounding_cells.h"

void DrawGrid(vec3 a,vec3 b,float step);

void Draw2DInfo();
namespace CT
{
	extern bool rendering_to_file;
void DrawCS1()
{
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glLineWidth(3);

	float k=0.06f,arr_h=0.02f,arr_d=0.003f,wh=width/float(height+1);
	glPushMatrix();
	vec3 dt;
	if(cam.GetProjection())
		dt = cam.GetPosition()+cam.GetNav()*0.6f-(cam.GetLeft()*wh+cam.GetTop())*0.2f;
	else
	{
		float CameraZoom2D=0.3f,zz=CameraZoom2D*cam.GetDistance()*3.2;
		dt = cam.GetPosition()+cam.GetNav()*0.5f-(cam.GetLeft()*wh+cam.GetTop())*0.2f*zz;
		k*=zz;arr_h*=zz;arr_d*=zz;
	}
	
	glTranslated(dt.x,dt.y,dt.z);
	
	glColor4f(1,0,0,0.5);
	glMYLine(0,0,0,k,0,0);
	DrawPrizma(vec3(k,0,0),vec3(k-arr_h,arr_d,0),vec3(k-arr_h,0,arr_d),vec3(k-arr_h,-arr_d,-arr_d));
	
	glColor4f(0,1,0,0.5);
	glMYLine(0,0,0,0,k,0);
	DrawPrizma(vec3(0,k,0),vec3(arr_d,k-arr_h,0),vec3(-arr_d,k-arr_h,-arr_d),vec3(0,k-arr_h,arr_d));
	
	glColor4f(0,0,1,0.5);
	glMYLine(0,0,0,0,0,k);
	DrawPrizma(vec3(0,0,k),vec3(arr_d,0,k-arr_h),vec3(-arr_d,-arr_d,k-arr_h),vec3(0,arr_d,k-arr_h));

	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glLineWidth(1);

}

void SetLight(vec3 pos)
{
	GLfloat ps[4] = {pos.x,pos.y,pos.z,0};
    glLightfv(GL_LIGHT0,GL_POSITION, ps);
	
}
void DrawLightSources()
{
	glColor3f(1,1,0);
	vec3 cc = (iso->GetBoundingBox(0)+iso->GetBoundingBox(1))*0.5f;
	vec3 ln = iso->GetLightDir();
	//DrawCylinder(cc+ln*1.5,cc+ln*1,0.03f);
	glLineWidth(3);
	DrawLine(cc+ln*1.5,cc+ln*1);
	glLineWidth(1);

	//DrawSphere(iso->GetDotLight(),0.05f);
}
bool IsFastView()
{
//	return (!RM_pic_man && (is_changing || (d_zoom!=1))) && (fast_res!=1);
	return (is_changing_tf || is_changing_box || is_changing || (d_zoom!=1)) && (fast_res!=1) && (!stereo_on ||(stereo_on==1 && RenderingType==2));
}
int GetFastWidth()
{
	return IsFastView()?(width/fast_res):width;
}
int GetFastHeight()
{
	return IsFastView()?(height/fast_res):height;
}

void DrawScene()
{

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	int width1 = GetFastWidth();
	int height1 = GetFastHeight();

	vec3 ps = cam.GetCenter();
	if(stereo_on)
	{
		
		cam.SetCenter(ps+cam.GetLeft()*stereo_step*(is_left?-1:1));
		if(stereo_on==1)
		{
			iso->SetAnag(anag,is_left);
		}
	}
	
	

	cam.SetupPosition();
	glEnable(GL_DEPTH_TEST);
	
	if(IsFastView())
	{
		rtt_Update(width/fast_res,height/fast_res);
	}
	if(!draw_bounding_mesh)
	if(use_bounding_mesh)
	{
		static ShaderProgram sp("common_shaders//srf.vs", "common_shaders//store_dist.fs");
		sp.Use();
		sp.SetVar("pos",CT::cam.GetPosition());
		sp.SetVar("nav",CT::cam.GetNav());
		sp.SetVar("screen_width",(float)width1);
		sp.SetVar("screen_height",(float)height1);
		sp.SetVar("free_depth",0.99f);

		glBlendFunc(GL_ONE, GL_ZERO);
		glClearColor(0,0,0,0);

		

		for(int fr=0;fr<2;fr++)
		{
			if(fr)glCullFace(GL_FRONT);
			rtt_Begin(fr+1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			rtt_End();
			rtt_Begin(3);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			rtt_End();

			for(int d=0;d<4;d++)
			{
				int id_store = fr+1,id_copy = 3;
				if(!(d&1))swap(id_store,id_copy);
				sp.SetVar("fdepth",d);
				sp.SetVar("dist_txt", rtt_SetID(2,rtt_GetTexture(id_copy)));
				
				if(d==3 && fr){glClearDepth(0.0f);  glDepthFunc(GL_GREATER);		sp.SetVar("free_depth",0.01f);}
				rtt_Begin(id_store);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
				seg_DrawMesh();
				if(d==3 && fr){glClearDepth(1.0f);  glDepthFunc(GL_LESS);}
				rtt_End();
			}
			if(fr)glCullFace(GL_BACK);
		}
		sp.UnUse();
		glClearColor(bg_color.x,bg_color.y,bg_color.z,1.0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		


	}

	///if(IsFastView())
	{
		rtt_Begin(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	
	SetLight(iso->GetLightDir());

	int cur_rm = iso->GetCurRMID();
	for(int i=0;i<iso->rendering_methods.size();i++)
	{
		if(RM_pic_man)
		{
			vec2 pos = (iso->rendering_methods[i]->pos-RM_center)*RM_scale*height1/800.0f;
			vec2 size = iso->rendering_methods[i]->size*RM_scale*height1/800.0f;

			cam.SetupProjection(pos.x,height1-pos.y-size.y,size.x,size.y);
		}
		vec3 bb1=iso->GetBoundingBox(0),bb2=iso->GetBoundingBox(1);
		if(draw_frame_is && !stereo_on)
		{
/*			glColor3d(1,1,1);
			DrawLine(marker-vec3(1,0,0),marker+vec3(1,0,0));
			DrawLine(marker-vec3(0,1,0),marker+vec3(0,1,0));
			DrawLine(marker-vec3(0,0,1),marker+vec3(0,0,1));
*/
			if(!light_to_camera)
				DrawLightSources();
			glEnable(GL_DEPTH_TEST);
			DrawCS1();
			DrawGrid(bb1,bb2,10/DicomReader::GetScale());
			//bc_Draw();

			if(use_bounding_mesh && draw_bounding_mesh)
				seg_Draw();
		}

		if(RM_pic_man)iso->SetCurRM(i);
		if(!draw_bounding_mesh)
		iso->Draw(draw_small_data);


		if(!RM_pic_man)break;
	}
	if(RM_pic_man) iso->SetCurRM(cur_rm);
	
	if(RM_pic_man)
//	if(draw_frame_is)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glViewport(0,0,width1,height1);
		Begin2D(width1,height1);

		
		for(int i=0;i<iso->rendering_methods.size();i++)
		{
			vec2 pos = (iso->rendering_methods[i]->pos-RM_center)*RM_scale*height1/800.0f;
			vec2 size = iso->rendering_methods[i]->size*RM_scale*height1/800.0f;
			if(i == iso->GetCurRMID())glColor3d(1,1,0.5);else glColor3d(0.3,0.3,0.3);
			DrawRectangle(pos,pos+size);
			DrawFilledRectangle(pos+vec2(0,-size.y*0.1f),pos+vec2(size.x,0));


		}

		End2D();
		//Draw2DInfo();
	}
	//if(IsFastView())
	

		if(rendering_to_file)
		{
			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

			unsigned char *dt = new unsigned char [width*height*3];

			glReadPixels(0, 0, width,height, GL_RGB,GL_UNSIGNED_BYTE,dt);	
			wxImage img(width,height,dt,true);
			wxBitmap bp(img);
			bp.SaveFile(screenshot_dst,wxBITMAP_TYPE_PNG);
			delete[]dt;
		}
		rtt_End();

	
//		glEnable(GL_TEXTURE_2D);
	
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);
		glViewport(0,0,width,height);
		glColor3d(1,1,0);
		Begin2D(width,height);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rtt_GetTexture(0));

		static ShaderProgram sp("common_shaders//srf.vs", "common_shaders//fast_view.fs");

		sp.Use();
		sp.SetVar("f_text",0);
		sp.SetVar("txt_size",vec4(rtt_GetTextureSize().x,rtt_GetTextureSize().y,!use_bicubic_filtering,0));
 		//DrawFilledRectangle(vec2(0,0),vec2(width,height),vec2(0),vec2(1.0f/fast_res));
		DrawFilledRectangle(vec2(0,0),vec2(width,height));
		sp.UnUse();

		//glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);

		End2D();
	
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glViewport(0,0,width,height);
	Begin2D(width,height);

	if(!is_changing_box)
	if(use_bounding_mesh && (CT::mouse.btn&1) && !RM_pic_man )
	{
		glColor4f(1,1,1,0.6f);
		DrawRectangle(vec2(old_mouse_pos.x,old_mouse_pos.y),vec2(mouse.oldx,mouse.oldy));
	}
	End2D();
	glEnable(GL_DEPTH_TEST);

	
	if(IsFastView())
	{
		rtt_Update(width,height);
	}

	if(stereo_on)
		cam.SetCenter(ps);


}

void DrawScene1()
{
	cam.SetupPosition();
	glEnable(GL_DEPTH_TEST);
	SetLight(iso->GetLightDir());
	
	vec3 center = GetCenter();
		vec3 scale = CPU_VD::GetScale();
		vec3 bb1=iso->GetBoundingBox(0),bb2=iso->GetBoundingBox(1);
	if(draw_frame_is)
	{
		DrawLightSources();
		DrawCS1();
		DrawGrid(bb1,bb2,10/DicomReader::GetScale());

		glDisable(GL_LIGHTING);

		ShaderProgram*sp = CT::viewMPR_sp;
		sp->Use();
		sp->SetVar("min_level",CT::iso->GetWindow().x);
		sp->SetVar("max_level",CT::iso->GetWindow().y);
		sp->SetVar("LightDir",CT::iso->GetLightDir());
		sp->SetVar("pos",CT::cam.GetPosition());
		sp->SetVar("scale",CPU_VD::GetScale());
		sp->SetVar("txt_level_color",CT::iso->GetTFTexture()->SetID(2));

		//if(0)
		for(int i=0;i<3;i++)
		if(draw_MRP[i])
		if(MyApp::frame->v2d[i])
		if(MyApp::frame->v2d[i]->v2d->txt2)// || MyApp::frame->v2d[i]->v2d->txt_reg)
		{
			//glBindTexture(GL_TEXTURE_2D,MyApp::frame->v2d[i]->v2d->txt2->SetID(1));
			if(MyApp::frame->v2d[i]->v2d->txt2)sp->SetVar("txt",MyApp::frame->v2d[i]->v2d->txt2->SetID(1));
//			if(MyApp::frame->v2d[i]->v2d->txt_reg)sp->SetVar("txt_reg",MyApp::frame->v2d[i]->v2d->txt_reg->GetTexture());
			if(i==0)DrawCube(vec3(center.x*scale.x,0,0),vec3(center.x*scale.x,scale.y,scale.z));
			if(i==1)DrawCube(vec3(0,center.y*scale.y,0),vec3(scale.x,center.y*scale.y,scale.z));
			if(i==2)DrawCube(vec3(0,0,center.z*scale.z),vec3(scale.x,scale.y,center.z*scale.z));
		}
		
		sp->UnUse();

		
		
		///
		glColor3f(1,0,0);
		if(draw_MRP[0])DrawCubeFrame(vec3(center.x*scale.x,0,0),vec3(center.x*scale.x,scale.y,scale.z));
		glColor3f(0,1,0);
		if(draw_MRP[1])DrawCubeFrame(vec3(0,center.y*scale.y,0),vec3(scale.x,center.y*scale.y,scale.z));
		glColor3f(0,0,1);
		if(draw_MRP[2])DrawCubeFrame(vec3(0,0,center.z*scale.z),vec3(scale.x,scale.y,center.z*scale.z));

glEnable(GL_LINE_STIPPLE);
		glColor3f(1,0,0);
		glPushMatrix();
		CS3 cs11 = CT::iso->volume_transform[CT::GetCurDataID()];
		ChangeCoordSystem(cs11);
		DrawCubeFrame(vec3(0),vec3(1));

		glPopMatrix();
		glColor3f(0.8f,0.8f,0);
		if(draw_frame_is)
			DrawCubeFrame(vec3(0),vec3(1));

		glColor3f(0.4f,0.4f,1);
		if(draw_gpu_box)
			DrawCubeFrame(CPU_VD::real_gpu_b1,CPU_VD::real_gpu_b2);

		bb1 = (CPU_VD::GetRecomendedGPUOffset().ToVec3()/CPU_VD::full_data.GetSize().ToVec3())*scale;
		bb2 = ((CPU_VD::GetRecomendedGPUOffset()+CPU_VD::gpu_size_dummy).ToVec3()/CPU_VD::full_data.GetSize().ToVec3())*scale;
		glColor3f(0,1,0);
		
		if(draw_gpu_dummy)
		DrawCubeFrame(bb1,bb2);
glDisable(GL_LINE_STIPPLE);
	}
	iso->Draw(draw_small_data);
	
	
	glEnable(GL_LIGHTING);


/*
	if(stereo_on!=1)
	{
		glColor3d(1,1,1);
		if(keyboard['3'])
			DrawDot(iso->GetMinBox());
		if(keyboard['4'])
			DrawDot(iso->GetMaxBox());
	}
	*/
}
}