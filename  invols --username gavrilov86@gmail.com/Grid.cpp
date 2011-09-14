#include "AllDef.h"
#include "Draw.h"
#include "Draw2D.h"
#include "CT.h"
#include "vec.h"
#include "DicomReader.h"
#include "Camera.h"

void DrawGrid(vec3 a,vec3 b,float step)
{
	
//	glColor3f(1,1,0.8f);

	//DrawCubeFrame(a,b);
	float h1=0.006f,h2=0.01f;

/*
	glEnable(GL_NORMALIZE);
	glDisable(GL_CULL_FACE);

	DrawCube1(vec3(b.x,a.y,a.z),a-vec3(h1));
	DrawCube1(vec3(a.x,b.y,a.z),a-vec3(h1));
	DrawCube1(vec3(a.x,a.y,b.z),a-vec3(h1));
	DrawCube1(b+vec3(h1),vec3(a.x,b.y,b.z));
	DrawCube1(b+vec3(h1),vec3(b.x,a.y,b.z));
	DrawCube1(b+vec3(h1),vec3(b.x,b.y,a.z));

	DrawCube1(vec3(b.x,a.y,a.z),vec3(b.x+h1,	b.y,	a.z-h1));
	DrawCube1(vec3(b.x,a.y,a.z),vec3(b.x+h1,	a.y-h1,	b.z));
	DrawCube1(vec3(a.x,b.y,a.z),vec3(b.x,	b.y+h1,	a.z-h1));
	DrawCube1(vec3(a.x,b.y,a.z),vec3(a.x-h1,	b.y+h1,	b.z));
	DrawCube1(vec3(a.x,a.y,b.z),vec3(b.x,	a.y-h1,	b.z+h1));
	DrawCube1(vec3(a.x,a.y,b.z),vec3(a.x-h1,	b.y,	b.z+h1));
*/
	glLineWidth(2);

	glColor3f(0.7f,0.7f,0.5f);
	DrawLine(a,vec3(b.x,a.y,a.z));
	DrawLine(a,vec3(a.x,b.y,a.z));
	DrawLine(a,vec3(a.x,a.y,b.z));
	DrawLine(b,vec3(a.x,b.y,b.z));
	DrawLine(b,vec3(b.x,a.y,b.z));
	DrawLine(b,vec3(b.x,b.y,a.z));

	DrawLine(vec3(b.x,a.y,a.z),vec3(b.x,	b.y,	a.z));
	DrawLine(vec3(b.x,a.y,a.z),vec3(b.x,	a.y,	b.z));
	DrawLine(vec3(a.x,b.y,a.z),vec3(b.x,	b.y,	a.z));
	DrawLine(vec3(a.x,b.y,a.z),vec3(a.x,	b.y,	b.z));
	DrawLine(vec3(a.x,a.y,b.z),vec3(b.x,	a.y,	b.z));
	DrawLine(vec3(a.x,a.y,b.z),vec3(a.x,	b.y,	b.z));

	glLineWidth(1);
	vec3 bb[2];
	bb[0] = a;
	bb[1] = b;

	glEnable(GL_NORMALIZE);

	for(int i=0;i<2;i++)
		for(int j=0;j<2;j++)
			for(int k=0;k<2;k++)
			{
				DrawSphere(vec3(bb[i].x,bb[j].y,bb[k].z),h2);
			}

	glDisable(GL_NORMALIZE);
//	glEnable(GL_CULL_FACE);


	if(CT::cam.GetProjection())
	{
		glBegin(GL_LINES);
		int id = (int(a.x/step)+1);
		for(;id<int(b.x/step);id++)
		{
			float cur = id*step;
			
			float hh = 0.01f*(1 + ((id%5)==0));

			glVertex3f(cur,a.y,   a.z);
			glVertex3f(cur,a.y+hh,a.z);
			glVertex3f(cur,a.y,   a.z);
			glVertex3f(cur,a.y,   a.z+hh);
			glVertex3f(cur,b.y,   a.z);
			glVertex3f(cur,b.y-hh,a.z);
			glVertex3f(cur,b.y,   a.z);
			glVertex3f(cur,b.y,   a.z+hh);
			glVertex3f(cur,b.y,   b.z);
			glVertex3f(cur,b.y-hh,b.z);
			glVertex3f(cur,b.y,   b.z);
			glVertex3f(cur,b.y,   b.z-hh);
			glVertex3f(cur,a.y,   b.z);
			glVertex3f(cur,a.y+hh,b.z);
			glVertex3f(cur,a.y,   b.z);
			glVertex3f(cur,a.y,   b.z-hh);

		}
		id = (int(a.y/step)+1);
		for(;id<int(b.y/step);id++)
		{
			float cur = id*step;
			
			float hh = 0.007f*(1 + ((id%5)==0));

			glVertex3f(a.x,cur,   a.z);
			glVertex3f(a.x+hh,cur,a.z);
			glVertex3f(a.x,cur,   a.z);
			glVertex3f(a.x,cur,   a.z+hh);
			glVertex3f(b.x,cur,   a.z);
			glVertex3f(b.x-hh,cur,a.z);
			glVertex3f(b.x,cur,   a.z);
			glVertex3f(b.x,cur,   a.z+hh);
			glVertex3f(b.x,cur,   b.z);
			glVertex3f(b.x-hh,cur,b.z);
			glVertex3f(b.x,cur,   b.z);
			glVertex3f(b.x,cur,   b.z-hh);
			glVertex3f(a.x,cur,   b.z);
			glVertex3f(a.x+hh,cur,b.z);
			glVertex3f(a.x,cur,   b.z);
			glVertex3f(a.x,cur,   b.z-hh);

		}
		id = (int(a.z/step)+1);
		for(;id<int(b.z/step);id++)
		{
			float cur = id*step;
			
			float hh = 0.007f*(1 + ((id%5)==0));


			glVertex3f(a.x,   a.y,cur);
			glVertex3f(a.x+hh,a.y,cur);
			glVertex3f(a.x,   a.y,cur);
			glVertex3f(a.x,   a.y+hh,cur);
			glVertex3f(b.x,   a.y,cur);
			glVertex3f(b.x-hh,a.y,cur);
			glVertex3f(b.x,   a.y,cur);
			glVertex3f(b.x,   a.y+hh,cur);
			glVertex3f(b.x,   b.y,cur);
			glVertex3f(b.x-hh,b.y,cur);
			glVertex3f(b.x,   b.y,cur);
			glVertex3f(b.x,   b.y-hh,cur);
			glVertex3f(a.x,   b.y,cur);
			glVertex3f(a.x+hh,b.y,cur);
			glVertex3f(a.x,   b.y,cur);
			glVertex3f(a.x,   b.y-hh,cur);

		}

		glEnd();
	}else
	{
		
		glPushMatrix();
		glLoadIdentity();
		//glDisable(GL_DEPTH_TEST);
/*		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(-dist*CameraZoom2D*_width/(double)_height,dist*CameraZoom2D*_width/(double)_height,-CameraZoom2D*dist,CameraZoom2D*dist);
		glMatrixMode(GL_MODELVIEW);
*/		glDisable(GL_LIGHTING);
		float dist = CT::cam.GetDistance();
		float gs = 10/(DicomReader::GetScale());
		
		
		DrawGrid(vec2(-float(CT::width)/CT::height,-1)*(dist*CT::cam.CameraZoom2D),vec2(float(CT::width)/CT::height,1)*(dist*CT::cam.CameraZoom2D),gs,vec4(0.3),vec4(0.6));


		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glPopMatrix();
		
	}
}

void Draw2DInfo()
{
	
}