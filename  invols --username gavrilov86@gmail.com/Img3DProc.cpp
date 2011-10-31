#include "AllInc.h"
#include "Img3DProc.h"
#include "memory.h"
#include "Progress.h"
#include "str.h"
#include "output.h"
#include "CPU_VD.h"
#include "Draw2D.h"
#include "CT/CT.h"
/*
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"
*/

#include <wx/numdlg.h>
#include "View2D.h"
#include "TF_Window.h"
#include "VolumeData.h"

//using namespace tbb;


//mutex* progress_mutex;
/*
void InitProc()
{
	static tbb::task_scheduler_init init;
	progress_mutex = new mutex();
}
void UnInitProc()
{
	delete progress_mutex;
}

class tbb_ResampleTo256
{
    VData *data,*res;

	float sc;
public:
	tbb_ResampleTo256(VData* _data,VData* _res) : data(_data),res(_res){ sc = data->GetSize().z/256.0f;};

public:
    void operator()(const blocked_range <size_t>& r) const
    {        
		ivec3 size = data->GetSize();

        for (size_t i=r.begin(); i!=r.end(); i++) 
        {
            

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
						sum += data->GetValue(i*2+ii,j*2+jj,kk)*www;
						ww += www;

					}

					//res[i+256*(j+256*k)] = short(sum/ww);
					res->SetValue(short(sum/ww),i,j,k);
				}
		
        }
		
		

    }
};


VData ResampleTo256(VData& data)
{
	InitProc();
	
	VData data_small(ivec3(256));
	ivec3 size=data.GetSize();
	
	if(!(size.x==512 && size.y==512))throw "123123";
	{
		for(int i=0;i<4;i++)
		{
			parallel_for(blocked_range<size_t>(i*64, (i+1)*64, 16),	tbb_ResampleTo256(&data,&data_small) );
			Progress::inst->SetPercent((i+1)*0.25);
		}

	}
	UnInitProc();

	return data_small;
}*/
VData GetMipData(VData& data,int dv)
{
	ivec3 size = data.GetSize();
	ivec3 size2(size.x/dv,size.y/dv,size.z/dv);
	VData data_small(size2);
	data_small.spacing = data.spacing*dv;

	for(int i=0;i<size2.x;i++)
	{
		for(int j=0;j<size2.y;j++)
		for(int k=0;k<size2.z;k++)
		{
			int sum = 0;
			for(int ii=0;ii<dv;ii++)
			for(int jj=0;jj<dv;jj++)
			for(int kk=0;kk<dv;kk++)
				sum += data.GetValue(i*dv+ii,j*dv+jj,k*dv+kk);

			data_small.SetValue(sum/float(dv*dv*dv), i,j,k);
		}
		Progress::inst->SetPercent(i/float(size2.x));
	}

	return data_small;
}
void filter_Median(short *src,short*dst,ivec3 size)
{
	int rad=1;
	Progress::inst->AddText("Median Filter");
	for(int i=0;i<size.x;i++)
	{
	for(int j=0;j<size.y;j++)
	for(int k=0;k<size.z;k++)
	{/*
		int sum=0,num=0;
		for(int ii=-rad;ii<=rad;ii++)
		for(int jj=-rad;jj<=rad;jj++)
		for(int kk=-rad;kk<=rad;kk++)
			if(i+ii>=0 && i+ii<size.x)
			if(j+jj>=0 && j+jj<size.y)
			if(k+kk>=0 && k+kk<size.z)
		{
			sum += data[ii+i+size.x*(jj+j+size.y*(kk+k))];
			num++;
		}*/
		dst[i+size.x*(j+size.y*(k))] = src[i+size.x*(j+size.y*(k))];//sum/num;
		
	}
	Progress::inst->SetPercent(i/float(size.x));
	}
}

DataStat data_stat;

void DataStat::AnalyseData(VData&data)
{
	ivec3 size = data.GetSize();
	long int sum=0;
	short max=data.GetValue(0,0,0),min=max, *c_data;
	double mid;

	output::application.println("---Data analysis---");
	for(int k=0;k<size.z;k++)
	{
		c_data = data.GetSlice(k);
		for(int j=0;j<size.y;j++)
		for(int i=0;i<size.x;i++)
		{
			if(max<*c_data)max=*c_data;
			if(min>*c_data)min=*c_data;
			sum += *c_data;
			c_data++;

		}
	}
	if(max-min>32000)
	{
		min=0;
		for(int k=0;k<size.z;k++)
		{
			c_data = data.GetSlice(k);
			for(int j=0;j<size.y;j++)
			for(int i=0;i<size.x;i++)
			{
				
				*c_data= *((unsigned short*)c_data)/2;
				c_data++;

			}
		}
	}else
	for(int k=0;k<size.z;k++)
	{
		c_data = data.GetSlice(k);
		for(int j=0;j<size.y;j++)
		for(int i=0;i<size.x;i++)
		{
			
			*c_data-=min;
			c_data++;

		}
	}
	mid = sum/double(size.x*size.y*size.z);

	output::application.println("min = "+str::ToString(min));
	output::application.println("max = "+str::ToString(max));
	output::application.println("mid = "+str::ToString((float)mid));

	MinValue = min;
	MaxValue = max;
	MidValue = mid;
}
DataStat::DataStat()
{
}
DataStat::~DataStat()
{
}


Histogram::Histogram():hist(0),chunk_size(5){}
Histogram::~Histogram()
{
	if(hist)delete[]hist;
}


void Histogram::Build(VData&data)
{
	ivec3 data_size = data.GetSize();
	hist_size = 64000/chunk_size;
	if(hist)delete[]hist;
	hist = new unsigned int[hist_size];
	memset(hist,0,hist_size*sizeof(int));
	int max_h = data_size.x*data_size.y*data_size.z/(512*64);
	
	for(int i=0;i<data_size.x-1;i+=2)
	for(int j=0;j<data_size.y-1;j+=2)
	for(int k=0;k<data_size.z-1;k+=2)
	{
		unsigned int tmpv = data.GetValue(i,j,k)/chunk_size;
		if(tmpv>=0 && tmpv<hist_size)
		{
			if(rand()%max_h > hist[tmpv])
				hist[tmpv]++;
		}
	}
	max_chunk = 0;
	unsigned int *hh = hist;
	for(int i=0;i<hist_size;i++,hh++)
	{
		if(max_chunk<*hh)max_chunk=*hh;
	}

	View2D::bc.set(0,0);
	hh = hist;
	for(int i=1;i<hist_size;i++,hh++)
	if(*hh>max_chunk/4 && *hh<max_chunk-max_chunk/20)
	{
		View2D::bc.x = (i*chunk_size)/(256.0f*128.0f);
		break;
	}
	hh = hist+hist_size-1;
	for(int i=0;i<hist_size;i++,hh--)
	if(*hh>max_chunk/4)
	{
		View2D::bc.y = ((hist_size-i)*chunk_size)/(256.0f*128.0f);
		break;
	}
//	TF_window::inst->center = View2D::bc.x;
//	TF_window::inst->scale = TF_window::inst->width/(View2D::bc.y-View2D::bc.x+0.01f);

	CT::need_rerender_tf=1;

}

///


Histogram2D::Histogram2D():hist(0),chunk_size(5,16){}
Histogram2D::~Histogram2D()
{
	if(hist)delete[]hist;
//	if(txt2)delete txt2;
}

void Histogram2D::Build(VData&data)
{
	vec3 voxel_size = data.spacing;
	chunk_size.y = wxGetNumberFromUser( "Enter chunk size for the gradient",
										_T("Enter a number(>0):"), _T("Numeric input"),
										chunk_size.y, 1, 2024, 0 );
	ivec3 data_size = data.GetSize();

	hist_size = ivec2(1024,256);
	mag_scale = (float)(hist_size.y*chunk_size.y)/(float)(256*256);
	if(hist)delete[]hist;
	hist = new unsigned int[hist_size.x*hist_size.y];
	memset(hist,0,hist_size.x*hist_size.y*sizeof(int));
	int max_h = data_size.x*data_size.y*data_size.z/(hist_size.y*hist_size.x);
	if(!max_h)max_h=1;
	for(int i=0;i<data_size.x-1;i+=2)
	for(int j=0;j<data_size.y-1;j+=2)
	for(int k=0;k<data_size.z-1;k+=2)
	{
		unsigned int val = data.GetValue(i,j,k);

		ivec3 gr(-val);
		gr.x += data.GetValue(i+1,j,k);
		gr.y += data.GetValue(i,j+1,k);
		gr.z += data.GetValue(i,j,k+1);

		unsigned int tmpv = val/chunk_size.x;
		unsigned int grad_mag = sqrt(sqr(gr.x/voxel_size.x)+sqr(gr.y/voxel_size.y)+sqr(gr.z/voxel_size.z))/chunk_size.y;
//		tmpv=rand()%hist_size.x;
//		grad_mag=rand()%hist_size.y;
		if(tmpv>=0 && tmpv<hist_size.x && grad_mag<hist_size.y)
		{
			int id = tmpv+grad_mag*hist_size.x;
			if(rand()%max_h > hist[id])
				hist[id]++;
		}
	}
	max_chunk = 0;
	unsigned int *hh = hist;
	for(int i=0;i<hist_size.x*hist_size.y;i++,hh++)
	{
		if(max_chunk<*hh)max_chunk=*hh;
	}

	//UpdateTexture();
	need_reload_txt=1;
}

void Histogram2D::Build(VData&data,VData&data2)
{
	hist_size = ivec2(1024,256);
	chunk_size.y = wxGetNumberFromUser( "Enter chunk size for the second field",
										_T("Enter a number(>=0, 0 to automatic maximum detection):"), _T("Numeric input"),
										chunk_size.y, 0, 2024, 0 );
	ivec3 data_size = data.GetSize();
	ivec3 data_size2 = data2.GetSize();
	if(!chunk_size.y)
	{
		unsigned int max2=0;
		for(int i=0;i<data_size2.x;i++)
		for(int j=0;j<data_size2.y;j++)
		for(int k=0;k<data_size2.z;k++)
		{
			unsigned int val = data2.GetValue(i,j,k);
			if(max2<val)max2=val;
		}
		chunk_size.y = max(1,max2/hist_size.y);
	}
	
	mag_scale = (float)(hist_size.y*chunk_size.y)/(float)(256*256);
	if(hist)delete[]hist;
	hist = new unsigned int[hist_size.x*hist_size.y];
	memset(hist,0,hist_size.x*hist_size.y*sizeof(int));
	int max_h = data_size.x*data_size.y*data_size.z/(hist_size.y*hist_size.x);
	if(!max_h)max_h=1;
	for(int i=0;i<data_size.x;i+=2)
	for(int j=0;j<data_size.y;j+=2)
	for(int k=0;k<data_size.z;k+=2)
	{
		unsigned int val = data.GetValue(i,j,k);

		int i1 = (i*data_size2.x)/data_size.x;
		int j1 = (j*data_size2.y)/data_size.y;
		int k1 = (k*data_size2.z)/data_size.z;
		if(i1>=data_size2.x)i1=data_size2.x-1;
		if(j1>=data_size2.y)j1=data_size2.y-1;
		if(k1>=data_size2.z)k1=data_size2.z-1;

		unsigned int tmpv = val/chunk_size.x;
		unsigned int grad_mag = data2.GetValue(i1,j1,k1)/chunk_size.y;
//		tmpv=rand()%hist_size.x;
//		grad_mag=rand()%hist_size.y;
		if(tmpv>=0 && tmpv<hist_size.x && grad_mag<hist_size.y)
		{
			int id = tmpv+grad_mag*hist_size.x;
			if(rand()%max_h > hist[id])
				hist[id]++;
		}
	}
	max_chunk = 0;
	unsigned int *hh = hist;
	for(int i=0;i<hist_size.x*hist_size.y;i++,hh++)
	{
		if(max_chunk<*hh)max_chunk=*hh;
	}

	//UpdateTexture();
	need_reload_txt=1;
}

void Histogram2D::UpdateTexture()
{
//	if(txt2)delete txt2;
//	txt2 = new SimText3D(2,hist_size.x,hist_size.y,1,1,hist,0,1,GL_UNSIGNED_SHORT);
}
