#include "CT.h"
#include "CPU_VD.h"
#include "Img3DProc.h"
#include "Progress.h"
#include "wxIncludes.h"
#include <wx/numdlg.h>
#include "DicomReader.h"


short f_to_s(float&f)
{
	return f*(256.0f*128.0f);
}

float s_to_f(short&s)
{
	return s/(256.0f*128.0f);
}
short* GetSubData(short* data,ivec3 size,ivec3& pos,ivec3& sub_size);


void ApplyFilter2(SimText3D *img3d,ShaderProgram* sp,ivec3 src_size,VData dst,GLuint t_type)
{

	ivec3 dst_size = dst.GetSize();

	GLuint texture_format=GL_LUMINANCE, internal_format=GL_LUMINANCE8;
	int d_size=1;
	if(t_type==GL_SHORT)
	{
		d_size=2;
		internal_format=GL_LUMINANCE16F_ARB;
	}

	GLuint fb,texture;
	glGenFramebuffersEXT(1, &fb); 

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,internal_format,dst_size.x,dst_size.y,0,texture_format,t_type,0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE); 

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture,0);


	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1,0.0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0,0,dst_size.x,dst_size.y);
	
	sp->Use();

	glDisable(GL_BLEND);
	glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture);

	Progress progress(wxT("Resampling"));

	sp->SetVar("f_text",img3d->SetID(1));
	
	sp->SetVar("cell_size0",vec3(1.0f/src_size.x,1.0f/src_size.y,1.0f/src_size.z));
	sp->SetVar("cell_size1",vec3(1.0f/dst_size.x,1.0f/dst_size.y,1.0f/dst_size.z));
	for(int k=0;k<dst_size.z;k++)
	{
		void *dst1=dst.GetSlice(k);
		float c_z = (k+0.5f)/dst_size.z;
//		glPolygonMode(GL_FRONT,GL_FILL);

		glBegin(GL_QUADS);
		glVertex3f(0.0, 0.0,c_z);
		glVertex3f(1, 0.0,c_z);
		glVertex3f(1, 1,c_z);
		glVertex3f(0.0, 1,c_z);
		glEnd();
		glFlush();
		glReadPixels(0, 0, dst_size.x, dst_size.y, texture_format,t_type,dst1);

		Progress::inst->SetPercent(c_z);
	}
	sp->UnUse();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT (1,&fb);
glActiveTexture(GL_TEXTURE0);
glDeleteTextures (1,&texture);
	glDrawBuffer (CT::gl_back_buffer_target);
	glReadBuffer (CT::gl_back_buffer_target);


	sp->UnUse();


	CT::Update_rtt();
}
void ApplyFilter2(SimText3D *img3d,ivec3 src_size,VData dst,GLuint t_type,std::string shader_filename)
{
	ShaderProgram* sp = new ShaderProgram("common_shaders//srf.vs",shader_filename.c_str());
	ApplyFilter2(img3d,sp,src_size,dst,t_type);
	delete sp;
}
void MakeBoundingCells(SimText3D *img3d,ivec3 src_size,void* dst,ivec3 dst_size,GLuint t_type,std::string shader_filename)
{
	RenderingMethod *rm = CT::iso->GetCurRM();
/*
	str::AddPair(repl,"$IsoNum",str::ToString(rm->isos_sum));
	str::AddPair(repl,"$QuadNum",str::ToString(rm->quads_sum));
	str::AddPair(repl,"$VD_NUMBER",str::ToString(CT::GetVolumesNumber()));
	str::AddPair(repl,"$tf_type",str::ToString((int)rm->tf_type));
	
*/

	ShaderProgram* ps = new ShaderProgram("common_shaders//srf.vs",shader_filename.c_str());
	ps->Use();
	ps->SetVar("box1",CT::iso->GetBoundingBox(0));
	ps->SetVar("box2",CT::iso->GetBoundingBox(1));

	for(int i=0;i<1;i++)
	{
		std::string idd = "["+str::ToString(i)+"]";
		ps->SetVar1("min_level"+idd,rm->min_level[i]);
		ps->SetVar1("max_level"+idd,rm->max_level[i]);

		CS3 cc11 = CT::iso->volume_transform[i];
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
//		ps->SetVar1("f_text"+idd,CT::GetData(i)->SetDataTextureID(VD_TXT_ID+i));
	}

	ps->UnUse();
//	ApplyFilter2(img3d,ps,src_size,dst,t_type);
	delete ps;
}

void ApplyFilter1(void*src,ivec3 src_size,void*dst,ivec3 dst_size,GLuint t_type,std::string shader_filename)
{


	GLuint texture_format=GL_LUMINANCE, internal_format=GL_LUMINANCE8;
	int d_size=1;
	if(t_type==GL_SHORT)
	{
		d_size=2;
		internal_format=GL_LUMINANCE16F_ARB;
	}

	ShaderProgram* sp = new ShaderProgram("common_shaders//srf.vs",shader_filename.c_str());
	SimText3D *img3d = new SimText3D(3,src_size.x,src_size.y,src_size.z,1,src,3,1,t_type);
	GLuint fb,texture;
	glGenFramebuffersEXT(1, &fb); 

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,internal_format,dst_size.x,dst_size.y,0,texture_format,t_type,0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE); 

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture,0);


	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1,0.0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0,0,dst_size.x,dst_size.y);
	
	void *dst1=dst;
	sp->Use();

	glDisable(GL_BLEND);
	glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture);

	Progress progress(wxT("Resampling"));

	sp->SetVar("f_text",img3d->SetID(1));
	
	sp->SetVar("cell_size0",vec3(1.0f/src_size.x,1.0f/src_size.y,1.0f/src_size.z));
	sp->SetVar("cell_size1",vec3(1.0f/dst_size.x,1.0f/dst_size.y,1.0f/dst_size.z));
	for(int k=0;k<dst_size.z;k++)
	{
		float c_z = (k+0.5f)/dst_size.z;
//		glPolygonMode(GL_FRONT,GL_FILL);

		glBegin(GL_QUADS);
		glVertex3f(0.0, 0.0,c_z);
		glVertex3f(1, 0.0,c_z);
		glVertex3f(1, 1,c_z);
		glVertex3f(0.0, 1,c_z);
		glEnd();
		glFlush();
		glReadPixels(0, 0, dst_size.x, dst_size.y, texture_format,t_type,dst1);
		dst1 = (char*)dst1 + dst_size.x*dst_size.y*d_size;

		Progress::inst->SetPercent(c_z);
	}
	sp->UnUse();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT (1,&fb);
glActiveTexture(GL_TEXTURE0);
glDeleteTextures (1,&texture);
	glDrawBuffer (CT::gl_back_buffer_target);
	glReadBuffer (CT::gl_back_buffer_target);


	delete img3d;
	sp->UnUse();
	delete sp;


	CT::Update_rtt();
}

namespace segmentation
{
	void Resample()
	{


	}



	void MedianFilter(VData vd)
	{
		VData tmp_vd(vd.GetSize());
		long kernel_radius = 1;//wxGetNumberFromUser( "Enter the kernel radius",										_T("Enter a number:"), _T("Numeric input"),										1, 1, 10, 0 );
		
		if ( kernel_radius <= 0 )return;
		Progress progress(wxT("Noise reduce"));

		ivec3 size = vd.GetSize();
		ivec3 b1 = ivec3::Max(ivec3(kernel_radius),ivec3(0));
		ivec3 b2 = ivec3::Min(size-ivec3(kernel_radius+1),vd.GetSize());
		ivec3 id,jd;
		int ker_size = (kernel_radius*2+1);
		ker_size *=ker_size*ker_size;


		short*ker = new short[ker_size];

		for(id.z=b1.z;id.z<=b2.z;id.z++)
		{
		for(id.y=b1.y;id.y<=b2.y;id.y++)
		for(id.x=b1.x;id.x<=b2.x;id.x++)
		{
			vec2 grad(0);
			float sum_w=0;
			

			short*ker1 = ker;
			for(jd.z=id.z-kernel_radius;jd.z<=id.z+kernel_radius;jd.z++)
			for(jd.y=id.y-kernel_radius;jd.y<=id.y+kernel_radius;jd.y++)
			for(jd.x=id.x-kernel_radius;jd.x<=id.x+kernel_radius;jd.x++)
			{
				*ker1 = vd.GetValue(jd);
				ker1++;
			}

			for(int i=0;i<ker_size-1;i++)
				for(int j=i+1;j<ker_size;j++)
					if(ker[i]>ker[j])
					{
						short tmp = ker[i];
						ker[i] = ker[j];
						ker[j] = tmp;
					}


			vd.SetValue( ker[ker_size/2], id);

			
		}
		Progress::inst->SetPercent((id.z-b1.z)/(float)(b2.z-b1.z));
		}
	
		vd.SwapWith(tmp_vd);
		vd.Clear();
		
		
		delete []ker;
	}
	
	void GaussFilter(VData vd)
	{
		VData tmp_vd(vd.GetSize());
		long kernel_radius = 1;//wxGetNumberFromUser( "Enter the kernel radius",										_T("Enter a number:"), _T("Numeric input"),										1, 1, 10, 0 );
		
		if ( kernel_radius <= 0 )return;
		Progress progress(wxT("Noise reduce"));

		ivec3 size = vd.GetSize();
		ivec3 b1 = ivec3::Max(ivec3(kernel_radius),ivec3(0));
		ivec3 b2 = ivec3::Min(size-ivec3(kernel_radius+1),vd.GetSize());
		ivec3 id,jd;
		int ker_size = (kernel_radius*2+1);
		ker_size *=ker_size*ker_size;



		for(id.z=b1.z;id.z<=b2.z;id.z++)
		{
		for(id.y=b1.y;id.y<=b2.y;id.y++)
		for(id.x=b1.x;id.x<=b2.x;id.x++)
		{
			vec2 grad(0);
			double sum_w=0;
			double summ=0;
			
			for(jd.z=id.z-kernel_radius;jd.z<=id.z+kernel_radius;jd.z++)
			for(jd.y=id.y-kernel_radius;jd.y<=id.y+kernel_radius;jd.y++)
			for(jd.x=id.x-kernel_radius;jd.x<=id.x+kernel_radius;jd.x++)
			{
				float ww = 1.0f/(1+(jd-id).lengthSQR());
				summ += vd.GetValue(jd)*ww;
				sum_w += ww;
			}

			vd.SetValue( summ/sum_w, id);

			
		}
		Progress::inst->SetPercent((id.z-b1.z)/(float)(b2.z-b1.z));
		}
	
		vd.SwapWith(tmp_vd);
		vd.Clear();
		
		
	}
};

