#include "Render.h"
#include "Shader.h"
#include "AllDef.h"
#include "Draw.h"
#include "CT.h"
#include "Camera.h"
///#include <windows.h>
#include "vec.h"
#include "str.h"
#include "CPU_VD.h"
#include "RenderToTexture.h"
#include "seg_points.h"



#define VD_TXT_ID 1
#define TF_TXT_ID (VD_TXT_ID+MAX_VD_NUMBER)
#define I_TXT_ID (TF_TXT_ID+1)
#define FRONT_FACE_TXT_ID (I_TXT_ID+1)
#define BACK_FACE_TXT_ID (I_TXT_ID+2)



RenderingMethod::RenderingMethod()
{
	need_reload_shader=need_update_uniforms=need_reload_color_table=1;

	drop_shadows=0;
	use_cubic_filt=0;
	shade_mode=tf_type=0;

	sp=0;
	txt_color = 0;
	isos_sum=0;
	quads_sum=0;
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		min_level[i]=0.01;
		max_level[i]=0.05;
		opacity[i] = 0.5;
	}

}
RenderingMethod::~RenderingMethod()
{
	if(sp)delete sp;
	if(txt_color)delete txt_color;

}
void RenderingMethod::DeletePoint(int id)
{
	std::vector<TF_POINT>* isos1 = &(tf_points[CT::GetCurDataID()]);
	if(id<isos1->size())
	{
		isos1->erase(isos1->begin()+id);
		ApplyPoints();
	}
}
void RenderingMethod::DeleteIso(int id)
{
	std::vector<IsoSurface>* isos1 = &(isos[CT::GetCurDataID()]);
	if(id<isos1->size())
	{
		isos1->erase(isos1->begin()+id);
		ApplyPoints();
		need_reload_shader=1;
	}
}
void RenderingMethod::DeleteQuad(int id)
{
	std::vector<TF_QUAD>* tf_quads1 = &(tf_quads[CT::GetCurDataID()]);
	if(id<tf_quads1->size())
	{
		tf_quads1->erase(tf_quads1->begin()+id);
		ApplyQuads();
		need_reload_shader=1;
	}
}
bool RenderingMethod::AddIso(vec2 pos)
{
	std::vector<IsoSurface>* isos1 = &(isos[CT::GetCurDataID()]);

	if(isos1->size()<MAX_ISO_NUM)
	{
		IsoSurface iso;
		iso.color = vec4(RND01,RND01,RND01,pos.y);
		iso.value = pos.x;

		isos1->push_back(iso);
		need_reload_shader=1;
		return 1;
	}
	return 0;
}
bool RenderingMethod::AddPoint(vec2 pos)
{
	std::vector<TF_POINT>* isos1 = &(tf_points[CT::GetCurDataID()]);

	if(isos1->size()<MAX_POINTS_NUM)
	{
		TF_POINT iso;
		iso.color = vec4(RND01,RND01,RND01,pos.y);
		iso.value = pos.x;
		isos1->push_back(iso);
		ApplyPoints();
		return 1;
	}
	return 0;
}
bool RenderingMethod::AddQuad(vec2 pos)
{
	std::vector<TF_QUAD>* tf_quads1 = &(tf_quads[CT::GetCurDataID()]);

	if(tf_quads1->size()<MAX_QUAD_NUM)
	{
		TF_QUAD qq;
		qq.color = vec4(RND01,RND01,RND01,1);
		qq.l1 = qq.l2 = pos.x;
		qq.min_gm = 0.1f;
		qq.max_gm = pos.y;

		tf_quads1->push_back(qq);
		need_reload_shader=1;
		ApplyQuads();
		return 1;
	}
	return 0;
}
void PutLine(float x1,float x2,vec4 col1,vec4 col2,vec2 ww,vec4*tf)
{
	//vec2 ww = CT::iso->GetWindow();
	int cx1 = TF_WIDTH*(x1-ww.x)/(ww.y-ww.x);
	int cx2 = TF_WIDTH*(x2-ww.x)/(ww.y-ww.x);
	if(cx1<0)cx1=0;
	if(cx2<0)cx2=0;
	if(cx1>=TF_WIDTH)cx1=TF_WIDTH-1;
	if(cx2>=TF_WIDTH)cx2=TF_WIDTH-1;
	if(col1==col2)
	{
		for(int j=cx1;j<=cx2;j++)
			tf[j] = col1;
	}else
	{
		for(int j=cx1;j<=cx2;j++)
		{
			float al = (j-cx1)/float(cx2-cx1);
			tf[j] = col2*al + col1*(1-al);
		}
	}
}

void RenderingMethod::ApplyQuads(int vd_id)
{/*
	
	vec4* tf = color_table + vd_id*TF_WIDTH;
	std::vector<TF_QUAD>*tf_quads1 = &tf_quads[vd_id];

	vec2 ww = vec2(1,0);
	for(int i=0;i<tf_quads1->size();i++)
	{
		ww.x = min(ww.x,(*tf_quads1)[i].l1);
		ww.y = max(ww.y,(*tf_quads1)[i].l2);
	}
	tf_ww[vd_id] = ww;

	memset(tf,0,sizeof(float)*4*TF_WIDTH);
	for(int i=0;i<tf_quads1->size();i++)
	//for(int j=(*tf_quads1)[i].l1;j<=(*tf_quads1)[i].l2;j++)
	{
		vec4 cc = (*tf_quads1)[i].color;
		cc.w = (*tf_quads1)[i].max_gm;
		PutLine((*tf_quads1)[i].l1,(*tf_quads1)[i].l2,cc,cc,ww,tf);
	}

	need_reload_color_table = 1;
	*/
}

void RenderingMethod::ApplyPoints(int vd_id)
{
	vec4* tf = color_table + vd_id*TF_WIDTH;
	std::vector<TF_POINT> *tf_isos1 = &tf_points[vd_id];

	vec2 ww = vec2(1,0);
	for(int i=0;i<tf_isos1->size();i++)
	{
		float val = (*tf_isos1)[i].value;
		ww.x = min(ww.x,val);
		ww.y = max(ww.y,val);
	}
	tf_ww[vd_id] = ww;
	int iso_id[MAX_POINTS_NUM];
	memset(tf,0,sizeof(float)*4*TF_WIDTH);
	if(tf_isos1->size()>1)
	{
		//sort
		for(int i=0;i<tf_isos1->size();i++)
			iso_id[i]=i;
		for(int i=0;i<tf_isos1->size()-1;i++)
			for(int j=i+1;j<tf_isos1->size();j++)
				if((*tf_isos1)[iso_id[i]].value > (*tf_isos1)[iso_id[j]].value)
				{
					swap(iso_id[i],iso_id[j]);
				}

		//fill
		for(int i=1;i<tf_isos1->size();i++)
		{
			PutLine((*tf_isos1)[iso_id[i-1]].value, (*tf_isos1)[iso_id[i]].value, (*tf_isos1)[iso_id[i-1]].color,(*tf_isos1)[iso_id[i]].color,ww,tf);
		}

	}
	if(tf_isos1->size()==1)
	{
		tf_ww[vd_id].set(0,1);
		for(int i=0;i<TF_WIDTH;i++)
			tf[i] = (*tf_isos1)[0].color;
	}

	tf[1]=tf[0];
	tf[TF_WIDTH-2]=tf[TF_WIDTH-1];

	CT::need_rerender=1;
	need_reload_color_table = 1;
}
void RenderingMethod::ApplyPoints()
{
	ApplyPoints(CT::GetCurDataID());
}
void RenderingMethod::ApplyQuads()
{
	ApplyQuads(CT::GetCurDataID());
}

void Render::ReLoadShader()
{
	for(int id=0;id<rendering_methods.size();id++)
		rendering_methods[id]->need_reload_shader=1;
	CT::need_rerender=1;
//ReLoadCurShader();
	
}

void Render::ReLoadCurShader()
{
	rendering_methods[CurRM]->need_reload_shader=1;
	CT::need_rerender=1;
	//_ReLoadShader();

	
}
void Render::_ReLoadShader()
{
	
	RenderingMethod*rm = rendering_methods[CurRM];
	rm->isos_sum=0;
	rm->quads_sum=0;
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		rm->isos_sum += (int)rm->isos[i].size();
		rm->iso_last[i] = rm->isos_sum-1;

		rm->quads_sum += (int)rm->tf_quads[i].size();
		rm->quad_last[i] = rm->quads_sum-1;
	}

	rm->need_reload_shader=0;
	
	svec vt;
	
	str::StringPairs repl;
	str::AddPair(repl,"$IsPerspective",str::ToString((int)CT::cam.GetProjection()));
	
	str::AddPair(repl,"$IsoNum",str::ToString(rm->isos_sum));
	str::AddPair(repl,"$QuadNum",str::ToString(rm->quads_sum));
	str::AddPair(repl,"$VD_NUMBER",str::ToString(CT::GetVolumesNumber()));
	//str::AddPair(repl,"$VD_NUMBER","1");
	str::AddPair(repl,"$use_accel_struct",rm->use_accel_struct?"1":"0");
	str::AddPair(repl,"$use_cubic_filt",rm->use_cubic_filt?"1":"0");
	str::AddPair(repl,"$drop_shadows",rm->drop_shadows?"1":"0");
	str::AddPair(repl,"$shade_mode",str::ToString((int)rm->shade_mode));
	str::AddPair(repl,"$tf_type",str::ToString((int)rm->tf_type));
	str::AddPair(repl,"USE_BOUNDING_MESH",CT::use_bounding_mesh?"1":"0");

	
	
	str::CreateTxtFile("full.fs",str::OpenTxtFile("common_shaders//head.fs")+str::OpenTxtFile(rm->fs_filename.c_str()));
	str::ReplaceInFile("full.fs","current.fs",repl);
	if(rm->sp)delete rm->sp;
	rm->sp = new ShaderProgram("common_shaders//srf.vs", "current.fs");
	
	_UpdateUniforms();
}



Render::Render():b1(0),b2(1),step_length(0.0001f),LightDir(1,0,0)
{
	SetAnag(0,0);
	CurRM=0;
}
void Render::UpdateUniforms()
{
	for(int id=0;id<rendering_methods.size();id++)
		rendering_methods[id]->need_update_uniforms = 1;
	//_UpdateUniforms();
}
void Render::_UpdateUniforms()
{
	RenderingMethod*rm = rendering_methods[CurRM];

	rm->need_update_uniforms = 0;
	
		ShaderProgram*ps = rm->sp;

		ps->Use();
		//if(volume_data)ps->SetVar("f_text",is_small ? volume_data->GetSmallDataTexture() : volume_data->GetDataTexture());
		ps->SetVar("RFrom",anag[0]);
		ps->SetVar("GFrom",anag[1]);
		ps->SetVar("BFrom",anag[2]);
		//ApplyColorTable();


		if(rm->isos_sum)
		{
			float ll[MAX_ISO_NUM];
			vec4 cc[MAX_ISO_NUM];
			int id=0;
			for(int i=0;i<MAX_VD_NUMBER;i++)
				for(int j=0;j<rm->isos[i].size();j++,id++)
				{
					ll[id]=rm->isos[i][j].value;
					cc[id]=rm->isos[i][j].color;
				}

			ps->SetVar("level",rm->isos_sum,ll);
			ps->SetVar("my_color",rm->isos_sum,cc);
		}
		if(rm->quads_sum)
		{
			vec4 ll[MAX_QUAD_NUM];
			vec4 cc[MAX_QUAD_NUM];
			int id=0;
			for(int i=0;i<MAX_VD_NUMBER;i++)
			{
				float mag_scale = CT::histogram2D[i].mag_scale;
				for(int j=0;j<rm->tf_quads[i].size();j++,id++)
				{
					ll[id] = vec4(rm->tf_quads[i][j].l1, rm->tf_quads[i][j].l2, rm->tf_quads[i][j].min_gm*mag_scale,rm->tf_quads[i][j].max_gm*mag_scale);
					cc[id]=rm->tf_quads[i][j].color;
				}
			}

			ps->SetVar("q_level",rm->quads_sum,ll);
			ps->SetVar("q_color",rm->quads_sum,cc);
		}
		ps->UnUse();
	
	CT::need_rerender=1;
}
RenderingMethod* Render::GetCurRM()
{
	return rendering_methods[CurRM];
}
void Render::SetCurRM(int id)
{
	if(id>=0 && id<rendering_methods.size())
	{
		if(CurRM != id)CT::need_rerender=1;
		CurRM = id;
	}
	
}
int Render::GetCurRMID()
{
	return CurRM;
}

void Render::SetOpacity(float val)
{
	GetCurRM()->opacity[CT::GetCurDataID()] = val;
}
float Render::GetOpacity()
{
	return GetCurRM()->opacity[CT::GetCurDataID()];
}
vec4* Render::GetCurTF()
{
	return rendering_methods[CurRM]->color_table+TF_WIDTH*CT::GetCurDataID();
}
void Render::SetBoundingBox(vec3 a,vec3 b)
{
	
	b1=vec3::Clamp(0,1,a);
	b2=vec3::Clamp(0,1,b);

	CT::need_rerender=1;
	CT::need_rerender2d=1;
}
void Render::SetWindow(vec2 w,int id)
{
	if(w.x>w.y)w.x=w.y=(w.x+w.y)*0.5f;
	GetCurRM()->min_level[id] = w.x;
	GetCurRM()->max_level[id] = w.y;
}
void Render::SetWindow(vec2 level)
{
	SetWindow(level,CT::GetCurDataID());
}
vec2 Render::GetWindow(int id)
{
	return vec2(GetCurRM()->min_level[id],GetCurRM()->max_level[id]);
}
vec2 Render::GetWindow()
{
	return GetWindow(CT::GetCurDataID());
}

void Render::SetStepLength(float q)
{
	step_length = q;
	CT::need_rerender=1;
}
float Render::GetStepLength()
{
	return step_length;
}

void Render::ApplyColorTable(vec2 _tf_ww)
{
	GetCurRM()->tf_ww[CT::GetCurDataID()] = _tf_ww;
	CT::need_rerender=1;
	GetCurRM()->need_reload_color_table=1;
}
SimText3D* Render::GetTFTexture()
{
	return GetCurRM()->txt_color;
}

void Render::Draw(bool is_small)
{
	
	if(GetCurRM()->need_reload_shader)
	{
		_ReLoadShader();
	}

	if(GetCurRM()->need_reload_color_table)
	{
		if(GetCurRM()->txt_color)delete GetCurRM()->txt_color;
		GetCurRM()->txt_color = new SimText3D(2,TF_WIDTH,MAX_VD_NUMBER,1,4,&GetCurRM()->color_table->x,0,1,GL_FLOAT);
		//txt_color = new Text(TF_WIDTH,MAX_VD_NUMBER,&CT::color_table->x,4,GL_FLOAT,1);
		GetCurRM()->need_reload_color_table=0;
	}

	vec3 pp,ll,tt;
	float kf = 0.4f;
	pp = CT::cam.GetPosition()+CT::cam.GetNav()*kf;
	if(CT::cam.GetProjection())
	{
		float kff = kf * 0.4142f;
		ll = CT::cam.GetLeft()*kff*CT::width/(CT::height+0.001f);
		tt = CT::cam.GetTop()*kff;
	}else
	{
		ll = CT::cam.GetLeft()*CT::cam.GetDistance()*CT::width/(CT::height+0.1f);
		tt = CT::cam.GetTop()*CT::cam.GetDistance();
	}
//	vec3 bb1=b1*scale,bb2=b2*scale;
	if(GetCurRM()->need_update_uniforms)_UpdateUniforms();
	ShaderProgram*ps = GetCurRM()->sp;


	ps->Use();
vec3 box1 = b1;
vec3 box2 = b2;
	ps->SetVar("pos",CT::cam.GetPosition());
	ps->SetVar("nav",CT::cam.GetNav());
	ps->SetVar("top",CT::cam.GetTop());
	ps->SetVar("left",CT::cam.GetLeft());
	ps->SetVar("screen_width",(float)CT::width);
	ps->SetVar("screen_height",(float)CT::height);
	ps->SetVar("box1",box1);
	ps->SetVar("box2",box2);

	ps->SetVar("LightDir",LightDir);
	ps->SetVar("step_length",step_length*((CT::IsFastView()||CT::is_changing)?CT::interaction_quality:1.0f));


	ps->SetVar("random_seed",(CT::RenderingType==2) ? vec4(RND01,RND01,RND01,RND01)*2000 : vec4(300));
	if(GetCurRM()->txt_color)
	{
		//glBindTexture(GL_TEXTURE_2D,txt_color->GetTexture());
		ps->SetVar("txt_level_color",GetCurRM()->txt_color->SetID(TF_TXT_ID));
	}

	if(changed_anag)
	{
		ps->SetVar("RFrom",anag[0]);
		ps->SetVar("GFrom",anag[1]);
		ps->SetVar("BFrom",anag[2]);
		changed_anag=0;

	}


	bool dr=0;

	for(int i=0;i<MAX_VD_NUMBER;i++)
	if(CT::GetData(i)->IsLoaded())
	{
		dr=1;
		std::string idd = "["+str::ToString(i)+"]";
		ps->SetVar1("min_level"+idd,GetCurRM()->min_level[i]);
		ps->SetVar1("max_level"+idd,GetCurRM()->max_level[i]);
		ps->SetVar1("opacity"+idd,GetCurRM()->opacity[i]);
		ps->SetVar1("IsoLast"+idd,GetCurRM()->iso_last[i]);
		ps->SetVar1("QuadLast"+idd,GetCurRM()->quad_last[i]);
		ps->SetVar1("tf_ww"+idd,GetCurRM()->tf_ww[i]);

		
		CS3 cc11 = volume_transform[i];
		//cc11 = cc11._1();
		
//		cc11.center = volume_transform[i].center;
		ps->SetVar1("cs_center"+idd,cc11.center);

		cc11.x /=cc11.x.lengthSQR();
		cc11.y /=cc11.y.lengthSQR();
		cc11.z /=cc11.z.lengthSQR();

		vec3 vx = vec3(cc11.x.x,cc11.y.x,cc11.z.x);
		vec3 vy = vec3(cc11.x.y,cc11.y.y,cc11.z.y);
		vec3 vz = vec3(cc11.x.z,cc11.y.z,cc11.z.z);
		ps->SetVar1("cs_x"+idd,vx);
		ps->SetVar1("cs_y"+idd,vy);
		ps->SetVar1("cs_z"+idd,vz);

//		ps->SetVar("cs_x"+idd,cc11.x);
//		ps->SetVar("cs_y"+idd,cc11.y);
//		ps->SetVar("cs_z"+idd,cc11.z);

		ivec3 size1 = CT::GetData(i)->GetSize();
		vec3 cell_size(1.0f/size1.x,1.0f/size1.y,1.0f/size1.z);
		ps->SetVar1("cell_size"+idd,cell_size);
		//ps->SetVar1("f_text"+idd,CT::GetData(i)->SetDataTextureID(VD_TXT_ID+i));
		//ps->SetVar1("f_text_tf"+idd,CT::GetTFData(i)->SetDataTextureID(I_TXT_ID+i));
	}
	if(CT::GetData(0)->IsLoaded())
	{
		ps->SetVar1("f_text1",CT::GetData(0)->SetDataTextureID(VD_TXT_ID));
		//ps->SetVar1("f_text_tf",CT::GetTFData(0)->SetDataTextureID(I_TXT_ID));
	}
	if(CT::GetData(1)->IsLoaded())
	{
		ps->SetVar1("f_text2",CT::GetData(1)->SetDataTextureID(VD_TXT_ID+1));
	}

	ps->SetVar1("front_dist_txt", rtt_SetID(FRONT_FACE_TXT_ID,rtt_GetTexture(1)));
	ps->SetVar1("back_dist_txt", rtt_SetID(BACK_FACE_TXT_ID,rtt_GetTexture(2)));
	
glColor3d(1,0,0);
	if(dr)DrawQuad(pp+ll+tt,pp-ll+tt,pp-ll-tt,pp+ll-tt);

	
	//DrawCube(bb1,bb2);
	//DrawSectOfCube(b1,b2,c->GetPosition()+c->GetNav(),c->GetNav());
	ps->UnUse();


}
void Render::DeleteRM(int id)
{
	delete rendering_methods[id];
	rendering_methods.erase(rendering_methods.begin()+id);
	if(id>=rendering_methods.size())
		id = rendering_methods.size()-1;
	
	CT::need_rerender=1;
}

int Render::CloneRM(int id)
{
	RenderingMethod *tmp = new RenderingMethod(), *src = rendering_methods[id];

	tmp->size = src->size;
	tmp->pos = src->pos;

	tmp->caption = src->caption;
	tmp->fs_filename = src->fs_filename;
	tmp->isos_sum = src->isos_sum;
	tmp->quads_sum = src->quads_sum;
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		tmp->min_level[i] = src->min_level[i];
		tmp->max_level[i] = src->max_level[i];
		tmp->opacity[i] = src->opacity[i];
		tmp->tf_ww[i] = src->tf_ww[i];
		tmp->tf_quads[i] = src->tf_quads[i];	
		tmp->tf_points[i] = src->tf_points[i];	
		tmp->isos[i] = src->isos[i];
		tmp->iso_last[i] = src->iso_last[i];
		tmp->quad_last[i] = src->quad_last[i];
	}
	memcpy(tmp->color_table,src->color_table,sizeof(vec4)*TF_WIDTH*MAX_VD_NUMBER);

	tmp->use_accel_struct = src->use_accel_struct;
	tmp->use_cubic_filt = src->use_cubic_filt;
	tmp->drop_shadows = src->drop_shadows;
	tmp->shade_mode = src->shade_mode;
	tmp->tf_type = src->tf_type;


	CT::need_rerender=1;
	rendering_methods.push_back(tmp);
	return rendering_methods.size();
}
vec2 Render::GetTFWW()
{
	return GetCurRM()->tf_ww[CT::GetCurDataID()];
}
Render::~Render()
{
	ClearRMs();

	
}

void Render::SetAnag(int s, bool left)
{
	//CT::need_rerender=1;
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

	changed_anag=1;
}
vec3 Render::GetLightDir()
{
	return LightDir;
}
void Render::SetLightDir(vec3 _LightDir)
{
	LightDir = _LightDir;
	CT::need_rerender=1;
}


void Render::Save(wxFile& fs)
{
	SaveItem(fs,b1);
	SaveItem(fs,b2);

	SaveItem(fs,LightDir);
	SaveItem(fs,step_length);

	//

	
	int rm_num = rendering_methods.size();
	SaveItem(fs,rm_num);
	for(int k=0;k<rm_num;k++)
	{
		RenderingMethod* tmp = rendering_methods[k];
		

		SaveString(fs,tmp->fs_filename);
		SaveString(fs,tmp->caption);
		SaveItem(fs,tmp->size);
		SaveItem(fs,tmp->pos);
		SaveItem(fs,tmp->isos_sum);
		SaveItem(fs,tmp->quads_sum);
		for(int i=0;i<MAX_VD_NUMBER;i++)
		{
			SaveItem(fs,tmp->min_level[i]);
			SaveItem(fs,tmp->max_level[i]);
			SaveItem(fs,tmp->opacity[i]);
			SaveVector(fs,tmp->tf_quads[i]);
			SaveVector(fs,tmp->tf_points[i]);
			SaveVector(fs,tmp->isos[i]);
			SaveItem(fs,tmp->iso_last[i]);
			SaveItem(fs,tmp->quad_last[i]);
		}
		SaveItem(fs,tmp->use_accel_struct);
		SaveItem(fs,tmp->use_cubic_filt);
		SaveItem(fs,tmp->drop_shadows);
		SaveItem(fs,tmp->shade_mode);
		SaveItem(fs,tmp->tf_type);
	}
	SaveItem(fs,CurRM);
	
}
void Render::ClearRMs()
{
	for(int i=0;i<rendering_methods.size();i++)
		delete rendering_methods[i];
	rendering_methods.clear();
}
void Render::Load(wxFile& fs)
{
	OpenItem(fs,b1);
	OpenItem(fs,b2);
	
	OpenItem(fs,LightDir);
	
	OpenItem(fs,step_length);

//
	

	RenderingMethod* tmp;
	//ClearRMs();
	
	int rm_num;
	OpenItem(fs,rm_num);
	for(int k=0;k<rm_num;k++)
	{
		wxString tmp_s;
		OpenString(fs,tmp_s);
		tmp=0;
		for(int i=0;i<rendering_methods.size();i++)
			if(rendering_methods[i]->fs_filename==tmp_s)
			{
				tmp = rendering_methods[i];break;
			}
		if(!tmp)continue;
		OpenString(fs,tmp->caption);
		OpenItem(fs,tmp->size);
		OpenItem(fs,tmp->pos);
	
		OpenItem(fs,tmp->isos_sum);
		OpenItem(fs,tmp->quads_sum);
		for(int i=0;i<MAX_VD_NUMBER;i++)
		{
			OpenItem(fs,tmp->min_level[i]);
			OpenItem(fs,tmp->max_level[i]);
			OpenItem(fs,tmp->opacity[i]);
			OpenVector(fs,tmp->tf_quads[i]);
			tmp->ApplyQuads(i);
			OpenVector(fs,tmp->tf_points[i]);
			OpenVector(fs,tmp->isos[i]);
			tmp->ApplyPoints(i);
			OpenItem(fs,tmp->iso_last[i]);
			OpenItem(fs,tmp->quad_last[i]);
		}
		//rendering_methods.push_back(tmp);
		OpenItem(fs,tmp->use_accel_struct);
		OpenItem(fs,tmp->use_cubic_filt);tmp->use_cubic_filt=0;
		OpenItem(fs,tmp->drop_shadows);
		OpenItem(fs,tmp->shade_mode);
		OpenItem(fs,tmp->tf_type);
		tmp=0;

	}
	OpenItem(fs,CurRM);
	

	SetCurRM(CurRM);



	ReLoadShader();

}

