#include "VolumeData.h"

#include "Progress.h"
#include "Img3DProc.h"
#include "CT.h"
#include "CPU_VD.h"

VData::VData():size(0),spacing(1.0/512.0),allocated(0)
{
}
VData::VData(ivec3 n_size,int nvalue_format):size(0),spacing(1.0/512.0),allocated(0)
{
	Allocate(n_size,nvalue_format);
}
void VData::Allocate(ivec3 n_size,int nvalue_format)
{
	Clear();
	
	value_format = nvalue_format;
	value_size = nvalue_format?1:2;
	size = n_size;
	if(value_format==0)	dt_s = new short*[n_size.z];
	if(value_format==1)	dt_b = new BYTE*[n_size.z];
	for(int i=0;i<n_size.z;i++)
	{
		if(value_format==0)	dt_s[i] = new short[n_size.x*n_size.y];
		if(value_format==1)	dt_b[i] = new BYTE[n_size.x*n_size.y];
	}
	allocated=1;

}
void VData::SetAllValues(short val)
{
	for(int i=0;i<size.x;i++)
	for(int j=0;j<size.y;j++)
	for(int k=0;k<size.z;k++)
	{
		SetValue(val,i,j,k);
	}

}
void VData::Clear()
{
	if(!allocated)return;
	allocated=0;
	for(int i=0;i<size.z;i++)
	{
		if(value_format==0)	delete[] dt_s[i];
		if(value_format==1)	delete[] dt_b[i];
	}
	if(value_format==0)	delete[]dt_s;
	if(value_format==1)	delete[]dt_b;
	

}
void VData::SwapWith(VData vd)
{
	swap(spacing,vd.spacing);
	swap(dt_s,vd.dt_s);
	swap(dt_b,vd.dt_b);
	swap(size,vd.size);
	swap(allocated,vd.allocated);
	swap(value_format,vd.value_format);
	swap(value_size,vd.value_size);
}
short VData::GetValue(ivec3 id)
{
	if(value_format==0)
		return *(dt_s[id.z]+id.x+id.y*size.x);
	if(value_format==1)
		return *(dt_b[id.z]+id.x+id.y*size.x);
	return 0;
}
short VData::GetValue(int i,int j,int k)
{
	if(value_format==0)
		return *(dt_s[k]+i+j*size.x);
	if(value_format==1)
		return *(dt_b[k]+i+j*size.x);
	return 0;
}
void VData::SetValue(short val,ivec3 id)
{
	if(value_format==0)
		*(dt_s[id.z]+id.x+id.y*size.x) = val;	
	if(value_format==1)
		*(dt_b[id.z]+id.x+id.y*size.x) = val;
}
void VData::SetValue(short val,int i,int j,int k)
{
	if(value_format==0)
		*(dt_s[k]+i+j*size.x) = val;
	if(value_format==1)
		*(dt_b[k]+i+j*size.x) = val;
}
void*VData::GetSlice(int z)
{
	if(!allocated)return 0;
	if(value_format==0)
		return dt_s[z];
	if(value_format==1)
		return dt_b[z];
	return 0;

}
void**VData::GetSlices()
{
	if(!allocated)return 0;
	if(value_format==0)
		return (void**)dt_s;
	if(value_format==1)
		return (void**)dt_b;
	return 0;
}
ivec3 VData::GetSize()
{
	return size;
}

//////////////////////

VolumeData::VolumeData():st(0),st_i(0)//,st_small(0)
{
}

VolumeData::~VolumeData()
{
	if(st)delete st;
	if(st_i)delete st_i;
	//if(st_small)delete st_small;
}

#define CELL_SIZE 16

short* GetSubData(VData& data,ivec3& pos,ivec3& sub_size)
{
	ivec3 size = data.GetSize();
	if(sub_size.x&1){sub_size.x--;}
	if(sub_size.y&1){sub_size.y--;}
	if(sub_size.z&1){sub_size.z--;}
	pos = ivec3::Min(pos,size-ivec3(1));
	sub_size = sub_size - ivec3::Max(pos+sub_size-size,ivec3(0));
	short*res=0,*res1=res;
	
	res = new short[sub_size.x*sub_size.y*sub_size.z];
	
	res1=res;
	
	for(int k=0;k<sub_size.z;k++)
	for(int j=0;j<sub_size.y;j++)
	for(int i=0;i<sub_size.x;i++)
	{
//		if(i+pos.x<0 || i+pos.x>=size.x || j+pos.y<0 || j+pos.y>=size.y || k+pos.z<0 || k+pos.z>=size.z)
//			*res1=0;
//		else
		*res1 = data.GetValue(pos+ivec3(i,j,k));
		res1++;
	}

	return res;
}
BYTE* byte_GetSubData(VData& data,ivec3& pos,ivec3& sub_size)
{
	ivec3 size = data.GetSize();
	if(sub_size.x&1){sub_size.x--;}
	if(sub_size.y&1){sub_size.y--;}
	if(sub_size.z&1){sub_size.z--;}
	pos = ivec3::Min(pos,size-ivec3(1));
	sub_size = sub_size - ivec3::Max(pos+sub_size-size,ivec3(0));
	BYTE*res=0,*res1=res;
	
	res = new BYTE[sub_size.x*sub_size.y*sub_size.z];
	
	res1=res;
	
	for(int k=0;k<sub_size.z;k++)
	for(int j=0;j<sub_size.y;j++)
	for(int i=0;i<sub_size.x;i++)
	{
//		if(i+pos.x<0 || i+pos.x>=size.x || j+pos.y<0 || j+pos.y>=size.y || k+pos.z<0 || k+pos.z>=size.z)
//			*res1=0;
//		else
		*res1 = data.GetValue(pos+ivec3(i,j,k));
		res1++;
	}

	return res;
}

/*
template <class T>
T* ResampleTo256(T *data,ivec3 size)
{
	float sc = size.z/256.0f;
	T*data_small = new T[256*256*256];
	if(size.x==512 && size.y==512)
	{
		for(int i=0;i<256;i++)
		{
			Progress::inst->SetPercent((1000*i)/256);
		for(int j=0;j<256;j++)
		for(int k=0;k<256;k++)
		{
			float sum=0,ww=0;
			int k1=int(k*sc)-1,k2=int((k+1)*sc)+1;
			if(k1<0)k1=0;
			if(k2>=size.z)k2=size.z-1;
			for(int kk=k1;kk<=k2;kk++)
			for(int ii=0;ii<2;ii++)
			for(int jj=0;jj<2;jj++)
			{
				float www = 1.0/(abs(k*sc-kk)+1);
				sum += data[i*2+ii+size.x*(j*2+jj+size.y*kk)]*www;
				ww += www;

			}

			data_small[i+256*(j+256*k)] = T(sum/ww);
		}
		}
	
	}else
	{
		throw "123123";
	}
	
	return data_small;
}
*/
/*
short* ResampleTo256_GPU(SimText3D *st)
{
	double time = glfwGetTime ( );
	vec3 size(st->width,st->height,st->depth), new_size(256);
	ShaderProgram sp("ct_proc.vs","ct_proc.fs");
	SimText img(new_size.x/4,new_size.y,&sp,6);
	sp.Use();
	sp.SetVar("f_text",st->GetTexture());
	sp.UnUse();

	float* tmp = new float[new_size.x*new_size.y*new_size.z],*c_tmp=tmp;
	for(int k=0;k<size.z;k++)
	{
		sp.Use();
		sp.SetVar("current_z",float(k)/size.z);
		sp.UnUse();

		img.Simulation(c_tmp);
		c_tmp+=new_size.x*new_size.y;
	}
 
	float max_tmp=0,min_tmp=99999;

	dst = new short[new_size.x*new_size.y*new_size.z];
	int idd=0;
	for(int i=0;i<new_size.x;i++)
		for(int j=0;j<new_size.y;j++)
			for(int k=0;k<new_size.z;k++)
			{
				float val_mod = tmp[idd];
				if(min_tmp>val_mod)min_tmp=val_mod;
				if(max_tmp<val_mod)max_tmp=val_mod;
				idd++;
			}
	max_tmp+=0.00001;
	for(int i=new_size.x*new_size.y*new_size.z-1;i>=0;i--)
		dst[i] = ((tmp[i]-min_tmp)/(max_tmp-min_tmp))*256.0f;

	delete[] tmp;

	return dst;
}
*/
/*

unsigned char* GetHalfData(unsigned char *data,ivec3 size,ivec3& out_size)
{
	ivec3 size2(size.x/2,size.y/2,size.z/2);
	unsigned char*data_small = new unsigned char[size2.x*size2.y*size2.z];

	for(int i=0;i<size2.x;i++)
	for(int j=0;j<size2.y;j++)
	for(int k=0;k<size2.z;k++)
	{
		int sum = 0;
		for(int ii=0;ii<2;ii++)
		for(int jj=0;jj<2;jj++)
		for(int kk=0;kk<2;kk++)
			sum += data[i*2+ii+size.x*(j*2+jj+size.y*(k+kk)*2)];

		data_small[i+size2.x*(j+size2.y*k)] = unsigned char(sum/8.0);
	}
	out_size = size2;

	return data_small;
}
*/
#define ACCLEL_STRUCT_SIZE 32

template<class T>
T* MakeBoundingCells(T*data,ivec3 size,ivec3& size_b)
{
	int id;
	size_b = ivec3(ACCLEL_STRUCT_SIZE);
	T*res = new T[size_b.x*size_b.y*size_b.z*3];

	for(int i=0;i<size_b.x;i++)
	{
	for(int j=0;j<size_b.y;j++)
	for(int k=0;k<size_b.z;k++)
	{
		T max=0;
		T min=0;
		bool bbb=1;
		for(int ii=(i*size.x)/size_b.x-1;ii<((i+1)*size.x)/size_b.x+1;ii++)
		for(int jj=(j*size.y)/size_b.y-1;jj<((j+1)*size.y)/size_b.y+1;jj++)
		for(int kk=(k*size.z)/size_b.z-1;kk<((k+1)*size.z)/size_b.z+1;kk++)
			if( ivec3(ii,jj,kk).InBounds(ivec3(0),size) )
			{
				T vl = data[(ii + size.x*(jj + size.y*kk))];
				if(bbb){bbb=0;max=min=vl;}
				if(vl > max) {max=vl;}
				if(vl < min) {min=vl;}
			}

		id=(i+(size_b.x)*(j+k*(size_b.y)));
		res[id*3] = min;
		res[id*3+1] = max;
		
		
//		id+=3;
	}
	Progress::inst->SetPercent(i/float(size_b.x));
	}
	
	return res;
}

void VolumeData::Upload(VData& data)//data_type: 0-UBYTE 2-short
{
	size = data.GetSize();
	
	if(st)delete st;
	int df = data.GetValueFormat();
//	int type_size = 2;
	
	void*tmpp;

	ivec3 sub_size = CPU_VD::gpu_size_dummy;
	ivec3 pos = CPU_VD::GetRecomendedGPUOffset();
	sub_size = ivec3::Min(sub_size,size);
	void*sub_data;
	if(df==0) sub_data = GetSubData(data,pos,sub_size);
	else sub_data = byte_GetSubData(data,pos,sub_size);
	

	CPU_VD::gpu_size = sub_size;
	CPU_VD::gpu_offset = pos;
	CPU_VD::CalcReal();

	
	st = new SimText3D(3,sub_size.x,sub_size.y,sub_size.z,1,sub_data,1,true,data.GetGLFormat());

	delete[]sub_data;

}
unsigned int VolumeData::SetDataTextureID(int id)
{
	if(!st)return 0;
	return st->SetID(id);
}
/*
unsigned int VolumeData::GetSmallDataTexture()
{
	return st_small?st_small->GetTexture():0;
}
*/
unsigned int VolumeData::SetIDataTextureID(int id)
{
	if(!st_i)return 0;
	return st_i->SetID(id);
}
