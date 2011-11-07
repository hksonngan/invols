#include "DicomReader.h"
#include "Progress.h"
#include "Img3DProc.h"
#include "CPU_VD.h"
#include "vec.h"

#define BUF_SIZE 1024*1024

namespace DicomReader
{
	wxString cur_dicom_dir;
	float slice_thickness=1, spacing_between_slices=1;
	float slice_location[2]={0,1};
	float pixel_spacing[2]={0,0};
	float reconstruction_diameter=0;

	float GetScale()
	{
		return reconstruction_diameter;
	}


int FindStringInBuffer(char * buf, char * s)
{
	int i,j;
	int sl = strlen(s);
	for(i=128;i<BUF_SIZE-44;i++)
	{
		for(j=0;j<sl;j++)
			if(buf[i+j]!=s[j])break;
		if(j==sl)return i;


	}
	return -1;
}
int FindElement(unsigned char * buf, unsigned int n1, unsigned int n2)
{
	int i;
	for(i=128;i<BUF_SIZE-44;i++)
	{
		//if(buf[i+1]==(unsigned char)(n1&0xFF) && (unsigned char)buf[i+0]==((n1>>16)&0xFF) && (unsigned char)buf[i+3]==(n2&0xFF) && (unsigned char)buf[i+2]==((n2>>16)&0xFF))
		unsigned int nn1 = buf[i] + buf[i+1]*256;
		unsigned int nn2 = buf[i+2] + buf[i+3]*256;
		if(nn1==n1 && nn2==n2)
		{
				return i+4;
		}
	}
	return -1;
}
bool FindInt(unsigned char * buf, unsigned int n1, unsigned int n2, int& res)
{
	int id = FindElement(buf,n1,n2);
	if(id==-1)return false;
	if(buf[id]=='U' && buf[id+1]=='L')
		res = buf[id+2]*256 + buf[id+3];

	if(buf[id]=='U' && buf[id+1]=='S')
		res = buf[id+2+2] + buf[id+3+2]*256;
	

	return true;
}
bool FindString(unsigned char * buf, unsigned int n1, unsigned int n2, std::string& res)
{
	int id = FindElement(buf,n1,n2);
	if(id==-1)return false;
	int nn = buf[id+2]*256 + buf[id+3];
	std::string res1(buf+id+4,buf+id+4+nn);
	res=res1;

	return true;
}

bool LoadDICOMDIR(wxString dicom_dir)
{
	int id,nn=0,bpp=0;
	short *arri = 0;
	unsigned char *buf = new unsigned char[BUF_SIZE];
	wxString f = wxFindFirstFile(dicom_dir+"/*");
	std::string ss;


	vec3 scale(1,1,1);
	ivec3 size(0,0,0);

	output::application.println("----loading DICOM files----");
	Progress::inst->AddText("Looking for DICM files...<br>");

	while ( !f.empty() )
	{
		for(int jj=0;jj<4;jj++)buf[128+jj]=0;

		wxFile fs;
		
		fs.Open(f);
		
		f = wxFindNextFile();
		if(!fs.IsOpened())continue;
		fs.Read((void*)buf,135);
		
		fs.Close();
		if(!(buf[128]!='D' || buf[129]!='I' || buf[130]!='C' || buf[131]!='M')) nn++;
	}
	Progress::inst->AddText(str::ToString(nn)+wxT(" slices<br>"));
	f = wxFindFirstFile(dicom_dir+"/*");
	size.z = nn;

	if(!nn)return 0;
	int nn1=0;
	while ( !f.empty() )
	{
		for(int jj=0;jj<4;jj++)buf[128+jj]=0;

		wxFile fs;
		
		fs.Open(f);
		//std::ifstream fs(f.c_str(),std::ios::in | std::ios::binary);
		f = wxFindNextFile();
		if(!fs.IsOpened())continue;
		fs.Read((void*)buf,BUF_SIZE);
		
		fs.Close();
		

		if(buf[128]!='D' || buf[129]!='I' || buf[130]!='C' || buf[131]!='M')continue;
		if(!size.x)
		{
			if( !FindInt(buf, 0x0028, 0x0011, size.x) ) continue;
			Progress::inst->AddText("width: "+str::ToString(size.x)+"<br>");
			if( !FindInt(buf, 0x0028, 0x0010, size.y) ) continue;
			Progress::inst->AddText("height: "+str::ToString(size.y)+"<br>");
			
			Progress::inst->AddText("Allocating memory ("+str::ToString((size.x*size.y*size.z*2)/(1024*1024))+"Mb)<br>");
			arri = new short[size.x*size.y];
			CPU_VD::full_data.Allocate(size,0);

			

//			int idd = FindStringInBuffer((char*)buf,"+0.00");
//			char tmp[100];
//			memcpy(tmp,buf+idd-20,100);


			if( FindString(buf, 0x0018, 0x0050, ss) )
			{
				slice_thickness = str::ToFloat(ss);
				Progress::inst->AddText("Slice thickness: "+ss+"<br>");
			}
			if( FindString(buf, 0x0018, 0x0088, ss) )
			{
				spacing_between_slices = str::ToFloat(ss);
				Progress::inst->AddText("!!!Spacing Between Slices: "+ss+"<br>");
			}
			if( FindString(buf, 0x0028, 0x0030, ss) )
			{
				Progress::inst->AddText("!!!Pixel Spacing: "+ss);
				svec tmpsv = str::split(ss,"\\");
				pixel_spacing[0] = str::ToFloat(tmpsv[0]);
				pixel_spacing[1] = str::ToFloat(tmpsv[1]);

				Progress::inst->AddText(" ok!<br>");

			}

			
			if( FindInt(buf, 0x0028, 0x0002 , bpp) )
			{
				Progress::inst->AddText("Samples per Pixel: "+str::ToString(bpp)+"<br>");
			}
			if( FindInt(buf, 0x0028, 0x0100 , bpp) )
			{
				Progress::inst->AddText("Bits Allocated: "+str::ToString(bpp)+"<br>");
			}
			if( FindInt(buf, 0x0028, 0x0102 , bpp) )
			{
				Progress::inst->AddText("High bit: "+str::ToString(bpp)+"<br>");
			}
			
			if( FindString(buf, 0x0018, 0x1100, ss) )
			{
				reconstruction_diameter = str::ToFloat(ss);
				
				Progress::inst->AddText("Reconstruction Diameter: "+ss+"<br>");
			}

			

			              
		}

		if(nn1<2)
		if( FindString(buf, 0x0020, 0x1041, ss) )
		{
			slice_location[nn1] = str::ToFloat(ss);
			if(nn1==1)
			{
				spacing_between_slices = abs(slice_location[1]-slice_location[0]);
				Progress::inst->AddText("spacing_between_slices: "+str::ToString(spacing_between_slices)+"<br>");
				if(reconstruction_diameter)
				{
					Progress::inst->AddText("{"+str::ToString(pixel_spacing[0]*size.x)+"x");
					Progress::inst->AddText(str::ToString(pixel_spacing[1]*size.y)+"x");
				Progress::inst->AddText(str::ToString(spacing_between_slices*size.z)+"mm}<br><br>");
				}
			}
		}

		int nnn=size.x;
		id = FindElement(buf,0x7FE0,0x0010);
		if(id==-1)continue;
		
		memcpy(CPU_VD::full_data.GetSlice(nn1),buf+id,size.x*size.y*2);
		//memcpy(arri,buf+id,size.x*size.y*2);
		//for(int ii=0;ii<size.x*size.y;ii++)			arri[ii] += value_offset;
		//memcpy(CPU_VD::full_data.GetSlice(nn1),arri,size.x*size.y*2);

		nn1++;
		Progress::inst->SetPercent(float(nn1)/nn);
		
	}
	//nn=512;
	//scale.z = (nn*spacing_between_slices/slice_thickness)/(float)size.x;
	//scale.z = (spacing_between_slices/slice_thickness);
	CPU_VD::full_data.spacing.x = pixel_spacing[1];
	CPU_VD::full_data.spacing.y = pixel_spacing[0];
	//scale.z = spacing_between_slices*size.z/(pixel_spacing[0]*size.x);
	CPU_VD::full_data.spacing.z = spacing_between_slices;
	if(!reconstruction_diameter)reconstruction_diameter = pixel_spacing[0]*size.x;
	output::application.println((const char*)Progress::inst->GetText().wx_str());
	
	delete[] buf;
	
	cur_dicom_dir = dicom_dir;

	CPU_VD::full_data.GetSize()=size;
	CPU_VD::scale = 1.0f/max(max(CPU_VD::full_data.spacing.x*size.x,CPU_VD::full_data.spacing.y*size.y),CPU_VD::full_data.spacing.z*size.z);

	data_stat.AnalyseData(CPU_VD::full_data);

	delete[] arri;
	return true;

}
}