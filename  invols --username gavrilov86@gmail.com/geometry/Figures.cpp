
#include "Figures.h"
#include "AllDef.h"
#include <math.h>

void AddQuad(Geometry*g, vec3 a,vec3 b,vec3 c,vec3 d)
{
	
	g->AddTriangle(a,b,c);
	g->AddTriangle(a,c,d);
	

}

void AddBox(Geometry*g, vec3 a,vec3 b)
{
	Geometry tmp;
	if(a.x>b.x)swap(a.x,b.x);
	if(a.y>b.y)swap(a.y,b.y);
	if(a.z>b.z)swap(a.z,b.z);

	vec3 s = b - a;
	vec3 a_x(s.x,0,0);
	vec3 a_y(0,s.y,0);
	vec3 a_z(0,0,s.z);

	AddQuad(&tmp,a,a+a_y,a+a_x+a_y,a+a_x);
	AddQuad(&tmp,a,a+a_x,a+a_x+a_z,a+a_z);
	AddQuad(&tmp,a,a+a_z,a+a_z+a_y,a+a_y);

	AddQuad(&tmp,b,b-a_x,b-a_x-a_y,b-a_y);
	AddQuad(&tmp,b,b-a_z,b-a_x-a_z,b-a_x);
	AddQuad(&tmp,b,b-a_y,b-a_z-a_y,b-a_z);

	tmp.SetNormOutOf((a+b)*0.5f);
	g->Add(tmp);

}


void AddBox(Geometry*g, vec3 a,vec3 b,ivec3 br)
{
	Geometry tmp;
	if(a.x>b.x)swap(a.x,b.x);
	if(a.y>b.y)swap(a.y,b.y);
	if(a.z>b.z)swap(a.z,b.z);

	vec3 s = b - a;
	vec3 a_x(s.x/br.x,0,0);
	vec3 a_y(0,s.y/br.x,0);
	vec3 a_z(0,0,s.z/br.x);
	/*

	for(int i=0;i<=br.x;i++)
	for(int j=0;j<=br.x;j++)
	{
		AddQuad(&tmp,a,a+a_y,a+a_x+a_y,a+a_x);
		AddQuad(&tmp,b,b-a_x,b-a_x-a_y,b-a_y);

		AddQuad(&tmp,a,a+a_x,a+a_x+a_z,a+a_z);
		AddQuad(&tmp,b,b-a_z,b-a_x-a_z,b-a_x);
		
		AddQuad(&tmp,b,b-a_y,b-a_z-a_y,b-a_z);
		AddQuad(&tmp,a,a+a_z,a+a_z+a_y,a+a_y);
	}*/

	tmp.SetNormOutOf((a+b)*0.5f);
	g->Add(tmp);

}

void AddCylinder(Geometry*g,float h,float r, int sides)
{
	Geometry tmp;
	float ang=(float)(PI*2.0/sides);
	float sina=sin(ang),cosa=cos(ang);
	int i;
	vec3 t1(r,0,0),t0(r,0,0),hh(0,h,0),hh0(0,0,0);
	for(i=0;i<sides;i++)
	{
		t1.RotateOij(sina,cosa,0,2);
		AddQuad(&tmp,t0,t1,t1+hh,t0+hh);
		tmp.AddTriangle(t0,t1,hh0);
		tmp.AddTriangle(t1+hh,t0+hh,hh);
		t0=t1;
	}
	
	tmp.SetNormOutOf(vec3(0,h/2,0));

	g->Add(tmp);
	
}
void AddSphere(Geometry*g,float r, int sides_a, int sides_b)
{
	Geometry tmp;
	float ang=(float)(PI/sides_a);
	float sina=sin(ang),cosa=cos(ang);

	float ang2=(float)(PI*2.0/sides_b);
	float sinb=sin(ang2),cosb=cos(ang2);
	int i,j;
	vec3 v0(0,r,0),u0,u1,w0,w1;
	vec3 v1(v0);
	for(i=0;i<sides_a;i++)
	{
		v1.RotateOij(sina,cosa,0,1);
	
		u0 = v0;
		w0 = v1;
		u1 = v0;
		w1 = v1;
		for(j=0;j<sides_b;j++)
		{
			u1.RotateOij(sinb,cosb,0,2);
			w1.RotateOij(sinb,cosb,0,2);

			if(i==sides_b-1)
				tmp.AddTriangle(u0,u1,w0);
			else
			if(!i)
				tmp.AddTriangle(u1,w1,w0);
			else
				AddQuad(&tmp,u0,u1,w1,w0);

			u0=u1;
			w0=w1;
		}
		
		
		
		v0=v1;
	}
	
	tmp.SetNormOutOf(vec3(0,0,0));

	g->Add(tmp);
	

}
void AddArmDetail(Geometry*g,float w,float r, int sides)
{
	Geometry tmp;
	float ang=(float)(PI/sides);
	float sina=sin(ang),cosa=cos(ang);
	int i;
	vec3 t1(0,0,r),t0(0,0,r),hh(w/2,0,0),hh0(0,0,0);
	for(i=0;i<sides;i++)
	{
		t1.RotateOij(sina,cosa,1,2);
		AddQuad(&tmp,t0,t1,t1+hh,t0+hh);
		tmp.AddTriangle(t0,t1,hh0);
		tmp.AddTriangle(t1+hh,t0+hh,hh);
		t0=t1;
	}
	
	tmp.SetNormOutOf(vec3((w)/4,0,0));
	g->Add(tmp);

	//tmp.renull();
	//AddQuad(&tmp,);
	AddBox(g,vec3(0,0,r),vec3(w/2,r,-r));
	AddBox(g,vec3(-w/2,2*r,r),vec3(w/2,r,-r));
	

}