#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"
#include "CPU_VD.h"

//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "tf2D.h"
#include "drawtext.h"
#include "Img3DProc.h"


void TF2D_window::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    SetCurrent();
	//if(MyApp::gl_window)MyApp::gl_window->GetContext();else
    if (!GetContext()) return;

	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height,0);
	glMatrixMode(GL_MODELVIEW);

	
	InitGL();

	glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();

	int height1 = height-2*TF2D_MARGIN;
	
	//this->tf = CT::color_table+TF_WIDTH*CT::GetCurDataID();
	//tf = CT::iso->GetCurTF();

	//isos = &CT::iso->GetCurRM()->isos[CT::GetCurDataID()];
//	tf_quads2 = &CT::iso->GetCurRM()->tf_quads2[CT::GetCurDataID()];

	/// Fill
	
	//histogram
	
	

	/// FRAME
	glColor3f(0.5f,0.5f,0.5f);
	DrawRectangle(vec2(0,TF2D_MARGIN),vec2(width,TF2D_MARGIN+height1));

	glBegin(GL_LINES);
	glColor3f(0.4f,0.4f,0.4f);
	float hh=6;

	
	float cx = (-center)*scale;
	float step=scale/(256.0f*128.0f);
	int grid_step = 1000;
	if(scale>1000)grid_step = 100;
	if(scale>10000)grid_step = 10;
	if(scale>100000)grid_step = 1;

	int aaa = data_stat.MinValue+10000;
	int i1=data_stat.MinValue,i2=data_stat.MaxValue,i_max=data_stat.MaxValue-data_stat.MinValue;
	for(int i=0;i<=i_max;i++)
	{
		if(cx>0)
		{
			if(i1==data_stat.MinValue)i1=i+data_stat.MinValue;
			if(aaa%grid_step==0)
			{
				float hhh = 4+4*(aaa%(10*grid_step)==0);
				if(aaa%(100*grid_step)==0)glColor3f(1,1,1);
				if(aaa==10000){glColor3f(0,0,1);hhh=-height1/3;}
				glVertex2f(cx,height1+TF2D_MARGIN);
				glVertex2f(cx,height1+TF2D_MARGIN+hhh);
				glVertex2f(cx,TF2D_MARGIN);
				glVertex2f(cx,TF2D_MARGIN-hhh);
				if(aaa%100==0)glColor3f(0.4f,0.4f,0.4f);
			}
		}
		if(cx>=width){i2=i+data_stat.MinValue;break;}

		cx += step;
		aaa++;
	}
	

	glEnd();

	start_view = i1;
	end_view = i2;

	
	//DrawText("DR SD",vec2(12,32),vec4(0.6f));


    glFlush();
    SwapBuffers();

}