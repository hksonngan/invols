#ifndef ISO_H
#define ISO_H

#include "vec3.h"
#include "vec2.h"
#include "SimText.h"
#include "SimText3D.h"
#include <ivec3.h>
#include "VolumeData.h"
#include "CS3.h"
#include "wx/file.h"

class ShaderProgram;

#define TF_WIDTH 512
#define MAX_POINTS_NUM 200
#define MAX_ISO_NUM 10
#define MAX_QUAD_NUM 10
#define MAX_VD_NUMBER 4

struct IsoSurface
{
	float value;
	vec4 color;
};
struct TF_POINT
{
	float value;
	vec4 color;
};
struct TF_QUAD
{
	float l1,l2,min_gm,max_gm;
	vec4 color;
};


class RenderingMethod
{
public:
	RenderingMethod();
	~RenderingMethod();
	
	void ApplyQuads();
	void ApplyQuads(int vd_id);

	void ApplyPoints();
	void ApplyPoints(int vd_id);

	bool AddIso(vec2 pos);
	bool AddPoint(vec2 pos);
	bool AddQuad(vec2 pos);

	void DeleteIso(int id);
	void DeletePoint(int id);
	void DeleteQuad(int id);

	vec2 pos,size;

	wxString caption;
	wxString fs_filename;
	ShaderProgram *sp;
	bool need_reload_shader,need_update_uniforms,need_reload_color_table;

	int isos_sum, iso_last[MAX_VD_NUMBER];
	std::vector<IsoSurface> isos[MAX_VD_NUMBER];
	float min_level[MAX_VD_NUMBER];
	float max_level[MAX_VD_NUMBER];
	float opacity[MAX_VD_NUMBER];

	bool use_accel_struct, use_cubic_filt, drop_shadows;
	char shade_mode;//0-no 1-Phong
	char tf_type;//0-grey 1-1d tf
	
//	std::vector<TF_QUAD2> tf_quads2[MAX_VD_NUMBER];

	std::vector<TF_POINT> tf_points[MAX_VD_NUMBER];

	int quads_sum, quad_last[MAX_VD_NUMBER];
	std::vector<TF_QUAD> tf_quads[MAX_VD_NUMBER];
	vec4 color_table[TF_WIDTH*MAX_VD_NUMBER];
	vec2 tf_ww[MAX_VD_NUMBER];
	SimText3D* txt_color;


};


class Render
{
public:
	
	Render();
	~Render();

	void Draw(bool is_small);

	int CloneRM(int id);
	void DeleteRM(int id);
	void ReLoadShader();
	void ReLoadCurShader();
	
	void Save(wxFile& fs);
	void Load(wxFile& fs);
	
//params
	void UpdateUniforms();

	void SetWindow(vec2 level,int id);
	void SetWindow(vec2 level);
	vec2 GetWindow(int id);
	vec2 GetWindow();

	void SetBoundingBox(vec3 a,vec3 b);
	vec3 GetBoundingBox(bool max){return max?b2:b1;}


	void SetStepLength(float q);
	float GetStepLength();

	void SetAnag(int s,bool left);
	void ApplyColorTable(vec2 _tf_ww);

	vec3 GetLightDir();
	void SetLightDir(vec3 _LightDir);
	
	void SetCurRM(int id);
	int GetCurRMID();
	RenderingMethod* GetCurRM();

	vec2 GetTFWW();

	void ClearRMs();

	void SetOpacity(float val);
	float GetOpacity();
	SimText3D* GetTFTexture();
	vec4* GetCurTF();


	bool changed_anag;
	

	CS3 volume_transform[MAX_VD_NUMBER];

	std::vector<RenderingMethod*> rendering_methods;

private:
	void _UpdateUniforms();
	void _ReLoadShader();

	int CurRM;
	vec3 LightDir;
	vec3 anag[3];
	vec3 b1,b2;
	float step_length;



};


#endif