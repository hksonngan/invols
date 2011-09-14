#ifndef SIMTEXT3D_H
#define SIMTEXT3D_H

//#include "define.h"

#include <vector>
#include <assert.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <ctime>

#include "Shader.h"


typedef short field_type;
#define gl_field_type GL_SHORT

/*
typedef float field_type;
#define gl_field_type GL_FLOAT
*/
class SimText3D
{
public:
	SimText3D(int dim_num1, int w,int h,int d, int pixel_size,void* data,int n_ID_to_use,bool n_linear,int nt_type);
	~SimText3D();

	

	int ID_to_use;

	//GLuint GetTexture();
	unsigned int SetID(int id);

	void Upload(void*arr1);

	int width,height,depth;

	GLuint texture;
//private:
	int dim_num;
	void transferToTexture(GLuint texID=-1);
	

	void setupTexture(GLuint texID);

//	const GLchar*tex_name;


//	int LoD;
	bool linear;

	void *arr;

	GLenum t_type;
	GLenum texture_target;
	GLenum internal_format;
	GLenum texture_format;


//	GLuint fb;
};

//void RenderToTexture(SimText3D*src, ShaderProgram*sp,void*arr=0);

#endif