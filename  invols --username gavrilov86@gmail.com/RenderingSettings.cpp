#include "RenderingSettings.h"

/*

void RenderingSettings::ReLoadShader()
{
	need_reload_shader=1;
	CT::need_rerender=1;
	//_ReLoadShader();

	
}
void RenderingSettings::_ReLoadShader()
{
	isos_sum=0;
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		isos_sum += (int)CT::isos[i].size();
		iso_last[i] = isos_sum-1;
	}

	need_reload_shader=0;
	if(ps)delete ps;
	svec vt;
	
	str::StringPairs repl;
	str::AddPair(repl,"$IsPerspective",str::ToString((int)CT::cam.GetProjection()));
	
	str::AddPair(repl,"$IsoNum",str::ToString(isos_sum));
	str::AddPair(repl,"$VD_NUMBER",str::ToString(CT::GetVolumesNumber()));
	str::AddPair(repl,"$use_accel_struct",use_accel_struct?"1":"0");
	str::AddPair(repl,"$draw_fog",draw_fog?"1":"0");
	str::AddPair(repl,"$use_cubic_filt",use_cubic_filt?"1":"0");
	str::AddPair(repl,"$drop_shadows",drop_shadows?"1":"0");
	
	str::CreateTxtFile("full.fs",str::OpenTxtFile("head.fs")+str::OpenTxtFile(fs_file_name));
	str::ReplaceInFile("full.fs","current.fs",repl);

	ps = new ShaderProgram("srf.vs", "current.fs");
	_UpdateUniforms();
}
void RenderingSettings::LoadShader(std::string fname)
{
	fs_file_name = fname;
	ReLoadShader();
}


RenderingSettings::RenderingSettings():ps(0),txt_color(0),isos_num(0)
{
	need_reload_color_table=need_reload_shader=need_update_uniforms=0;
	
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		min_level[i]=0;
		max_level[i]=1;
		opacity[i] = 0.5;
	}
}
void RenderingSettings::UpdateUniforms()
{
	need_update_uniforms = 1;
	//_UpdateUniforms();
}
void RenderingSettings::_UpdateUniforms()
{
	need_update_uniforms = 0;
	ps->Use();
	if(isos_sum)
	{
		float ll[MAX_ISO_NUM];
		vec4 cc[MAX_ISO_NUM];
		int id=0;
		for(int i=0;i<MAX_VD_NUMBER;i++)
			for(int j=0;j<CT::isos[i].size();j++,id++)
			{
				ll[id]=CT::isos[i][j].value;
				cc[id]=CT::isos[i][j].color;
			}

		ps->SetVar("level",isos_sum,ll);
		ps->SetVar("my_color",isos_sum,cc);
	}
	ps->UnUse();
	CT::need_rerender=1;
}
void RenderingSettings::SetWindow(vec2 w,int id)
{
	if(w.x>w.y)w.x=w.y=(w.x+w.y)*0.5f;
	min_level[id] = w.x;
	max_level[id] = w.y;
}
void RenderingSettings::SetWindow(vec2 level)
{
	SetWindow(level,CT::GetCurDataID());
}
vec2 RenderingSettings::GetWindow(int id)
{
	return vec2(min_level[id],max_level[id]);
}
vec2 RenderingSettings::GetWindow()
{
	return GetWindow(CT::GetCurDataID());
}

void RenderingSettings::ApplyColorTable()
{
	CT::need_rerender=1;
	need_reload_color_table=1;
}

void RenderingSettings::Apply()
{
	
	if(need_reload_shader)
	{
		_ReLoadShader();
	}

	if(need_reload_color_table)
	{
		if(txt_color)delete txt_color;
		txt_color = new SimText3D(2,TF_WIDTH,MAX_VD_NUMBER,1,4,&CT::color_table->x,0,1,GL_FLOAT);
		need_reload_color_table=0;
	}

	if(need_update_uniforms)_UpdateUniforms();
	ps->Use();

	if(txt_color)
	{
		ps->SetVar("txt_level_color",txt_color->GetTexture());
	}

	for(int i=0;i<MAX_VD_NUMBER;i++)
	if(CT::GetData(i)->GetDataTexture())
	{
		dr=1;
		std::string idd = "["+str::ToString(i)+"]";
		ps->SetVar1("min_level"+idd,min_level[i]);
		ps->SetVar1("max_level"+idd,max_level[i]);
		ps->SetVar1("opacity"+idd,opacity[i]);
		ps->SetVar1("IsoLast"+idd,iso_last[i]);

	}
	
}
RenderingSettings::~RenderingSettings()
{
	delete ps;
	delete txt_color;

}

void Render::Save(std::ofstream& fs)
{
	SaveItem(fs,b1);
	SaveItem(fs,b2);

	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		SaveItem(fs,min_level[i]);
		SaveItem(fs,max_level[i]);
	}

	SaveItem(fs,use_accel_struct);

	SaveItem(fs,draw_fog);
	SaveItem(fs,LightDir);
	SaveItem(fs,step_length);

	SaveString(fs,fs_file_name);
	
}
void Render::Load(std::ifstream& fs)
{
	OpenItem(fs,b1);
	OpenItem(fs,b2);
	for(int i=0;i<MAX_VD_NUMBER;i++)
	{
		OpenItem(fs,min_level[i]);
		OpenItem(fs,max_level[i]);
	}

	OpenItem(fs,use_accel_struct);

	OpenItem(fs,draw_fog);
	OpenItem(fs,LightDir);
	
	OpenItem(fs,step_length);

	OpenString(fs,fs_file_name);

	ReLoadShader();

}


*/