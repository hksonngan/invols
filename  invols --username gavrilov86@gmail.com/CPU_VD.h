#ifndef CPU_VD_H
#define CPU_VD_H

#include "vec3.h"
#include "ivec3.h"
#include "VolumeData.h"

namespace CPU_VD
{
	ivec3 GetIPos(vec3 pos);

	void CalcReal();
	ivec3 GetRecomendedGPUOffset();
	void UnInit();
	vec3 GetScale();
	vec3 GetFullSize();

	extern 	VData full_data;

	extern ivec3 gpu_size,gpu_offset,gpu_size_dummy;
	
/////
	extern vec3 real_gpu_b1,real_gpu_b2;
	extern float scale;
}

#endif
