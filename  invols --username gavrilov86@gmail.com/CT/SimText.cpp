#include "SimText.h"
#include "output.h"
#include "AllInc.h"
#pragma comment(lib,"glew32.lib")

/*
GLenum texture_target=GL_TEXTURE_2D;
GLenum internal_format=GL_RGBA32F_ARB;
GLenum texture_format=GL_RGBA;
*/
void checkGLErrors (const char *label);

bool checkFramebufferStatus() {
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
	    output::general.println("Framebuffer incomplete, incomplete attachment");
            return false;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
	    output::general.println("Unsupported framebuffer format");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
	    output::general.println("Framebuffer incomplete, missing attachment");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
	    output::general.println("Framebuffer incomplete, attached images must have same dimensions");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
	    output::general.println("Framebuffer incomplete, attached images must have same format");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
	    output::general.println("Framebuffer incomplete, missing draw buffer");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
	    output::general.println("Framebuffer incomplete, missing read buffer");
            return false;
    }
    return false;
}

/**
 * Sets up a floating point texture with NEAREST filtering.
 * (mipmaps etc. are unsupported for floating point textures)
 */
void SimText::setupTexture(GLuint texID)
{
    // make active and bind
    glBindTexture(texture_target,texID);
    // turn off filtering and wrap modes
    glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // define texture with floating point format
    glTexImage2D(texture_target,0,internal_format,width,height,0,texture_format,GL_FLOAT,0);
}


/**
 * Transfers data to texture. 
 * Check web page for detailed explanation on the difference between ATI and NVIDIA.
 */
void SimText::transferToTexture(GLuint texID,float*arr) 
{
	if(texID==-1)texID=texture[!writeTex];
    glBindTexture(texture_target, texID);
    glTexSubImage2D(texture_target,0,0,0,width,height,texture_format,GL_FLOAT,arr);
	
}
/*
void SimText::Attach(SimText* txt)
{
	glUniform1i(shader_program->getUniLoc(txt->tex_name),txt->ID_to_use);
}
*/
SimText::SimText(int w,int h,int ID_to_Use,int type_):ID_to_use(ID_to_Use),type(type_)
{
	writeTex=1;
	texture_target=GL_TEXTURE_2D;
	internal_format=GL_RGBA32F_ARB;
	texture_format=GL_RGBA;


	width=w;
	height=h;
    // create FBO (off-screen framebuffer)
    glGenFramebuffersEXT(1, &fb); 
    // bind offscreen framebuffer (that is, skip the window-specific render target)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

    glGenTextures(2, texture);

    setupTexture(texture[0]);
    setupTexture(texture[1]);
    
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE); 
    

    // check if something went completely wrong
    checkGLErrors ("createFBOandTextures()");
	//Simulation();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
};

SimText::~SimText()
{
    glDeleteTextures (2,texture);
    glDeleteFramebuffersEXT (1,&fb);
}
/*
void SimText::Simulation(float* arr)
{
 glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    // attach texture

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture_target, texture[writeTex],0);

    // viewport transform for 1:1 pixel=texel=data mapping
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0,1,0.0,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0,0,width,height);
    
    glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
    //glUseProgram(shader.h_program);
	shader_program->Use();

    glActiveTexture(GL_TEXTURE0+ID_to_use);
    glBindTexture(texture_target, texture[!writeTex]);
	shader_program->SetVar(tex_name,(unsigned)ID_to_use);
    

    glPolygonMode(GL_FRONT,GL_FILL);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);	        glVertex2f(0.0, 0.0);
    glTexCoord2f(1, 0.0);	        glVertex2f(1, 0.0);
    glTexCoord2f(1, 1);	glVertex2f(1, 1);
    glTexCoord2f(0.0, 1);	        glVertex2f(0.0, 1);
    glEnd();


     checkFramebufferStatus();
	
    checkGLErrors(tex_name);

	if(arr)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(0, 0, width, height,texture_format,GL_FLOAT,arr);
	}

    glUseProgram(0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glActiveTexture(GL_TEXTURE0);

    // swap textures
    writeTex=!writeTex;

	//glPopMatrix();
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
}*/
void SimText::UploadTexture(float *arr)
{
	glActiveTexture(GL_TEXTURE0+ID_to_use);
    setupTexture(texture[!writeTex]);
    transferToTexture(texture[!writeTex],arr);
	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(texture_target,texture[!writeTex]);
//	gluBuild2DMipmaps(texture_target, 4, width,height, texture_format, GL_FLOAT,arr);


}
void SimText::UploadTexture1(float *arr)
{
	glActiveTexture(GL_TEXTURE0+ID_to_use);
    setupTexture(texture[writeTex]);
    transferToTexture(texture[writeTex],arr);
	glActiveTexture(GL_TEXTURE0);


}
void SimText::DownloadTexture(float *arr)
{
	glLoadIdentity();
	glDisable(GL_BLEND);

//	shader_program->Use();
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadPixels(0, 0, width, height,texture_format,GL_FLOAT,arr);

  //  glUseProgram(0);

	glEnable(GL_BLEND);
}
GLuint SimText::GetTexture()
{
	return texture[!writeTex];
}

void SimText::StartRenderToTexture()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture_target, texture[writeTex],0);
    glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
    glActiveTexture(GL_TEXTURE0+ID_to_use);
    glBindTexture(texture_target, texture[!writeTex]);

	checkFramebufferStatus();
//    checkGLErrors(tex_name);
  
    glDeleteFramebuffersEXT (1,&fb);
}
void SimText::EndRenderToTexture()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glActiveTexture(GL_TEXTURE0);
}

/////////////////////////////////////////////////////

class Text
{
public:
	void UploadTexture(void *arr);
	Text(int w,int h,void *arr,int pixel_size_,int t_type_,bool line_interp_);
	~Text();
	GLuint GetTexture();
private:
	void transferToTexture(GLuint texID,void*arr);

	void setupTexture(GLuint texID);

	int pixel_size,t_type;
bool line_interp;
	GLuint texture;
	int width,height;
	GLenum texture_target;
	GLenum internal_format;
	GLenum texture_format;

};

void Text::setupTexture(GLuint texID)
{
    // make active and bind
    glBindTexture(texture_target,texID);
    // turn off filtering and wrap modes
	glTexParameteri(texture_target, GL_TEXTURE_MIN_FILTER, line_interp?GL_LINEAR:GL_NEAREST);
    glTexParameteri(texture_target, GL_TEXTURE_MAG_FILTER, line_interp?GL_LINEAR:GL_NEAREST);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // define texture with floating point format
    glTexImage2D(texture_target,0,internal_format,width,height,0,texture_format,t_type,0);
}

void Text::transferToTexture(GLuint texID,void*arr) 
{
	glBindTexture(texture_target, texID);
    glTexSubImage2D(texture_target,0,0,0,width,height,texture_format,t_type,arr);
	
}
Text::Text(int w,int h,void *arr,int pixel_size_,int t_type_,bool line_interp_):pixel_size(pixel_size_),t_type(t_type_),line_interp(line_interp_)
{
	texture_target=GL_TEXTURE_2D;
	if(pixel_size==1)
	{
		
		if(t_type==GL_FLOAT)
		{
			internal_format=GL_LUMINANCE32F_ARB;
			texture_format=GL_LUMINANCE;
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
    glGenTextures(1, &texture);
    setupTexture(texture);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE); 
	if(arr)UploadTexture(arr);
};

Text::~Text()
{
    glDeleteTextures (1,&texture);
}

void Text::UploadTexture(void *arr)
{
    setupTexture(texture);
    transferToTexture(texture,arr);
	//glBindTexture(texture_target,texture);
	//gluBuild2DMipmaps(texture_target, 4, width,height, texture_format, GL_FLOAT,arr);


}
GLuint Text::GetTexture()
{
	return texture;
}