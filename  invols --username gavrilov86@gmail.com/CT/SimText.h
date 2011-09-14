#ifndef SIMTEXT_H
#define SIMTEXT_H

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

class SimText
{
public:
	SimText(int w,int h,int ID_to_Use,int type_);
	~SimText();

	
	void UploadTexture(float *arr);
	void UploadTexture1(float *arr);
	void DownloadTexture(float *arr);

	void StartRenderToTexture();
	void EndRenderToTexture();

	GLuint GetTexture();

	void transferToTexture(GLuint texID,float*arr);

	void setupTexture(GLuint texID);


	GLuint texture[2];

	int writeTex,ID_to_use,type;
	int width,height;

	//GLfloat *arr;

	GLenum texture_target;
	GLenum internal_format;
	GLenum texture_format;

	GLuint fb;

};


#endif