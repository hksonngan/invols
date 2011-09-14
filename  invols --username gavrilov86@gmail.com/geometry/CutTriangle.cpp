
#include "MeshSection.h"
#include "AllDef.h"
#include "vec2.h"
#include <Math.h>
#include "AllInc.h"

extern bool was_error;

void ConvertLines(Lines2& src_lines, v2vec& dst_vert, iv2vec& dst_lines)
{

	for(int i=0;i<src_lines.size();i++)
	{
		vec2 a = src_lines[i].v[0];
		vec2 b = src_lines[i].v[1];
		ivec2 ll(find_vec2(a,dst_vert),find_vec2(b,dst_vert));
		if(ll.x==-1)
		{
			dst_vert.push_back(a);
			ll.x=dst_vert.size()-1;
		}
		if(ll.y==-1)
		{
			dst_vert.push_back(b);
			ll.y=dst_vert.size()-1;
		}
		
		dst_lines.push_back(ll);
	}

}

void ConvertLines(Lines& src_lines, v3vec& dst_vert, iv2vec& dst_lines)
{

	for(int i=0;i<src_lines.size();i++)
	{
		vec3 a = src_lines[i].v[0];
		vec3 b = src_lines[i].v[1];
		ivec2 ll(find_vec3(a,dst_vert),find_vec3(b,dst_vert));
		if(ll.x==-1)
		{
			dst_vert.push_back(a);
			ll.x=dst_vert.size()-1;
		}
		if(ll.y==-1)
		{
			dst_vert.push_back(b);
			ll.y=dst_vert.size()-1;
		}
		
		dst_lines.push_back(ll);
	}

}
// если есть два смежных параллельных отрезка - объедин€ем их в один отрезок
void OptimizeLines(v2vec& vert,iv2vec& lines,v3vec& vert3d)
{
	vec del_vert;
	
	
	for(int i=0;i<lines.size()-1;i++)
	if(lines[i].x!=-1)
	{
		vec2 v1 = vert[lines[i].x]-vert[lines[i].y];
		int j=i+1;
		while(j<lines.size())
		{
			if(lines[j].x!=-1)
			if(lines[i].x==lines[j].x || lines[i].x==lines[j].y || lines[i].y==lines[j].x || lines[i].y==lines[j].y)
			{
				
				vec2 v2 = vert[lines[j].x]-vert[lines[j].y];
				if(abs(v1.x*v2.y-v1.y*v2.x)<=SMALL_VAL)
				//	if(abs(v1.length()+v2.length()-(v1+v2).length())<=SMALL_VAL)
				{
					
					if(lines[i].x==lines[j].x) {del_vert.push_back(lines[i].x);lines[i].x = lines[j].y;}else
					if(lines[i].x==lines[j].y) {del_vert.push_back(lines[i].x);lines[i].x = lines[j].x;}else
					if(lines[i].y==lines[j].x) {del_vert.push_back(lines[i].y);lines[i].y = lines[j].y;}else
					if(lines[i].y==lines[j].y) {del_vert.push_back(lines[i].y);lines[i].y = lines[j].x;}
					lines[j].x=-1;
					j=i+1;
//					v1 = vert[lines[i].x]-vert[lines[i].y];
					continue;
				}

			}
			j++;
			

		}
		
	}
	if(del_vert.size())
	{
		v2vec vert1;
		v3vec vert3d1;
		iv2vec lines1=lines;
		for(int j=0;j<vert.size();j++)
		{
			int i=0;
			for(;i<del_vert.size();i++)
			{
				if(j==del_vert[i]){break;}
			}
			if(del_vert.size()==i){vert1.push_back(vert[j]);vert3d1.push_back(vert3d[j]);}
		}
		
		vert=vert1;
		vert3d=vert3d1;
		for(int i=0;i<del_vert.size();i++)
		{
			int id = del_vert[i];
			for(int j=0;j<lines.size();j++)
				if(lines[j].x!=-1)
				{
					if(lines[j].x>=id)lines1[j].x--;
					if(lines[j].y>=id)lines1[j].y--;
				}
		}
		lines=lines1;
	}
	
	
//	for(int i=0;i<lines.size();i++)		if(lines[i].x!=-1)lines1.push_back(lines[i]);
//	lines = lines1;

}
//отрезает от треугольника tr по контурам lines3d. include_v0 определ€ет, €вл€етс€ ли перва€ вершина треугольника одной из вершин результата
void CutTriangle(Triangle& tr, Lines& lines3d,std::vector<LineConf>& lines_conf, bool include_v0,Geometry* res,bool back_faces)
{

	v3vec vert3d;
	v2vec vert;
	iv2vec lines;
	int coord[3];
	bool triangles_orientation=0;
	{
		float max_nrm = max(abs(tr.norm.x),max(abs(tr.norm.y),abs(tr.norm.z)));
		if(abs(tr.norm.x)==max_nrm)
		{
			coord[0]=1;
			coord[1]=2;
			coord[2]=0;
		}else 
		if(abs(tr.norm.y)==max_nrm)
		{
			coord[0]=2;
			coord[1]=0;
			coord[2]=1;
		}else
		{
			coord[0]=0;
			coord[1]=1;
			coord[2]=2;
		}
		triangles_orientation=(tr.norm.GetByID(coord[2])>0);
		triangles_orientation=xor(triangles_orientation,back_faces);
	}
	vec2 tr2[3];
	for(int i=0;i<3;i++)
		tr2[i].set(tr.v[i].GetByID(coord[0]),tr.v[i].GetByID(coord[1]));

	ivec side_points[3];//точки на i-ом ребре треугольника
	vec sp_vals[3];//степень удалЄнности точки от первой вершины ребра
	
	for(int i=0;i<lines3d.size();i++)
	{
		vec2 a(lines3d[i].v[0].GetByID(coord[0]),lines3d[i].v[0].GetByID(coord[1]));
		vec2 b(lines3d[i].v[1].GetByID(coord[0]),lines3d[i].v[1].GetByID(coord[1]));
		ivec2 ll(find_vec2(a,vert),find_vec2(b,vert));
		if(ll.x==-1)
		{
			if(lines_conf[i].v[1])
			{
				int s_id=lines_conf[i].v[0];
				side_points[s_id].push_back(vert.size());
				sp_vals[s_id].push_back(a.lengthSQR(tr2[s_id]));
			}
			vert.push_back(a);
			vert3d.push_back(lines3d[i].v[0]);
			ll.x=vert.size()-1;
		}
		if(ll.y==-1)
		{
			if(lines_conf[i].v[3])
			{
				int s_id=lines_conf[i].v[2];
				side_points[s_id].push_back(vert.size());
				sp_vals[s_id].push_back(b.lengthSQR(tr2[s_id]));
			}
			vert.push_back(b);
			vert3d.push_back(lines3d[i].v[1]);
			ll.y=vert.size()-1;
		}
		
		lines.push_back(ll);
	}
//	OptimizeLines(vert,lines,vert3d);

	
	
	for(int i=0;i<3;i++)
	{
			//сортировка точек по удалЄнности от вершины ребра треугольника
		int ii,jj,nn=side_points[i].size();
		for(ii=0;ii<nn-1;ii++)
		for(jj=ii+1;jj<nn;jj++)
		{
			if(sp_vals[i][ii]>sp_vals[i][jj])
			{
				
				swap(sp_vals[i][ii],sp_vals[i][jj]);
				swap(side_points[i][ii],side_points[i][jj]);
				
			}
		}	
	}
	
	bool incl[3];//включать ли вершины треугольника в результат
	int v_id[3]={-1,-1,-1};
	incl[0] = include_v0;
	incl[1] = ((include_v0?1:0) + side_points[0].size())%2;
	incl[2] = ((incl[1]?1:0) + side_points[1].size())%2;
	if( !!(((incl[2]?1:0) + side_points[2].size())%2) !=include_v0)
	{
		printf("\nWrong sides config!\n");
		was_error=1;
	}



	
		


	for(int i=0;i<3;i++)
		if(incl[i])
		{
			vert.push_back(tr2[i]);
			vert3d.push_back(tr.v[i]);
			v_id[i]=vert.size()-1;
		}



	//св€зывание сформированных и отсортированных точек на рЄбрах треугольника в отрезки
	for(int i=0;i<3;i++)
	{
		if(incl[i])
		{
			if(side_points[i].size())
				lines.push_back(ivec2(v_id[i],side_points[i][0]));
			else
				if(incl[(i+1)%3])lines.push_back(ivec2(v_id[i],v_id[(i+1)%3]));
		}
	
		if(side_points[i].size()>=2+incl[i])
		for(int j=incl[i];j<side_points[i].size()-1;j+=2)
		{
			lines.push_back(ivec2(side_points[i][j],side_points[i][j+1]));
		}
		if(side_points[i].size())
		if(incl[(i+1)%3])
		{
			lines.push_back(ivec2(v_id[(i+1)%3],side_points[i][side_points[i].size()-1]));
			
		}
	}
	


	//замкнутые контуры сформированы. ¬ы€сн€ем, какую они дадут триангул€цию
	{
		iv3vec tr_ids;
		MeshLines(vert,lines,tr_ids,triangles_orientation?1:-1);

//		printf("_");

		vec3 vv[3];
		for(int i=0;i<tr_ids.size();i++)
		{
			ivec3 tt_id = tr_ids[i];
			
			res->AddTriangle(vert3d[tr_ids[i].x], vert3d[tr_ids[i].y], vert3d[tr_ids[i].z]);
		}
	}
//		printf("c");

	if(0)
	if(was_error)
	{
		printf("\nlv:%d %d;",lines3d.size(),vert.size());
		printf("on sides:%d %d %d; inc:%d%d%d; ",side_points[0].size(),side_points[1].size(),side_points[2].size(),incl[0],incl[1],incl[2]);

		was_error=0;
		printf("[c%d]\n",coord[2]);
		for(int i=0;i<lines.size();i++)
			printf("(%d,%d)",lines[i].x,lines[i].y);
		for(int i=0;i<lines3d.size();i++)
		{

			printf("\n");
			for(int j=0;j<2;j++)
			{
				printf("\n");
				for(int k=0;k<3;k++)
					printf("	%g	",lines3d[i].v[j].GetByID(k));
			}
		}
			printf("\n\ntriangle:\n");
			for(int j=0;j<3;j++)
			{
				printf("\n");
				for(int k=0;k<3;k++)
					printf("	%g	",tr.v[j].GetByID(k));
			}
					printf("\n\n\n");

	}

}