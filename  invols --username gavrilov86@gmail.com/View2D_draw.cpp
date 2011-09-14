#include "CT/CT.h"
#include "Draw2D.h"
#include "CPU_VD.h"
#include "wxIncludes.h"
#include "MainFrame.h"
#include "DicomReader.h"

//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "View2D.h"
#include "drawtext.h"
#include "Img3DProc.h"
//#include "GLCanvas.h"


void View2D::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	
    wxPaintDC dc(this);
	//if(MyApp::gl_window)MyApp::gl_window->GetContext();else
    //if (!GetContext()) return;

    SetCurrent(*MyApp::GLContext);
	InitGL();
	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width/(height+0.0001f), 1,0);
	glMatrixMode(GL_MODELVIEW);

	if(need_texture_reload)LoadTexture();

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	vec2 b1 = Get2DBoxMin(), b2 = Get2DBoxMax();
	float hscale = 1.0f/(height+0.0001f);

	glScaled(scale*mirror.x,scale*mirror.y,1);

	glTranslated(-center.x,-center.y,0);


	vec2 full_size = Get2DPoint(CPU_VD::GetFullSize());

	if(txt2)// || txt_reg)
	{
//		glBindTexture(GL_TEXTURE_2D,txt2->GetTexture());
		ShaderProgram*sp = CT::view2d_sp;
		sp->Use();
		if(txt2)sp->SetVar("txt",txt2->SetID(1));
//		if(txt_reg)sp->SetVar("txt_reg",txt_reg->GetTexture());
		//sp->SetVar("txt_level_color",CT::iso->GetTFTexture()->SetID(2));
		sp->SetVar("min_level",bc.x);
		sp->SetVar("max_level",bc.y);
		sp->SetVar("size",Get2DPoint(CPU_VD::full_data.GetSize().ToVec3()));
		sp->SetVar("params",vec4(use_bicubic_filt,0,0,0));
		//DrawFilledRectangle(b1,b2);
		DrawFilledRectangle(vec2(0,full_size.y),vec2(full_size.x,0));
		sp->UnUse();
	}
	
		vec2 bb1,bb2;
	glEnable(GL_LINE_STIPPLE);
		glColor3f(0.8f,0.8f,0);
		DrawRectangle(vec2(0),full_size);

		bb1 = Get2DPoint(CPU_VD::gpu_offset.ToVec3()/CPU_VD::full_data.GetSize().ToVec3())*full_size;
		bb2 = Get2DPoint((CPU_VD::gpu_offset+CPU_VD::gpu_size).ToVec3()/CPU_VD::full_data.GetSize().ToVec3())*full_size;
		glColor3f(0.4f,0.4f,1);
		DrawRectangle(bb1,bb2);
		bb1 = Get2DPoint((CPU_VD::GetRecomendedGPUOffset().ToVec3()/CPU_VD::full_data.GetSize().ToVec3()))*full_size;
		bb2 = Get2DPoint(((CPU_VD::GetRecomendedGPUOffset()+CPU_VD::gpu_size_dummy).ToVec3()/CPU_VD::full_data.GetSize().ToVec3()))*full_size;
		glColor3f(0,1,0);
		DrawRectangle(bb1,bb2);


glDisable(GL_LINE_STIPPLE);

	bb1 = Get2DPoint(CT::GetCenter())*full_size;

	glColor3f(axis_x==0,axis_x==1,axis_x==2);
	DrawLine(vec2(bb1.x,-3),vec2(bb1.x,4));
	glColor3f(axis_y==0,axis_y==1,axis_y==2);
	DrawLine(vec2(-3,bb1.y),vec2(4,bb1.y));

	glColor3d(1,1,1);

	vec2 hh = vec2(2*hscale/scale);

	if(sel_obj == V2D_SELECTION_OBJECT_CENTER)
	{
		if(sel_part == V2D_SELECTION_PART_X)
			DrawRectangle(vec2(-3,bb1.y-hh.x),vec2(4,bb1.y+hh.x));
		if(sel_part == V2D_SELECTION_PART_Y)
			DrawRectangle(vec2(bb1.x-hh.x,-3),vec2(bb1.x+hh.x,4));
		if(sel_part == V2D_SELECTION_PART_CENTER)
			DrawRectangle(bb1-hh*4,bb1+hh*4);

	}
//
	DrawRectangle(b1,b2);

	if(sel_obj == V2D_SELECTION_OBJECT_BB)
	{
		if(sel_part == V2D_SELECTION_PART_POINT)
		{
			vec2 pp( (sel_id==0||sel_id==2)?b1.x:b2.x , (sel_id<=1)?b1.y:b2.y );
			DrawRectangle(pp-hh*4,pp+hh*4);
			
		}else
		if(sel_part == V2D_SELECTION_PART_EDGE)
		{
			vec2 p1( (sel_id<=2)?b1.x:b2.x , (sel_id==0||sel_id>=2)?b1.y:b2.y );
			vec2 p2( (sel_id==2)?b1.x:b2.x , (sel_id==0)?b1.y:b2.y );
			DrawRectangle(p1+hh,p2-hh);
			
		}
	}
	glLoadIdentity();
	glColor3f(1,1,1);
	//glScaled(scale*hscale,scale*hscale,1);
	float xx = width*hscale*0.9f,step_sm=(scale*10)/DicomReader::GetScale(),hhh=step_sm*5.0f;
	DrawRectangle(vec2(xx-step_sm,0.5f-hhh),vec2(xx,0.5f+hhh));

    glFlush();
    SwapBuffers();

}