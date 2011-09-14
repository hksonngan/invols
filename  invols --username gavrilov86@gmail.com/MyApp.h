#ifndef _WX_MY_APP_H_
#define _WX_MY_APP_H_

#include "wxIncludes.h"

class MainFrame;

class MyApp: public wxApp
{
public:
    bool OnInit();

	static MainFrame *frame;
	static wxGLContext* GLContext;
	static int*gl_attributes;

};


#endif