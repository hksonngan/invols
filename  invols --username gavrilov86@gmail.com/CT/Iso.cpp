#include "Iso.h"
#include "Shader.h"
#include "AllDef.h"
#include "Draw.h"
#include "CT.h"
#include <windows.h>
#include "vec.h"
#include "str.h"

void IsoViewer::defApply()
{
	if(ps)delete ps;
	svec vt;
	str::StringPairs repl;
	str::AddPair(repl,"$IsoNum",str::ToString((int)CT::isos.size()));
	str::AddPair(repl,"$use_accel_struct",use_accel_struct?"1":"0");
	str::AddPair(repl,"$draw_fog",draw_fog?"1":"0");

	str::ReplaceInFile("srf.fs","_srf.fs",repl);

	ps = new ShaderProgram("srf.vs", "_srf.fs");

	txt_color = new SimText3D(2,TF_WIDTH,1,1,4,0,3,true,GL_FLOAT);

	UpdateUniforms();
}

IsoViewer::IsoViewer():l1(0),b1(0),b2(1),st(0),st_i(0),qual(0.001f),scale(1),ps(0),LightDir(1,0,0)
{
	use_accel_struct=0;
	draw_fog = 1;

	draw_frame_is=1;
	
	defApply();
	SetAnag(0,0);
	SetBoundingBox(b1,b2);
	SetLevel(l1);
	SetBrCont(vec2(0.1,0.4));


	
}

void IsoViewer::UpdateUniforms()
{
	ps->Use();

	if(st)
	{
		ps->SetVar("f_text",st->GetTexture());
		ps->SetVar("f_text_i",st_i->GetTexture());
	}
	ps->SetVar("RFrom",anag[0]);
	ps->SetVar("GFrom",anag[1]);
	ps->SetVar("BFrom",anag[2]);

	ps->SetVar("step_length",b1.length(b2)*qual);
	ps->SetVar("pos",CT::cam.GetPosition());
	ps->SetVar("nav",CT::cam.GetNav());

	ps->SetVar("c_br",c_br);
	ps->SetVar("c_contr",c_contr);
	ps->SetVar("scale",scale);
	ps->SetVar("box1",b1*scale);
	ps->SetVar("box2",b2*scale);
	ps->SetVar("level1", l1);
	ps->SetVar("level_color1",GetLevelColor(GetMinLevel()));
	ps->SetVar("txt_level_color",txt_color->GetTexture());
	ps->SetVar("LightDir",LightDir);
	ApplyColorTable();

	if(CT::isos.size())
	{
		float ll[MAX_ISO_NUM];
		vec4 cc[MAX_ISO_NUM];
		for(int j=0;j<CT::isos.size();j++)
		{
			ll[j]=CT::isos[j].value;
			cc[j]=CT::isos[j].color;
		}

		ps->SetVar("level",CT::isos.size(),ll);
		ps->SetVar("my_color",CT::isos.size(),cc);
	}
	ps->UnUse();
}

float IsoViewer::GetFlag()
{
	return fl;
}
void IsoViewer::SetFlag(float f)
{
	fl=f;
	
	ps->Use();
	ps->SetVar("flag",fl);
	
	ps->UnUse();
}

void IsoViewer::SetLevel(float nl1)
{
	l1=clamp(0.0f,1.0f,nl1);

	CT::cur_level = nl1;
	ps->Use();
	ps->SetVar("level1", l1);
	ps->SetVar("level_color1",GetLevelColor(GetMinLevel()));
	ps->UnUse();
}
void IsoViewer::SetBoundingBox(vec3 a,vec3 b)
{
	
	b1=vec3::Clamp(0,1,a);
	b2=vec3::Clamp(0,1,b);

	ps->Use();
	ps->SetVar("box1",b1*scale);
	ps->SetVar("box2",b2*scale);
	ps->SetVar("step_length",b1.length(b2)*qual);
	ps->UnUse();
}
void IsoViewer::ApplyCamera(Camera* c)
{
	ps->Use();
	ps->SetVar("pos",c->GetPosition());
	ps->SetVar("nav",c->GetNav());
	ps->UnUse();
}
vec2 IsoViewer::GetBrCont()
{
	return vec2(c_br,c_contr);
}
void IsoViewer::SetBrCont(vec2 bc)
{
	c_br = clamp(-10.0f,10.0f,bc.x);
	c_contr = clamp(0.0f,100.0f,bc.y);
	ps->Use();
	ps->SetVar("c_br",c_br);
	ps->SetVar("c_contr",c_contr);
	ps->UnUse();
}
void IsoViewer::SetQuality(float q)
{
	qual = q;
	SetBoundingBox(b1,b2);
}
float IsoViewer::GetQuality()
{
	return qual;
}
void IsoViewer::ApplyColorTable()
{
	txt_color->Upload(&CT::color_table->x);
}

void DrawGrid(vec3 a,vec3 b,float sc);

void IsoViewer::Draw(Camera* c)
{
	
	scale = CT::ct_data.scale;
	vec3 bb1=b1*scale,bb2=b2*scale;

	if(draw_frame_is)
	{
		DrawGrid(bb1,bb2,0.042f);
	}

	ps->Use();
	ps->SetVar("scale",scale);
	ps->SetVar("box1",b1*scale);
	ps->SetVar("box2",b2*scale);


	vec3 pp = CT::cam.GetPosition()+CT::cam.GetNav()*0.4f, ll = CT::cam.GetLeft()*0.17f *CT::width/(CT::height+0.1f), tt = CT::cam.GetTop()*0.17f;
	DrawQuad(pp+ll+tt,pp-ll+tt,pp-ll-tt,pp+ll-tt);
	//DrawCube(bb1,bb2);
	//DrawSectOfCube(b1,b2,c->GetPosition()+c->GetNav(),c->GetNav());
	ps->UnUse();


}
IsoViewer::~IsoViewer()
{
	delete ps;
	if(st)delete st;
	if(st_i)delete st_i;
	delete txt_color;

}
#define CELL_SIZE 16

template<class T>
void MakeBoundingCells(T*data,ivec3 size,T*c,ivec3 size_b)
{
	int id=3;
	//int sz = (size.x*size.y*size.z)/(CELL_SIZE*CELL_SIZE*CELL_SIZE);
	
//	memset(c,0,sz*sizeof(float));
	

	for(int i=0;i<size_b.x;i++)
	for(int j=0;j<size_b.y;j++)
	for(int k=0;k<size_b.z;k++)
	{
		T max;
		bool bbb=1;
		for(int ii=i*size.x/(float)size_b.x-1;ii<(i+1)*size.x/(float)size_b.x+1;ii++)
		for(int jj=j*size.y/(float)size_b.y-1;jj<(j+1)*size.y/(float)size_b.y+1;jj++)
		for(int kk=k*size.z/(float)size_b.z-1;kk<(k+1)*size.z/(float)size_b.z+1;kk++)
			if( ivec3(ii,jj,kk).InBounds(ivec3(0),size) )
			{
				T vl = data[(ii + size.x*(jj + size.y*kk))];
				if(vl > max || bbb) {bbb=0;max=vl;}
			}

		id=(i+(size_b.x)*(j+k*(size_b.y)));
		c[id] = max;
		
		
//		id+=3;
	}
}
/*
void MakeNormals(BYTE*data,ivec3 size,float*n)
{
	for(int i=0;i<size.x;i++)
	for(int j=0;j<size.y;j++)
	for(int k=0;k<size.z;k++)
	{
		int id = i+size.x*(j+size.y*k);
		int vl = (int)data[id];
		if(i && i!=size.x-1)
			n[id*3]=(int)data[i+1+size.x*(j+size.y*k)]-(int)data[i-1+size.x*(j+size.y*k)];
		else
		{
		if(!i)
			n[id*3]=(int)data[i+1+size.x*(j+size.y*k)]-vl;
		else
			n[id*3]=vl-(int)data[i-1+size.x*(j+size.y*k)];
		}

		if(j && j!=size.y-1)
			n[id*3+1]=(int)data[i+size.x*(j+1+size.y*k)]-(int)data[i+size.x*(j-1+size.y*k)];
		else
		{
		if(!j)
			n[id*3+1]=(int)data[i+size.x*(j+1+size.y*k)]-vl;
		else
			n[id*3+1]=vl-(int)data[i+size.x*(j-1+size.y*k)];
		}

		if(k && k!=size.z-1)
			n[id*3+2]=(int)data[i+size.x*(j+size.y*(k+1))]-(int)data[i+size.x*(j+size.y*(k-1))];
		else
		{
		if(!k)
			n[id*3+2]=(int)data[i+size.x*(j+size.y*(k+1))]-vl;
		else
			n[id*3+2]=vl-(int)data[i+size.x*(j+size.y*(k-1))];
		}
	}
}
*/
void IsoViewer::UploadFieldData(void*data,ivec3 size,int data_type)//data_type: 0-BYTE 1-float 2-short
{
	if(st)delete st;
	DWORD tp;
	if(data_type==0) tp = GL_UNSIGNED_BYTE;
	if(data_type==1) tp = GL_FLOAT;
	//if(data_type==2) tp = GL_UNSIGNED_SHORT;
	if(data_type==2) tp = GL_SHORT;

	st = new SimText3D(3,size.x,size.y,size.z,1,data,1,true,tp);
	
	ivec3 isz = size/CELL_SIZE;
	
	isz.x += !!(size.x%CELL_SIZE);
	isz.y += !!(size.y%CELL_SIZE);
	isz.z += !!(size.z%CELL_SIZE);

	int sz = isz.x*isz.y*isz.z;
	void *dt2;
	dt2 = new BYTE[sz];
	//MakeBoundingCells<BYTE>((BYTE*)data,size,(BYTE*)dt2,isz);
	
	if(st_i)delete st_i;
	st_i = new SimText3D(3,isz.x,isz.y,isz.z,1,dt2,2,false,GL_UNSIGNED_BYTE);
	delete[]dt2;


	ps->Use();
	ps->SetVar("f_text",st->GetTexture());
	ps->SetVar("f_text_i",st_i->GetTexture());

	//ps->SetVar("f_text_n",st_n->GetTexture());
	ps->UnUse();


}


vec3 IsoViewer::GetLevelColor(float level)
{
	return CT::GetColor1();
}


void IsoViewer::SetAnag(int s, bool left)
{
	
	switch(s)
	{
	case 0: // no stereo
		anag[0].set(1,0,0);
		anag[1].set(0,1,0);
		anag[2].set(0,0,1);
		break;
	case 1: // True anaglyh
		if(left)
		{
			anag[0].set(0.299,0.587,0.114);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,0,0);
			anag[2].set(0.299,0.587,0.114);
		}
		break;
	case 2: // Gray anaglyh
		if(left)
		{
			anag[0].set(0.299,0.587,0.114);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0.299,0.587,0.114);
			anag[2].set(0.299,0.587,0.114);
		}
		break;
	case 3: // Color anaglyh
		if(left)
		{
			anag[0].set(1,0,0);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,1,0);
			anag[2].set(0,0,1);
		}
		break;	
	case 4: // Half-Color anaglyh
		if(left)
		{
			anag[0].set(0.299,0.587,0.114);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,1,0);
			anag[2].set(0,0,1);
		}
		break;	
	case 5: // Optimized anaglyh
		if(left)
		{
			anag[0].set(0,0.7,0.3);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,1,0);
			anag[2].set(0,0,1);
		}
		break;	
	};

	ps->Use();
	ps->SetVar("RFrom",anag[0]);
	ps->SetVar("GFrom",anag[1]);
	ps->SetVar("BFrom",anag[2]);
	ps->UnUse();
}
vec3 IsoViewer::GetLightDir()
{
	return LightDir;
}
void IsoViewer::SetLightDir(vec3 _LightDir)
{
	LightDir = _LightDir;
	ps->Use();
	ps->SetVar("LightDir",LightDir);
	ps->UnUse();
}
/*
vec3 IsoViewer::GetScale()
{
	return scale;
}
void IsoViewer::SetScale(vec3 _scale)
{
	scale = _scale;
	ps->Use();
	ps->SetVar("scale",scale);
	ps->UnUse();
}
&*/