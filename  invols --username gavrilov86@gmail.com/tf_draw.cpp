#include "CT/CT.h"
#include "Draw2D.h"

#include "wxIncludes.h"
#include "MainFrame.h"
#include "CPU_VD.h"

//#include <GL/gl.h>
//#include <GL/glu.h>

#include "AllDef.h"
#include "TF_window.h"
#include "drawtext.h"
#include "Img3DProc.h"


void TF_window::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	double scale = tf_scale[CT::GetCurDataID()];
	double center = tf_center[CT::GetCurDataID()];

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

	int height1 = height-2*TF_MARGIN;
	
	//this->tf = CT::color_table+TF_WIDTH*CT::GetCurDataID();
	tf = CT::iso->GetCurTF();

	isos = &CT::iso->GetCurRM()->isos[CT::GetCurDataID()];
	tf_quads = &CT::iso->GetCurRM()->tf_quads[CT::GetCurDataID()];
	tf_points = &CT::iso->GetCurRM()->tf_points[CT::GetCurDataID()];

	/// Fill
	vec2 win = CT::iso->GetTFWW();
	//if(!isos_mode)
	{
		/*
	
		glBegin(GL_TRIANGLE_STRIP);
		int cx0 = (win.x-center)*scale;
		glColor4fv(&tf[0].x);
		for(int i=1;i<TF_WIDTH;i++)
		{
			int cx = ((i/float(TF_WIDTH))*(win.y-win.x)+win.x-center)*scale;
			glVertex2f(cx0,height1+TF_MARGIN);
			glVertex2f(cx0,height1*(1-tf[i-1].w)+TF_MARGIN);
			glColor3fv(&tf[i].x);
			glVertex2f(cx,height1+TF_MARGIN);
			glVertex2f(cx,height1*(1-tf[i].w)+TF_MARGIN);
			cx0=cx;
		}
		glEnd();
		*/
		glBegin(GL_LINE_STRIP);
		for(int i=0;i<TF_WIDTH;i++)
		{
			int cx = ((i/float(TF_WIDTH))*(win.y-win.x)+win.x-center)*scale;
			glColor3fv(&tf[i].x);
			if(!i)glVertex2f(0,height1*(1-tf[i].w)+TF_MARGIN);
			glVertex2f(cx,height1*(1-tf[i].w)+TF_MARGIN);
			if(i==TF_WIDTH-1)glVertex2f(width,height1*(1-tf[i].w)+TF_MARGIN);
			
		}
		glEnd();

		
	}
	//histogram
	Histogram&histogram = CT::histogram[CT::GetCurDataID()];
	
	if(histogram.Get())
	{
	glBegin(GL_QUADS);
	glColor4f(1,1,1,0.4);
	float step;
	if(CPU_VD::full_data.GetValueFormat()==0)step=scale/(256.0f*128.0f/histogram.chunk_size);
	if(CPU_VD::full_data.GetValueFormat()==1)step=scale/(256.0f/histogram.chunk_size);
	float cx = (-center)*scale;
	float scsc = height1/float(histogram.GetMaxChunk());
	unsigned int*ch = histogram.Get();
	for(int i=0;i<histogram.GetSize();i++,ch++)
	{
		float cy = height1+TF_MARGIN-*ch*scsc;
		glVertex2f(cx,height1+TF_MARGIN);
		glVertex2f(cx,cy);
		glVertex2f(cx+step,cy);
		glVertex2f(cx+step,height1+TF_MARGIN);
		cx += step;
	}
	glEnd();
	}

	//2d hist
	Histogram2D&histogram2D = CT::histogram2D[CT::GetCurDataID()];
	if(histogram2D.Get())
	{
		/*
		if(histogram2D.need_reload_txt)histogram2D.UpdateTexture();

		glColor3d(1,1,0);
		ShaderProgram*sp = CT::view2d_sp;
		sp->Use();
		if(histogram2D.txt2)sp->SetVar("txt",histogram2D.txt2->SetID(1));
		sp->SetVar("min_level",0.0f);
		sp->SetVar("max_level",0.01f);
//		sp->SetVar("max_level",histogram2D.GetMaxChunk()/(128.0f*256.0f));
		sp->SetVar("size",vec2(histogram2D.GetSize().x,histogram2D.GetSize().y));
		sp->SetVar("params",vec4(0,0,0,0));
		DrawFilledRectangle(vec2(11),vec2(150));
		sp->UnUse();
		DrawRectangle(vec2(0),vec2(200));
		*/
		
		unsigned int*hist = histogram2D.Get();
		float step;
		if(CPU_VD::full_data.GetValueFormat()==0)step=scale/(256.0f*128.0f/histogram2D.chunk_size.x);
		if(CPU_VD::full_data.GetValueFormat()==1)step=scale/(256.0f/histogram2D.chunk_size.x);
		float cx = (-center)*scale;
		float cy = height1+TF_MARGIN;
		ivec2 hist_size = histogram2D.GetSize();
		float stepy = -(height1)/float(hist_size.y);


		glBegin(GL_QUADS);
		for(int j=0;j<hist_size.y;j++)
		for(int i=0;i<hist_size.x;i++)
		{

			glColor4d(1,1,0,hist2d_br*hist[i+hist_size.x*j]/float(histogram2D.GetMaxChunk()));
			glVertex2f(cx+step*i, cy+stepy*j);
			glVertex2f(cx+step*(i+1), cy+stepy*j);
			glVertex2f(cx+step*(i+1), cy+stepy*(j+1));
			glVertex2f(cx+step*i, cy+stepy*(j+1));
		}
		glEnd();
		glColor3d(1,1,0);
		DrawRectangle(vec2(cx,TF_MARGIN),vec2(cx+step*hist_size.x,cy));
	

	}

	/// LINES
	/*
	if(check_box[5]->IsChecked())
	{
		glLineWidth(2);
		for(int k=0;k<4;k++)
		{
			if(k==3)glColor3f(0.6f,0.6f,0.6f);else glColor3f(k==0,k==1,k==2);
			glBegin(GL_LINE_STRIP);
			for(int i=0;i<TF_WIDTH;i++)
			{
				int cx = ((i/float(TF_WIDTH))*(win.y-win.x)+win.x-center)*scale;
				glVertex2f(cx,height1*(1-*(&tf[i].x+k))+TF_MARGIN);
			}
			glEnd();
		}
		glLineWidth(1);
	}*/
	///(*tf_quads)
	
	for(int i=0;i<tf_quads->size();i++)
	{
		TF_QUAD qq = (*tf_quads)[i];
		float cx1 = (qq.l1-center)*scale;
		float cx2 = (qq.l2-center)*scale;
		vec2 b1(cx1,(1-qq.max_gm)*height1+TF_MARGIN),b2(cx2,(1-qq.min_gm)*height1+TF_MARGIN);
		int part=-1;
		if(sel_obj == SELECTION_OBJECT_QUAD && sel_id == i)part=sel_part;else
			if(selected_obj == SELECTION_OBJECT_QUAD && selected_id == i)
			part=selected_part;

		glColor4fv(&(qq.color.x));
		DrawFilledRectangle(b1,b2);

		if(selected_obj == SELECTION_OBJECT_QUAD && selected_id == i)
		{
			glColor3f(1,1,1);
		}else
		if(sel_obj == SELECTION_OBJECT_QUAD && sel_id == i)
		{
			glColor4f(1,1,1,0.2f);
			DrawFilledRectangle(b1,b2);
		}else
			glColor3f(0.5,0.5,0.5);
		DrawRectangle(b1,b2);


		float w=1;
		if(part!=-1)glColor3f(1,1,1);
		if(part==0)DrawRectangle(vec2(b1.x,b1.y)-vec2(w),vec2(b1.x,b2.y)+vec2(w));
		if(part==1)DrawRectangle(vec2(b2.x,b1.y)-vec2(w),vec2(b2.x,b2.y)+vec2(w));
		if(part==2)DrawRectangle(vec2(b1.x,b1.y)-vec2(w),vec2(b2.x,b1.y)+vec2(w));
		if(part==4)DrawRectangle(vec2(b1.x,b2.y)-vec2(w),vec2(b2.x,b2.y)+vec2(w));

	}

	/// ISO
	//if(check_box[6]->IsChecked())
	//if(isos_mode)
	{
		
		glBegin(GL_TRIANGLES);
		for(int i=0;i<isos->size();i++)
		{
			float cx = ((*isos)[i].value-center)*scale;
			/*
			glColor3f(1,0,0);
			glVertex2f(cx+TF_MARGIN,TF_MARGIN);
			glVertex2f(cx,0);
			glVertex2f(cx+2*TF_MARGIN,0);
			*/
			//glColor3f(1,1,1);
			glColor3fv(&((*isos)[i].color.x));
			glVertex2f(cx,height1+TF_MARGIN);
			glVertex2f(cx-3,height1+2*TF_MARGIN);
			glVertex2f(cx+3,height1+2*TF_MARGIN);
			glVertex2f(cx,TF_MARGIN);
			glVertex2f(cx-3,0);
			glVertex2f(cx+3,0);
			
		}
		glEnd();
		
		float wwww1=4;
		for(int i=0;i<isos->size();i++)
		{
			if(selected_obj == SELECTION_OBJECT_ISO && selected_id == i)
			{
				glColor3f(1,1,1);
			}else
			if(sel_obj == SELECTION_OBJECT_ISO && sel_id == i)
			{
				glColor3f(1,1,0);
			}else
				glColor3f(0.5,0.5,0.5);

			float cx = ((*isos)[i].value-center)*scale;
			float cy = (1-(*isos)[i].color.w)*height1+TF_MARGIN;
			int part=-1;
			if(sel_obj == SELECTION_OBJECT_ISO && sel_id == i)part=sel_part;else
			if(selected_obj == SELECTION_OBJECT_ISO && selected_id == i)
				part=selected_part;

			if(part)glEnable(GL_LINE_STIPPLE);
			DrawRectangle(vec2(cx-(part==0),TF_MARGIN),vec2(cx+(part==0),TF_MARGIN+height1));
			if(part)glDisable(GL_LINE_STIPPLE);
			DrawRectangle(vec2(cx-wwww1,cy-1-(part==1)),vec2(cx+wwww1,cy+1+(part==1)));
			
			if(part==1 && mouse_left_is_down)DrawLine(vec2(0,cy),vec2(width,cy));

		}
		
		
	}
	///TF_POINTS
	{
		float wwww1=5;
		for(int i=0;i<tf_points->size();i++)
		{
			

			glColor3fv(&((*tf_points)[i].color.x));

			float cx = ((*tf_points)[i].value-center)*scale;
			float cy = (1-(*tf_points)[i].color.w)*height1+TF_MARGIN;
			DrawFilledRectangle(vec2(cx-wwww1,cy-wwww1),vec2(cx+wwww1,cy+wwww1));

			bool sl = ( (selected_obj == SELECTION_OBJECT_TF_POINT && selected_id == i) || (sel_obj == SELECTION_OBJECT_TF_POINT && sel_id == i) );
			
			if(sl)
			{glColor3f(1,1,0.6f);glLineWidth(2);}
			else
				glColor3f(0.6f,0.6f,0.6f);
			DrawRectangle(vec2(cx-wwww1-sl,cy-wwww1-sl),vec2(cx+wwww1+sl,cy+wwww1+sl));
			
			if(sl)
			{glLineWidth(1);}

		}
	}

	/// FRAME
	glColor3f(0.5f,0.5f,0.5f);
	DrawRectangle(vec2(0,TF_MARGIN),vec2(width,TF_MARGIN+height1));

	glBegin(GL_LINES);
	glColor3f(0.4f,0.4f,0.4f);
	float hh=6;

	/*
	int grid_div = 256*128;
	float cx = (-center*scale);
	int steps = int(grid_div*cx/(scale));
	cx -= steps*scale/grid_div;
	//steps *= grid_div;

	int i1 = grid_div*width/(scale);
	for(int i=0;i<i1;i++)
	{
		int aaa=abs(steps);
		float hhh = 4+4*(aaa%10==0);
		if(aaa%100==0)glColor3f(1,1,1);
		glVertex2f(cx,height1+TF_MARGIN);
		glVertex2f(cx,height1+TF_MARGIN+hhh);
		glVertex2f(cx,TF_MARGIN);
		glVertex2f(cx,TF_MARGIN-hhh);
		cx += scale/grid_div;
		if(aaa%100==0)glColor3f(0.4f,0.4f,0.4f);
		steps--;
	}
	*/
	
	float cx = (-center)*scale;
	float step;
	if(CPU_VD::full_data.GetValueFormat()==0) step = scale/(256.0f*128.0f);
	if(CPU_VD::full_data.GetValueFormat()==1) step = scale/(256.0f);
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
				glVertex2f(cx,height1+TF_MARGIN);
				glVertex2f(cx,height1+TF_MARGIN+hhh);
				glVertex2f(cx,TF_MARGIN);
				glVertex2f(cx,TF_MARGIN-hhh);
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

	///LEVELS

	if(selected_obj == SELECTION_OBJECT_LEVEL)
	{
		glColor3f(1,1,1);
	}else
	if(sel_obj == SELECTION_OBJECT_LEVEL)
	{
		glColor3f(1,1,0);
	}else
		glColor3f(0.5,0.5,0.5);
	int part=-1;
	if(sel_obj == SELECTION_OBJECT_LEVEL)part=sel_id;else
	if(selected_obj == SELECTION_OBJECT_LEVEL)
		part=selected_id;


	vec2 w = (CT::iso->GetWindow()-vec2(center))*scale;
	DrawRectangle(vec2(w.x,TF_MARGIN),vec2(w.y,TF_MARGIN+height1));
	if(part!=-1)
	{
		glColor3f(1,1,1);
		DrawRectangle(vec2((part?w.y:w.x)-1,TF_MARGIN),vec2((part?w.y:w.x)+1,TF_MARGIN+height1));
	}
	
//	glColor4f(1,1,1,0.4f);
//	DrawFilledRectangle(vec2(0,0),vec2(w.x,height));
//	DrawFilledRectangle(vec2(w.y,0),vec2(width,height));

	//DrawText("DR SD",vec2(12,32),vec4(0.6f));


    glFlush();
    SwapBuffers();

}