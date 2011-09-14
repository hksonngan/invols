#include <string.h>
#include "AllInc.h"
#include "drawtext.h"


///
void init(void)
{
  // glShadeModel(GL_FLAT);
  // makeRasterFont();
}

void DrawText(std::string text,vec2 pos,vec4 color)
{
	static bool inited=0;
	if(!inited)init();

	glColor4fv(&color.x);
	glRasterPos2fv(&pos.x);
//	printString(text.c_str());

}
