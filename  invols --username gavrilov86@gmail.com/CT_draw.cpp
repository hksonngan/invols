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



namespace CT
{
	void DrawScene();
	float fps=0;

	float *screen_depth=0;
	
void UpdateScreenDepth()
{
	if(screen_depth)delete[]screen_depth;
	screen_depth = new float[width * height];
	glReadPixels(0, 0, width, height,GL_DEPTH_COMPONENT,GL_FLOAT, screen_depth);
	
}
float GetScreenDepth(int x,int y)
{
	float res;
	glReadBuffer(GL_FRONT);
	glReadPixels(x, height-y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT, &res);
	return res;
}
float DepthToLength(float depth)
{
	float n = Z_NEAR,f = Z_FAR,res;
	
	if(CT::cam.GetProjection()) 
	{
		if(depth==1.0f) 
			res = -1;
		else
			res = f*n/(f-(f-n)*depth);
	}
	else
	{
		res = depth*(f-n)+n;
	}
	return res;

//	return clamp((dot(ps,nav)-z_near)/(z_far-z_near),0.01,0.99);
}
vec3 marker(0);
vec3 GetPointByDepth(int x,int y)
{
	Ray ray = cam.GetRay(x,y);
	return ray.pos + ray.nav*DepthToLength(GetScreenDepth(x,y));
}

void SetupGL()
{
	glewInit();
	glClearColor(bg_color.x,bg_color.y,bg_color.z,1.0);
	
	//glShadeModel(GL_FLAT);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_LINE_SMOOTH);
	glLineStipple(1,15+(15<<8));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

bool need_InitInterlace=1;
void InitInterlace()
{
	need_InitInterlace = 1;
}
void InitInterlace_()
{
	need_InitInterlace = 0;
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
	Begin2D(width,height);
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	//glStencilMask(1);
	glClear (GL_STENCIL_BUFFER_BIT);
	

	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
	glBegin(GL_LINES);
	for(int i=0;i<height;i+=2)
	{
		glVertex2f(0,i);
		glVertex2f(width,i);
	}
	glEnd();

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_STENCIL_TEST);
	End2D();
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}

void Resize(int _width,int _height)
{
	bool resized = (_width!=width)||(_height!=height);

	if(RM_pic_man) 
	{
	}
	width=_width;
	height=_height;

	rtt_Resize(width,height);

	cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,0,0,width,height);

	//if(resized)
		if(stereo_on==3)InitInterlace();
	
	CT::need_rerender=1;
	
}
float glfwGetTime()
{
	return GetTickCount()*0.001f;
	
}
void CalcFPS()
{
	
	static int frames = 0;
	static double start = glfwGetTime ( );

    double time = glfwGetTime ( );
    if ( ( time - start ) > 1.0 || frames == 0 )
    {
         fps = frames / ( time - start );
         start = time;
         frames = 0;
/*
		 if(auto_fps)
		 {
			 if(fps>25)
				 iso->SetStepLength(max(iso->GetStepLength()*0.9f,0.002f));
			 if(fps<10)
				 iso->SetStepLength(min(iso->GetStepLength()*2,0.02f));
		 }
		 */
    }
    frames++;
	
}
void InitGL()
{
//	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

//	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glLineStipple(1,15+(15<<8));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_MULTISAMPLE_ARB);
	glClearColor(bg_color.x,bg_color.y,bg_color.z,1.0);

}
void UploadVD(int id)
{
	if(CPU_VD::full_data.GetSlices())
	{
		CT::GetData(id)->Upload(CPU_VD::full_data);
		vec3 scale1 = CPU_VD::full_data.spacing*CPU_VD::gpu_size.ToVec3()*CPU_VD::scale;

		iso->volume_transform[id].center=CPU_VD::real_gpu_b1;
		iso->volume_transform[id].x.set(scale1.x,0,0);
		iso->volume_transform[id].y.set(0,scale1.y,0);
		iso->volume_transform[id].z.set(0,0,scale1.z);
		iso->ReLoadShader();
	}
}
void FullDraw()
{

	if(light_to_camera)
		iso->SetLightDir(CT::cam.GetNav()*(-1));


	if(need_reload_volume_data)
	{
		UploadVD(CT::GetCurDataID());
		need_reload_volume_data=0;
	}
	CalcFPS();
	

	InitGL();
	if(IsFastView())cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,0,0,width/fast_res,height/fast_res);
	else cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,0,0,width,height);
//InitInterlace();
	if(need_InitInterlace)InitInterlace_();

		//SetLight(cam.GetPosition());

		switch(stereo_on)
		{
		case 0:	
			{
				CT::gl_back_buffer_target = GL_BACK;
//				if(IsFastView())rtt_Begin(0);
				//glClearColor(0,0,0,0);
				if(need_rerender)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//				if(IsFastView())rtt_End();

				DrawScene();
			}
			break;
		case 1:
				CT::gl_back_buffer_target = GL_BACK;
			{
				//vec3 ps = cam.GetCenter();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				if(CT::RenderingType==2)
				{
					is_left=!is_left;
					DrawScene();
					glFlush();
				}else
				{
					glClear(GL_ACCUM_BUFFER_BIT);
					is_left=0;
					DrawScene();
					//glFlush();
					glAccum(GL_LOAD, 1.0f);                
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					is_left=1;
					DrawScene();
					//glFlush();
					glAccum(GL_ACCUM, 1.0f);
					glAccum(GL_RETURN, 1.0f);				
				}


			}
			break;
		case 2:
			{
				CT::gl_back_buffer_target = GL_BACK;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				if(CT::RenderingType==2)
				{
					is_left=!is_left;
					cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,is_left*width/2,0,width/2,height);
					DrawScene();
					glFlush();
				}else
				{
					glClear(GL_ACCUM_BUFFER_BIT);
					is_left=0;
					cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,is_left*width/2,0,width/2,height);
					DrawScene();
					//glFlush();
					glAccum(GL_LOAD, 1.0f);                
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					is_left=1;
					cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,is_left*width/2,0,width/2,height);
					DrawScene();
					//glFlush();
					glAccum(GL_ACCUM, 1.0f);
					glAccum(GL_RETURN, 1.0f);				
				}
				/*
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				vec3 ps = cam.GetCenter();

				cam.SetCenter(ps-cam.GetLeft()*stereo_step);
				cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,0,0,width/2,height);

				DrawScene();

				cam.SetCenter(ps+cam.GetLeft()*stereo_step);
				cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,width/2,0,width/2,height);
				DrawScene();

				cam.SetCenter(ps);
				*/
			}
			break;
		case 3:
			{
				CT::gl_back_buffer_target = GL_BACK;

				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_EQUAL, 0, 1);
				is_left=0;
				DrawScene();
				glStencilFunc(GL_EQUAL, 1, 1);
				is_left=1;
				DrawScene();
				glDisable(GL_STENCIL_TEST);
			}
			break;
		case 4:
			{
				is_left=0;
				CT::gl_back_buffer_target = GL_BACK_LEFT;
				glDrawBuffer(CT::gl_back_buffer_target);  
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      //clear color and depth buffers
				DrawScene();

				is_left=1;
				CT::gl_back_buffer_target = GL_BACK_RIGHT;
				glDrawBuffer(CT::gl_back_buffer_target);  
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      //clear color and depth buffers
				DrawScene();

  
  			}
			break;
		}
}

void SetStereoMode(int stereo_mode)
{
	switch(stereo_on)
			{
			case 0:	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					MyApp::frame->m_canvas[0]->SwapBuffers();
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				break;
			case 1: iso->SetAnag(0,0);
				
				break;
			case 2: cam.SetupProjection(CAMERA_ANGLE,Z_NEAR,Z_FAR,0,0,width,height);
				break;
			case 4:glDrawBuffer(CT::gl_back_buffer_target);
				break;
			}
	
	stereo_on = stereo_mode;
	if(stereo_on==3)InitInterlace();
	need_rerender=1;
}


}