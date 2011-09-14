#ifndef IMG3DPROC_H
#define IMG3DPROC_H

#include "AllDef.h"
#include "vec3.h"
#include "ivec3.h"
#include "ivec2.h"
#include "SimText3D.h"
#include "VolumeData.h"


VData ResampleToMIP(VData& data);
void filter_Median(short*src,short*dst,ivec3 size);
void ApplyFilter2(SimText3D *img3d,ivec3 src_size,VData dst,GLuint t_type,std::string shader_filename);
void MakeBoundingCells(SimText3D *img3d,ivec3 src_size,void*dst,ivec3 dst_size,GLuint t_type,std::string shader_filename);


VData GetMipData(VData& data,int dv);

template <class T>
void MakeGrad(T*src,T*dst,ivec3 size)
{
	int ii1,ii2,id=0;
	for(int i=0;i<size.x;i++)
	for(int j=0;j<size.y;j++)
	for(int k=0;k<size.z;k++)
	{
		ii1 = max(i-1,0)+size.x*(j+size.y*k);
		ii2 = min(i+1,size.x-1)+size.x*(j+size.y*k);
		T dx = (src[ii2]-src[ii1])*(2/(ii2-ii1));

		ii1 =i+size.x*( max(j-1,0)+size.y*k);
		ii2 = i+size.x*(min(j+1,size.y-1)+size.y*k);
		T dy = (src[ii2]-src[ii1])*(2/(ii2-ii1));

		ii1 =i+size.x*( j+size.y*max((k-1),0));
		ii2 = i+size.x*(j+size.y*min(k+1,size.z-1));
		T dz = (src[ii2]-src[ii1])*(2/(ii2-ii1));

		ii1 = i+size.x*(j+size.y*k);
		dst[id] = dx*dx+dy*dy+dz*dz;
		id++;
	}
}

class Histogram
{
public:
	Histogram();
	void Build(VData&data);
	~Histogram();
	unsigned int * Get(){return hist;}
	int GetSize(){return hist_size;}
	unsigned int GetMaxChunk(){return max_chunk;}

	int chunk_size;
private:
	unsigned int * hist;
	int hist_size;
	unsigned int max_chunk;
};
class Histogram2D
{
public:
	Histogram2D();
	void Build(VData&data);
	void Build(VData&data,VData&data2);
	~Histogram2D();
	unsigned int * Get(){return hist;}
	ivec2 GetSize(){return hist_size;}
	unsigned int GetMaxChunk(){return max_chunk;}
	void UpdateTexture();
	void Draw(float center);

	ivec2 chunk_size;
	//SimText3D*txt2;
	bool need_reload_txt;
	
	float mag_scale;
private:

	unsigned int * hist;
	ivec2 hist_size;
	unsigned int max_chunk;
};


struct DataStat
{
	DataStat();
	~DataStat();
	
	void AnalyseData(VData&data);
	short MinValue,MaxValue;
	double MidValue;
};
/*
namespace segmentation
{
	void Init();
	void UnInit();

	void Mark(ivec3 point,BYTE val);
	void UnMarkAll();
	void MarkBB(BYTE val);
	void MakeSegmentation();

	void MedianFilter();
	void Resample();

	extern BYTE* region;
}
*/
extern DataStat data_stat;
extern Histogram histogram;

#endif