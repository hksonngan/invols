#ifndef RTT_INC
#define RTT_INC

#include "AllInc.h"
#include "Render.h"
#include "wxIncludes.h"

	void rtt_Cleanup();

	void rtt_Update(int w,int h);

	void rtt_Begin(int txt_id);

	void rtt_End();

	GLuint rtt_GetTexture(int txt_id);

	vec2 rtt_GetTextureSize();

	void rtt_Resize(int w,int h);
	void rtt_Init();
	GLuint rtt_SetID(GLuint id,GLuint txt_id);

#endif