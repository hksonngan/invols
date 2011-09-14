#ifndef REGION_INC
#define REGION_INC

#include <stdio.h>
#include <stdlib.h>
#include "AllDef.h"
#include "AllInc.h"
#include "vec3.h"
#include <vector>

//������ � ������������
class Region
{
	public:
	virtual vec3 GetRand()=0;//����. ����� �� �������
	virtual bool InRegion(vec3& v)=0;//�� �������?
	virtual ~Region(){}
};

	//�������, ������������ ������������ ���������������
	//����� ��� ��������� ������ ����� �� ������, �.�. �������� �������������
class Flats: public Region
{
public:
	Flats(vec3 pos,vec3 nv1,vec3 nv2,vec3 nv3,vec3 nv4);
	virtual vec3 GetRand();
	virtual bool InRegion(vec3& v);
	void AddFlat(flat&nf);
private:
	std::vector<flat> flats;

};

#endif
