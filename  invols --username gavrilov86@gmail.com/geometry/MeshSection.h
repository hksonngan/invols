#ifndef MESH_SECTION_H
#define MESH_SECTION_H

#include "GeomFunc.h"
#include "Geometry.h"

#define SMALL_VAL 0.00001f

struct LineConf
{
	char v[4];
};

// ���������� ����� �������, ����������� ������������ g1 � g2. ������ ������������ � res
//void GetIntersection(Triangle& t1,Triangle& t2,Lines& res,std::vector<LineConf>* lines_conf);
void GetIntersection(Geometry*g1,Geometry*g2,Lines& res);
void FlatCrossGeometry(Geometry& g,flat fl,Lines& res,float*perimeter);

// ���������� � res ����� ��������� target, ���������� section. inside_section ����������, � ����� ������� section ����� ��������
void GetSection(Geometry*target,Geometry*section,Geometry*res,bool inside_section,bool back_faces,float*perimeter=0);

void GetAorB(Geometry*a,Geometry*b,Geometry*res);
void GetAandB(Geometry*a,Geometry*b,Geometry*res);
void GetAnotB(Geometry*a,Geometry*b,Geometry*res);


// ���������� ����� ����������������� �������������, ��������� ����������� ���������(!) �������, ������������ ���������, ��������� ��������� � ���������
void MeshLines(v2vec vert, iv2vec lines, iv3vec& res,float triangles_orientation);
//�������� �� ������������ tr �� �������� lines3d. include_v0 ����������, �������� �� ������ ������� ������������ ����� �� ������ ����������
void CutTriangle(Triangle& tr, Lines& lines3d,std::vector<LineConf>& lines_conf, bool include_v0,Geometry* res,bool back_faces);


template<class T>
int find_element(T& el,std::vector<T>& arr)
{
	for(int i=0;i<arr.size();i++)
		if(el==arr[i])return i;
	return -1;
}

//������������ ��������� ��� ����� � ��������� ����� � ��� ��������
void ConvertLines(Lines2& src_lines, v2vec& dst_vert, iv2vec& dst_lines);
void ConvertLines(Lines& src_lines, v3vec& dst_vert, iv2vec& dst_lines);

inline bool vecs_near(vec2& a, vec2& b);
inline bool vecs_near(vec3& a, vec3& b);
int find_vec2(vec2& el,std::vector<vec2>& arr);
int find_vec3(vec3& el,std::vector<vec3>& arr);


#endif