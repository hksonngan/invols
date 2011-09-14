#ifndef GEOM_INC
#define GEOM_INC

#include "GeomFunc.h"
#include "mat34.h"
#include "vec4.h"
#include "vec.h"



struct Boxes
{
	ivec boxes;
	vec3 v[2];
};

class Geometry
{
public:
//	Martix mat;

	Geometry();
	Geometry(vec4 cl);
	//Geometry(const Geometry& g);
	void Copy(const Geometry& g);
	


	void BuildRep1();
	void BuildRep2();

	bool CrossLine(vec3 a,vec3 b,vec3* res);

	bool Cross(Geometry& g);//�������� ����������� � ����������(���������� �������������)
	void AddTriangle(vec3 a,vec3 b,vec3 c);
	void AddQuad(vec3 a,vec3 b,vec3 c,vec3 d);
	void AddTriangle(Triangle&tr1);
	void Add(Geometry& g);
	bool Inside(vec3 pt);
	
	float CalcVolume();
	float CalcArea();

	void SetNormOutOf(vec3 c);//������������� �������, ��������� �� ����� c

	void Move(vec3 d);
	void Move2(vec3 d);
	void Transform(mat34& mat);
	
	void Draw();
	void Draw2();
	void Draw3();
	void DrawBoxes();
	void renull();

	void InvertNormals();//������������� �������

	void RebuildTrBB();
	void RebuildTrBB2();

	bool visible;
	vec4 color;
	
	Triangles tr;
	Lines tr_bb;
	std::vector<Boxes> boxes;
	
	v3vec vert;
	v3vec norm;
	iv3vec face;

	
	void UpdateBox();//�������� bounding box(����������� ��������� ��������������)
	vec3 box1,box2;//bounding box
	bool box_updated;//bounding box ������� ����������


};
bool CrossBoxes(vec3 a1,vec3 a2,vec3 b1,vec3 b2); //����������� ������� a � b
bool FlatCrossBox(Line box,flat& fl); //����������� ������� box c ����������

float TriangleArea(Triangle& tr);

#endif