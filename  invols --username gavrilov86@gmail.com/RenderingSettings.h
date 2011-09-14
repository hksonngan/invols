#ifndef RENDERING_SETTINGS_H
#define RENDERING_SETTINGS_H

/*
#include "vec3.h"
#include "vec2.h"
#include "SimText.h"
#include "SimText3D.h"
#include <ivec3.h>
#include "VolumeData.h"

class RenderingSettings
{
public:

	
	RenderingSettings();
	~RenderingSettings();

	void ReLoadShader();
	void LoadShader(std::string fname);
	void Save(std::ofstream& fs);
	void Load(std::ifstream& fs);
	
//params
	void UpdateUniforms();

	void SetWindow(vec2 level,int id);
	void SetWindow(vec2 level);
	vec2 GetWindow(int id);
	vec2 GetWindow();

	void ApplyColorTable();

	
	std::string GetFileName(){return fs_file_name;}

	//unsigned int GetTFTexture(){if(!txt_color)return 0;return txt_color->GetTexture();}



private:

	bool need_reload_color_table, need_reload_shader, need_update_uniforms;

	void _UpdateUniforms();
	void _ReLoadShader();

	float min_level[MAX_VD_NUMBER];
	float max_level[MAX_VD_NUMBER];
	float opacity[MAX_VD_NUMBER];

	ShaderProgram *ps;
	SimText3D* txt_color;
	std::string fs_file_name;
	vec4* color_table;
	
	std::vector<IsoSurface> isos[MAX_VD_NUMBER];
	int isos_sum, iso_last[MAX_VD_NUMBER];

};
*/
#endif