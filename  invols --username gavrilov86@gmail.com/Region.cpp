#include "Region.h"

	vec3 Flats::GetRand()
	{
		return vec3(0);
	}
	bool Flats::InRegion(vec3& v)
	{
		flat* fl;
		for(int i=0;i<flats.size();i++)
		{
			fl=&flats[i];
			if(vec3::dot(fl->n,(v-fl->d))<0)return false;
		}
		return true;
	}

	void Flats::AddFlat(flat&nf)
	{
		flats.push_back(nf);
	}
	Flats::Flats(vec3 pos,vec3 nv1,vec3 nv2,vec3 nv3,vec3 nv4)
	{
		int i;
		vec3 mid=nv1+nv2+nv3+nv4;
		flat fl;
		vec3 nv[4];
		nv[0]=nv1;
		nv[1]=nv2;
		nv[2]=nv3;
		nv[3]=nv4;

		for(i=0;i<4;i++)
		{
			fl.d=pos;
			fl.n=vec3::vect_mult(nv[i],nv[(i+1)&3]);
			if(vec3::dot(fl.n,mid)<0)fl.n=fl.n*(-1.0f);

			AddFlat(fl);
		}

	}