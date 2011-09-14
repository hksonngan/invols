varying vec3 vertex;
uniform sampler3D f_text;

uniform vec3 cell_size0,cell_size1;


#define VD_NUMBER 1


uniform float max_level[VD_NUMBER], min_level[VD_NUMBER];
uniform vec3 box1,box2;
uniform vec3 cs_center[VD_NUMBER];
uniform vec3 cs_x[VD_NUMBER];
uniform vec3 cs_y[VD_NUMBER];
uniform vec3 cs_z[VD_NUMBER];

///////////////functions
vec3 ToTextureSpace(vec3 ps,int id)
{
	mat3 mm = mat3(cs_x[id],cs_y[id],cs_z[id]);
	return mm*(ps-cs_center[id]);
}
float Equ(vec3 arg,int id)
{
	vec3 coord=ToTextureSpace(arg,id);
	if(coord==clamp(coord,vec3(0.0),vec3(1.0)))
	{
		return texture3D(f_text, coord).x;
	}
	else
		return 0.0;
}



#define kernel_size 2.0


void main()
{
	float res = 0.0;
	vec3 i_cell_size = vec3(16.0);

	//e0[id] = Equ(ps.xyz,id);
	
	vec3 pt = box1+vertex*(box2-box1) - cell_size1*0.5;
	vec3 idd;
	for(idd.x=0.0;idd.x<=i_cell_size.x;idd.x++)
	for(idd.y=0.0;idd.y<=i_cell_size.y;idd.y++)
	for(idd.z=0.0;idd.z<=i_cell_size.z;idd.z++)
	{
		vec3 stp = cell_size1*idd/i_cell_size;
		res = max(res, Equ(pt+stp,0));
	}

	
	
	res = clamp(((res - min_level[0]) / (max_level[0] - min_level[0])) , 0.0,1.0);
	
	//res = 1.0;//(Equ(pt,0) > min_level[0]) ? 1.0 : 0.0;
	//if(vertex.x==1.0/16.0)res=1.0;

	gl_FragColor = vec4(res);
}