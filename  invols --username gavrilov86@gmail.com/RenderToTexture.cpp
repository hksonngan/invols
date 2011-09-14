#include "RenderToTexture.h"
#include "CT.h"

#define TXT_NUM 4
//0)fast view
//1)front faces dist
//2)back faces dist
//3)temp calc for dist

GLuint texture_format[TXT_NUM], internal_format[TXT_NUM], t_type[TXT_NUM];
GLuint fb,texture[TXT_NUM]={0},depth_rb;

int width, height;

void rtt_Init()
{
	texture_format[0] = GL_RGBA;
	internal_format[0] = GL_RGBA8;
	t_type[0] = GL_UNSIGNED_BYTE;

	for(int i=1;i<TXT_NUM;i++)
	{
		texture_format[i] = GL_RGBA;
		internal_format[i] = GL_RGBA32F_ARB;
		t_type[i] = GL_FLOAT;
		
	

	}
	glGenFramebuffersEXT(1, &fb); 
	glGenRenderbuffersEXT(1, &depth_rb);

}

void rtt_Cleanup()
{
	//if(fb)
		//glDeleteFramebuffersEXT (1,&fb);
	if(texture[0])glDeleteTextures (TXT_NUM,texture);
}
vec2 rtt_GetTextureSize()
{
	return vec2(width,height);
}
void rtt_Resize(int w,int h)
{
	if(	width==w &&	height==h)return;
	rtt_Update(w,h);

}
void rtt_Update(int w,int h)
{
	width=w;
	height=h;
	glActiveTexture(GL_TEXTURE0);

	rtt_Cleanup();


	glGenTextures(TXT_NUM, texture);
	//for(int i=TXT_NUM-1;i>=0;i--)
	for(int i=0;i<TXT_NUM;i++)
	{
		glBindTexture(GL_TEXTURE_2D,texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D,0,internal_format[i],width,height,0,texture_format[i],t_type[i],0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE); 
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	//Attach depth buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);

	//Does the GPU support current FBO configuration?
		  GLenum status;
		  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		  //if(status==GL_FRAMEBUFFER_COMPLETE_EXT)			  HANDLE_THE_ERROR
		  


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

}
void rtt_Begin(int txt_id)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,texture[txt_id]);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture[txt_id],0);
	glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);

}
void rtt_End()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer (CT::gl_back_buffer_target);
	glActiveTexture(GL_TEXTURE0);
}
GLuint rtt_GetTexture(int txt_id)
{
	return texture[txt_id];
}

GLuint rtt_SetID(GLuint id,GLuint txt_id)
{
	glActiveTexture(GL_TEXTURE0+id);
	glBindTexture(GL_TEXTURE_2D,txt_id);
	return id;
}