#include "MeshSection.h"
#include "AllDef.h"
#include "vec2.h"
#include <Math.h>
#include "AllInc.h"

bool LineExists(iv2vec& lines, ivec2 l)
{
	for(int i=0;i<lines.size();i++)
	if(lines[i].x!=-1)
	{
		if(l.x==lines[i].x && l.y==lines[i].y){lines[i].x=-1;return 1;}
		if(l.y==lines[i].x && l.x==lines[i].y){lines[i].x=-1;return 1;}
	}
	return 0;
		

}
bool was_error=0;

void add_sect(v2vec vert, iv2vec lines,iv3vec& res)
{
	
	static int ccc=0;
	static vec2 offset(-4);
	float scale=1;
	/*
	for(int i=0;i<lines.size();i++)
		if(lines[i].x!=-1)
		section2d.push_back(Line2(vec2(vert[lines[i].x]-vert[0]+offset)*scale,vec2(vert[lines[i].y]-vert[0]+offset)*scale));
	for(int i=0;i<res.size();i++)
	{
		vec3 v1 = vec3(vert[res[i].x]-vert[0]+offset,0)*scale;
		vec3 v2 = vec3(vert[res[i].y]-vert[0]+offset,0)*scale;
		vec3 v3 = vec3(vert[res[i].z]-vert[0]+offset,0)*scale;
//		vec3 ccc=(v1+v2+v3)/3;
//		v1=ccc+(v1-ccc)*0.95;
//		v2=ccc+(v2-ccc)*0.95;
//		v3=ccc+(v3-ccc)*0.95;
		neuron[2][2].AddTriangle(v1,v2,v3);
		neuron[2][2].AddTriangle(v1,v3,v2);
	}
	offset.x+=4/scale;
	
	ccc++;
	printf("err(%d)%d %d\n",ccc,vert.size(),lines.size());
	*/
	was_error=1;

}

// можно ли создавать треугольник со стороной cur_line и вершиной cur_vert из массивов vert и lines (начина€ с номера cur_line, т.к. предыдущие по идее алгоритма удалены) соответственно
// чтобы он был 1)внутри области, внутренним 2) без чужих вершин внутри 3) не пересекаемый никакими отрезками 
bool MayMesh(v2vec& vert, iv2vec lines,int cur_line,int cur_vert)
{
//	printf("_");
	bool was_cross_err=0;
	int a_id = lines[cur_line].x,b_id = lines[cur_line].y;
	vec2 pt = vert[cur_vert];
	vec2 a = vert[a_id];
	vec2 b = vert[b_id];
	if(abs(vec2::vect_mult(a-pt,b-pt))<=SMALL_VAL)return 0;
	vec2 center = (a+b+pt)/3;
	int cross_sum=0,cross_sum1=0;
	for(int i=0;i<lines.size();i++)//(3)
	if(lines[i].x!=-1)
	{

		//printf("{%d %d %d}",lines[i].x,lines[i].y,lines.size());
		ivec2 cl = lines[i];
		vec2 v1 = vert[cl.x];
		vec2 v2 = vert[cl.y];

		if(a_id!=cl.x && a_id!=cl.y)
			if(LineCrossLine(pt,a, v1, v2))
				if(abs(vec2::vect_mult(pt-a,v1-v2))>SMALL_VAL)
				return 0;

		if(b_id!=cl.x && b_id!=cl.y)
			if(abs(vec2::vect_mult(pt-b,v1-v2))>SMALL_VAL)
			if(LineCrossLine(pt,b, v1, v2))return 0;


		float tmpv = v2.y - v1.y;
		if(tmpv && !was_cross_err)
		{
			float tmpv1 = (center.y-v1.y)/tmpv;
			
			if(tmpv1>=0 && tmpv1<=1 && center.x<=v1.x+((v2.x-v1.x)*(center.y-v1.y))/tmpv) cross_sum++;
		}else was_cross_err=1;
		tmpv = v2.x - v1.x;
		if(tmpv && !was_cross_err)
		{
			float tmpv1 = (center.x-v1.x)/tmpv;
			
			if(tmpv1>=0 && tmpv1<=1 && center.y<=v1.y+((v2.y-v1.y)*(center.x-v1.x))/tmpv) cross_sum1++;
		}else was_cross_err=1;
	}
	if((cross_sum1&1)!=(cross_sum&1) || was_cross_err)
	{//если глюки: по ’ и по ” разна€ чЄтность пересечений. Ќадо по-лучше вычислить
		#define CR_NUM 6
		int cr[CR_NUM];
		//printf("{%d,%d}",cross_sum,cross_sum1);
		bool fin=0;
		while(!fin)
		{
			fin=1;
			
			
			for(int cc=0;cc<CR_NUM;cc++)
			{
				cr[cc]=0;
				vec2 center2(RND11,RND11);
				center2.normalize();
				center2 = center + center2*100;
				for(int i=0;i<lines.size();i++)//(3)
				if(lines[i].x!=-1)
				{
					if(LineCrossLine(center,center2, vert[lines[i].x], vert[lines[i].y]))cr[cc]++;
				}
				///printf(",%d",cr[cc]);
				if(cc)
					if((cr[cc-1]&1)!=(cr[cc]&1))
					{fin=0;printf("*");break;}
			}

		}
		cross_sum=cr[0];
	}
//	printf("%d",cross_sum);

	if(!(cross_sum&1))return 0;//(1)
//	printf("c");

	for(int i=0;i<vert.size();i++)//(2)
	if(i!=cur_vert && i!=a_id && i!= b_id)
		if(InsideTriangle(vert[i],a,b,pt))
			return 0;
//	printf("D ");

	return 1;
}
// записывает в res набор неперекрывающихс€ треугольников, полностью покрывающих замкнутые(!) контуры, определ€емые отрезками, заданными вершинами и индексами
// знак triangles_orientation определ€ет ориентацию треугольников
void MeshLines(v2vec vert, iv2vec lines, iv3vec& res,float triangles_orientation)
{
	/*
	iv2vec lines0=lines;
	int cur_line=0, i, lines_num = lines.size();

	//for(int j=0;j<vert.size();j++)		vert[j] += vec2(RND01*0.0001,RND01*0.0001);
	
	while(cur_line<lines.size())
	{
		ivec2 cl = lines[cur_line];
		if(cl.x!=-1)
		{
			for(i=0;i<vert.size();i++)
			if(i!=cl.x && i!=cl.y)
				if(MayMesh(vert,lines,cur_line,i))
				{
					res.push_back(ivec3(i,cl.x,cl.y));

					//if(res.size()==26)return;
					ivec2 l1(i,cl.x),l2(i,cl.y);
					if(!LineExists(lines,l1)) lines.push_back(l1);
					if(!LineExists(lines,l2)) lines.push_back(l2);
					lines[cur_line].x = -1;
					break;
				}
	

				if(i==vert.size()){add_sect(vert,lines0,res);printf("\n\nUnable to remove line\n");was_error=1;return;}
		}
		if(cur_line==5000){printf(" Overflow!!! ");return;}

		cur_line++;
	}
*/

	iv2vec lines0=lines;
	int cur_line=0, lines_num = 0;
	bool was_del;
	
	for(int i=0;i<lines.size();i++)		
		if(lines[i].x!=-1)lines_num++;

//	for(int j=0;j<vert.size();j++)		vert[j] += vec2(RND01*0.00001,RND01*0.00001);
//	for(int j=0;j<vert.size();j++){vert[j].x=vert[j].x*100;vert[j].y=vert[j].y*100;}
	while(lines_num)
	{
		cur_line=0;
		was_del=0;
		while(cur_line<lines.size())
		{
			ivec2 cl = lines[cur_line];
			if(cl.x!=-1)
			{
				for(int i=0;i<vert.size();i++)
				if(i!=cl.x && i!=cl.y)
					if(MayMesh(vert,lines,cur_line,i))
					{
						res.push_back(ivec3(i,cl.x,cl.y));

						//if(res.size()==26)return;
						ivec2 l1(i,cl.x),l2(i,cl.y);
						if(!LineExists(lines,l1)) {lines.push_back(l1);lines_num++;}else lines_num--;
						if(!LineExists(lines,l2)) {lines.push_back(l2);lines_num++;}else lines_num--;
						lines[cur_line].x = -1;
						lines_num--;
						was_del=1;
//						printf("*");

						break;
					}
		

					
			}

			cur_line++;
			if(cur_line==2000)
			{
				printf(" Overflow!!! ");
				return;
			}
		}
		if(!was_del)
		{
			add_sect(vert,lines0,res);printf("\n\nUnable to remove lines(%d)\n",lines_num);was_error=1;
			break;
		}
	}


	for(int i=0;i<res.size();i++)
	{
		vec2 v1 = vert[res[i].y]-vert[res[i].x];
		vec2 v2 = vert[res[i].z]-vert[res[i].x];
		if(vec2::vect_mult(v1,v2)*triangles_orientation<0)swap(res[i].x,res[i].y);
	}

}

inline bool vecs_near(vec2& a, vec2& b)
{
	return (abs(a.x-b.x)<=SMALL_VAL && abs(a.y-b.y)<=SMALL_VAL);
}

int find_vec2(vec2& el,std::vector<vec2>& arr)
{
	for(int i=0;i<arr.size();i++)
		if(vecs_near(el, arr[i]))return i;

	return -1;
}


inline bool vecs_near(vec3& a, vec3& b)
{
	return (abs(a.x-b.x)<=SMALL_VAL && abs(a.y-b.y)<=SMALL_VAL && abs(a.z-b.z)<=SMALL_VAL);
}

int find_vec3(vec3& el,std::vector<vec3>& arr)
{
	for(int i=0;i<arr.size();i++)
		if(vecs_near(el, arr[i]))return i;

	return -1;
}

