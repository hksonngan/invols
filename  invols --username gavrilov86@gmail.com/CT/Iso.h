#ifndef ISO_H
#define ISO_H
#include "vec3.h"
#include "vec2.h"
#include "Camera.h"

#include "SimText.h"
#include "SimText3D.h"
#include <ivec3.h>

class ShaderProgram;

#define MAX_ISO_NUM 6

struct IsoSurface
{
	float value;
	vec4 color;
};
class IsoViewer
{
public:
	IsoViewer();
	~IsoViewer();
	void ApplyCamera(Camera* c);

	void UploadFieldData(void*data,ivec3 size,int data_type);

	void defApply();
	void Draw(Camera* c);

	vec3 GetLevelColor(float level);

	void SetLevel(float nl1);
	void UpdateUniforms();
	void SetBoundingBox(vec3 a,vec3 b);
	float GetMinLevel(){return l1;}
	vec3 GetMinBox(){return b1;}
	vec3 GetMaxBox(){return b2;}
	void SetQuality(float q);
	float GetQuality();
	void SetFlag(float f);
	float GetFlag();

	void SetAnag(int s,bool left);

	bool draw_frame_is;
	
	vec2 GetBrCont();
	void SetBrCont(vec2 bc);
	

	void ApplyColorTable();

//	vec3 GetScale();
//	void SetScale(vec3 _scale);
	vec3 GetLightDir();
	void SetLightDir(vec3 _LightDir);

	bool use_accel_struct,draw_fog;
private:
	vec3 LightDir;
	float fl;
	float c_br,c_contr,max_add;
	vec3 anag[3];


	ShaderProgram *ps;

	SimText3D* st;
	SimText3D* st_i;
//	SimText3D* st_n;
	SimText3D* txt_color;

	float l1;
	vec3 b1,b2;
	vec3 scale;
	float qual;
	
};

#endif