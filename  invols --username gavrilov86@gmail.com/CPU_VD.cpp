#include "CPU_VD.h"
#include "CT.h"

namespace CPU_VD
{
	VData full_data;
	
	ivec3 gpu_size=ivec3(0);
	ivec3 gpu_size_dummy=ivec3(512);
	ivec3 gpu_offset=ivec3(0);
	
	float scale=1;

	vec3 real_gpu_b1(0),real_gpu_b2(0);

	vec3 GetScale()
	{
		return (full_data.spacing*full_data.GetSize().ToVec3())*scale;
	}
	vec3 GetFullSize()
	{
		return (full_data.spacing*full_data.GetSize().ToVec3())*scale;
	}

	void CalcReal()
	{
		CPU_VD::real_gpu_b1 = ((CPU_VD::gpu_offset.ToVec3())/CPU_VD::full_data.GetSize().ToVec3())*CPU_VD::GetScale();
		CPU_VD::real_gpu_b2 = (((CPU_VD::gpu_offset+CPU_VD::gpu_size).ToVec3())/CPU_VD::full_data.GetSize().ToVec3())*CPU_VD::GetScale();
		CT::need_rerender=1;
		CT::need_rerender2d=1;
	}
	void UnInit()
	{
		full_data.Clear();
	}
	
	ivec3 GetRecomendedGPUOffset()
	{
		ivec3 pos(CT::GetCenter().x*CPU_VD::full_data.GetSize().x, CT::GetCenter().y*CPU_VD::full_data.GetSize().y, CT::GetCenter().z*CPU_VD::full_data.GetSize().z);
		pos -= gpu_size_dummy/2;
		pos -= ivec3::Max(gpu_size_dummy+pos-CPU_VD::full_data.GetSize(),ivec3(0));
		pos = ivec3::Max(ivec3(0),pos);
		return pos;
	}
	
	ivec3 GetIPos(vec3 pos)
	{
		return (pos*CPU_VD::full_data.GetSize().ToVec3()).ToIVec3();
	}

}