#ifndef VDATA_H
#define VDATA_H

#include "vec3.h"
#include "vec2.h"
#include "SimText3D.h"
#include <ivec3.h>
#include "wxIncludes.h"


class VData
{
public:
	VData();
	VData(ivec3 n_size,int nvalue_format);

	void Allocate(ivec3 n_size,int nvalue_format);
	void Clear();
	
	void SwapWith(VData vd);
	void SetAllValues(short val);
	void SetValue(short val,ivec3 id);
	void SetValue(short val,int i,int j,int k);
	short GetValue(ivec3 id);
	short GetValue(int i,int j,int k);
	void*GetSlice(int z);
	void**GetSlices();
	ivec3 GetSize();

	vec3 spacing;
	int GetValueFormat(){return value_format;}
	int GetGLFormat(){return (value_format==0)?GL_SHORT:GL_UNSIGNED_BYTE;}
	int GetValueSize(){return value_size;}
	
private:
	bool allocated;
	short**dt_s;
	BYTE**dt_b;
	ivec3 size;
	int value_format;//0-short 1-ubyte
	int value_size;
};

class VolumeData
{
public:
	~VolumeData();
	VolumeData();
	unsigned int SetDataTextureID(int id);
//	unsigned int GetSmallDataTexture();
	unsigned int SetIDataTextureID(int id);
	
	void Upload(VData& data);
	
	ivec3 GetSize(){return size;}
	//ivec3 GetSmallSize(){return small_size;}

	bool IsLoaded(){return !!st;}
	SimText3D* st;//,*st_small;
private:
	
	ivec3 size;//,small_size;
	//unsigned int data_type;
	SimText3D* st_i;
//	SimText3D* st_n;

};
short* GetSubData(VData& data,ivec3& pos,ivec3& sub_size);

#endif