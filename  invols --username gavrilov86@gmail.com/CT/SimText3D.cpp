#include "SimText3D.h"
#pragma comment(lib,"glew32.lib")

#include <windows.h>
#include "output.h"
//#include <GL/glu.h>

/*
GLenum texture_target=GL_TEXTURE_RECTANGLE_ARB;
GLenum internal_format=GL_RGBA32F_ARB;
GLenum texture_format=GL_RGBA;
*/
void checkGLErrors (const char *label);

void SimText3D::setupTexture(GLuint texID)
{
    glBindTexture(texture_target,texID);
/*
	glTexParameteri(texture_target, GL_GENERATE_MIPMAP, TRUE);
	glTexParameteri(texture_target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(texture_target, GL_TEXTURE_MAX_LEVEL, 4);
*/
	glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, linear?GL_LINEAR:GL_NEAREST);
    glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, linear?GL_LINEAR:GL_NEAREST);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_R, GL_CLAMP);

}


void SimText3D::Upload(void*arr1) 
{
    glBindTexture(texture_target, texture);

	static PFNGLTEXIMAGE3DPROC glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
	if(dim_num==1)glTexImage1D(texture_target,0,internal_format,width,0,texture_format,t_type,arr1);
	if(dim_num==2)glTexImage2D(texture_target,0,internal_format,width,height,0,texture_format,t_type,arr1);
	if(dim_num==3)glTexImage3D(texture_target,0,internal_format,width,height,depth,0,texture_format,t_type,arr1);

}
void SimText3D::transferToTexture(GLuint texID) 
{
	if(texID==-1)texID=texture;
    glBindTexture(texture_target, texID);

	static PFNGLTEXIMAGE3DPROC glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
	static PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) wglGetProcAddress("glTexSubImage3D");
	
//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

//To copy texels from the framebuffer, use glCopyTexSubImage2D. 
//glBindTexture(GL_TEXTURE_2D, textureID);    //A texture you have already created with glTexImage2D
//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);   //Copy back buffer to texture


//Then came the programmable GPU. There aren't texture units anymore. Today, you have texture samplers. These are also called texture image units (TIU) which you can get with 
//int MaxTextureImageUnits;
//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);

//For texture coordinates, you can get the max with 
//int MaxTextureCoords;
//glGetIntegerv(GL_MAX_TEXTURE_COORDS, &MaxTextureCoords);

	if(dim_num==1)glTexImage1D(texture_target,0,internal_format,width,0,texture_format,t_type,arr);
	if(dim_num==2)glTexImage2D(texture_target,0,internal_format,width,height,0,texture_format,t_type,arr);
	if(dim_num==3)
	{
		glTexImage3D(texture_target,0,internal_format,width,height,depth,0,texture_format,t_type,0);

		glTexSubImage3D(texture_target,0,0,0,0,width,height,depth,texture_format,t_type,arr);
	}
	GLenum errCode;
	const GLubyte *errString;
	 errCode=glGetError();
	if (errCode!=GL_NO_ERROR)
	{
		  errString=gluErrorString(errCode);
		  output::general.println((char*)errString);
	}
	
}

SimText3D::SimText3D(int dim_num1,int w,int h,int d,int pixel_size,void* data,int n_ID_to_use,bool n_linear,int nt_type):linear(n_linear),t_type(nt_type),dim_num(dim_num1)
{

	ID_to_use = 1;//n_ID_to_use;//GetFreeID();
	glActiveTexture(GL_TEXTURE0+ID_to_use);
	
	if(dim_num==1)texture_target=GL_TEXTURE_1D;
	if(dim_num==2)texture_target=GL_TEXTURE_2D;
	if(dim_num==3)texture_target=GL_TEXTURE_3D;

	glEnable(texture_target);
	arr=data;
	if(pixel_size==1)
	{
		
		if(t_type==GL_FLOAT)
		{
			internal_format=GL_ALPHA16F_ARB;
			texture_format=GL_ALPHA;
			//internal_format=GL_LUMINANCE16F_ARB;
			//texture_format=GL_LUMINANCE;//GL_ALPHA;
		}else
		if(t_type==GL_UNSIGNED_BYTE)
		{
			
			//internal_format=GL_ALPHA8;
			//texture_format=GL_ALPHA;
			internal_format=GL_LUMINANCE8;
			texture_format=GL_LUMINANCE;
			
		}else
		{
			texture_format=GL_LUMINANCE;

//texture_format=GL_ALPHA_INTEGER_EXT;
//internal_format=GL_ALPHA16I_EXT;

//texture_format=GL_LUMINANCE_INTEGER_EXT;
//internal_format=GL_LUMINANCE16I_EXT;
			internal_format=GL_LUMINANCE16;

			//internal_format=GL_ALPHA16F_ARB;
			//internal_format=GL_ALPHA16;
			//texture_format=GL_ALPHA;
		}
		
	}else
if(pixel_size==2)
	{
		
		if(t_type==GL_FLOAT)
		{
			//internal_format=GL_RGB16F_ARB;
			//texture_format=GL_RGB;
		}else
		if(t_type==GL_UNSIGNED_BYTE)
		{
			//internal_format=GL_RGB8;
			//texture_format=GL_RGB;
		}else
		{
			internal_format=GL_LUMINANCE16_ALPHA16;
			texture_format=GL_LUMINANCE_ALPHA;
		}
	}else
	if(pixel_size==3)
	{
		
		if(t_type==GL_FLOAT)
		{
			internal_format=GL_RGB16F_ARB;
			texture_format=GL_RGB;
		}else
		if(t_type==GL_UNSIGNED_BYTE)
		{
			internal_format=GL_RGB8;
			texture_format=GL_RGB;
		}else
		{
			internal_format=GL_RGB16;
			texture_format=GL_RGB;
		}
	}
	else
	{
		//if(pixel_size!=4)std::cout << "WARNING: Unable to create (" << pixel_size << ")-pixeled texture.";
		if(t_type==GL_FLOAT)
		{
			internal_format=GL_RGBA32F_ARB;
			texture_format=GL_RGBA;
		}else
		{
			internal_format=GL_RGBA8;
			texture_format=GL_RGBA;
		}
	}

	width=w;
	height=h;
	depth=d;

    glGenTextures(1, &texture);

    setupTexture(texture);


    transferToTexture(texture);

	
	glDisable(texture_target);
	glActiveTexture(GL_TEXTURE0);
};

SimText3D::~SimText3D()
{
//	free_ids[ID_to_use-1]=0;
    glDeleteTextures (1,&texture);
	//glDeleteFramebuffersEXT (1,&fb);
}
/*
GLuint SimText3D::GetTexture()
{
	return ID_to_use;
	//return texture;
}
*/

unsigned int SimText3D::SetID(int id)
{
	glActiveTexture(GL_TEXTURE0+id);
	glBindTexture(texture_target,texture);
	//glActiveTexture(GL_TEXTURE0);
	return id;
}

//рендеринг в текстуру dst программой sp c возвращением результата в arr
void RenderToTexture(SimText3D*src, ShaderProgram*sp,void*arr)
{
	/*
	GLuint fb;
    glGenFramebuffersEXT(1, &fb); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dst->texture,0);
    glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
    glActiveTexture(GL_TEXTURE0+dst->GetTexture());
    glBindTexture(GL_TEXTURE_2D, dst->texture);

//	checkFramebufferStatus();


	// viewport transform for 1:1 pixel=texel=data mapping
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0,1,0.0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0,0,dst->width,dst->height);
    
	sp->Use();

    glPolygonMode(GL_FRONT,GL_FILL);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);	        glVertex2f(0.0, 0.0);
    glTexCoord2f(1, 0.0);	        glVertex2f(1, 0.0);
    glTexCoord2f(1, 1);	glVertex2f(1, 1);
    glTexCoord2f(0.0, 1);	        glVertex2f(0.0, 1);
    glEnd();


//     checkFramebufferStatus();
	

	 if(arr)
	 {
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(0, 0, dst->width, dst->height,dst->texture_format,dst->t_type,arr);
	 }
	

	 sp->UnUse();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glActiveTexture(GL_TEXTURE0);

	//glPopMatrix();
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
*/
}