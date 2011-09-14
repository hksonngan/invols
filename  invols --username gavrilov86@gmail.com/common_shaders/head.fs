varying vec3 vertex,normal;
varying vec2 text_coord;


uniform float step_length;
uniform vec3 box1,box2;
uniform vec4 random_seed;

uniform vec3 pos;
uniform vec3 nav;
uniform vec3 top;
uniform vec3 left;
uniform float screen_width;
uniform float screen_height;


uniform vec3 RFrom;
uniform vec3 GFrom;
uniform vec3 BFrom;


uniform float max_level[$VD_NUMBER],min_level[$VD_NUMBER];
uniform vec3 cell_size[$VD_NUMBER];
uniform vec3 cs_center[$VD_NUMBER];
uniform vec3 cs_x[$VD_NUMBER];
uniform vec3 cs_y[$VD_NUMBER];
uniform vec3 cs_z[$VD_NUMBER];
uniform sampler3D f_text[$VD_NUMBER];
uniform sampler3D f_text_i[$VD_NUMBER];
uniform sampler3D f_text_tf[$VD_NUMBER];
uniform float opacity[$VD_NUMBER];
uniform int IsoLast[$VD_NUMBER];
uniform int QuadLast[$VD_NUMBER];
uniform vec2 tf_ww[$VD_NUMBER];

uniform vec3 LightDir;

uniform sampler2D txt_level_color;
uniform sampler2D front_dist_txt;
uniform sampler2D back_dist_txt;

const float z_near=0.1;
const float z_far=30.0;

#if $IsoNum>0
uniform vec4 my_color[$IsoNum];
uniform float level[$IsoNum];
#endif

#if $QuadNum>0
uniform vec4 q_color[$QuadNum];
uniform vec4 q_level[$QuadNum];
#endif


vec4 GetLevelColor(float lv,int id) 
{
	return texture2D(txt_level_color, vec2(clamp((lv-tf_ww[id].x)/(tf_ww[id].y-tf_ww[id].x),1.0/512.0,511.0/512.0),(float(id)+0.5)/4.0));
	
}
//#if $use_cubic_filt!=0

float interpolate_cubic(sampler3D tex, vec3 coord,vec3 cell_size1)
{
	
	// transform the coordinate from [0,extent] to [-0.5, extent-0.5]
	vec3 coord_grid = coord/cell_size1-vec3(0.5);
	vec3 index = floor(coord_grid);
	vec3 fraction = coord_grid - index;
	vec3 one_frac = vec3(1.0) - fraction;
	vec3 one_frac2 = one_frac * one_frac;
	vec3 fraction2 = fraction * fraction;

	vec3 w0 = 1.0/6.0 * one_frac2 * one_frac;
	vec3 w1 = vec3(2.0/3.0) - 0.5 * fraction2 * (2.0-fraction);
	vec3 w2 = vec3(2.0/3.0) - 0.5 * one_frac2 * (2.0-one_frac);
	vec3 w3 = 1.0/6.0 * fraction2 * fraction;
	vec3 g0 = w0 + w1;
	vec3 g1 = w2 + w3;
	// h0 = w1/g0 - 1, move from [-0.5, extent-0.5] to [0, extent]
	vec3 h0 = (w1 / g0) - vec3(0.5) + index;
	vec3 h1 = (w3 / g1) + vec3(1.5) + index;
	h0*=cell_size1;
	h1*=cell_size1;

	// fetch the four linear interpolations
	
	float tex000 = texture3D(tex, h0).x;
	float tex100 = texture3D(tex, vec3(h1.x, h0.y,h0.z)).x;
	float tex010 = texture3D(tex, vec3(h0.x, h1.y,h0.z)).x;
	float tex110 = texture3D(tex, vec3(h1.x,h1.y,h0.z)).x;
	
	float tex001 = texture3D(tex, vec3(h0.x,h0.y,h1.z)).x;
	float tex101 = texture3D(tex, vec3(h1.x,h0.y,h1.z)).x;
	float tex011 = texture3D(tex, vec3(h0.x,h1.y,h1.z)).x;
	float tex111 = texture3D(tex, h1).x;
	// weigh along the z-direction
	tex000 = mix(tex001, tex000, g0.z);
	tex100 = mix(tex101, tex100, g0.z);
	tex010 = mix(tex011, tex010, g0.z);
	tex110 = mix(tex111, tex110, g0.z);

	// weigh along the y-direction
	tex000 = mix(tex010, tex000, g0.y);
	tex100 = mix(tex110, tex100, g0.y);
	// weigh along the x-direction
	return mix(tex100, tex000, g0.x);
}
//#endif
vec3 ToTextureSpace(vec3 ps,int id)
{
	mat3 mm = mat3(cs_x[id],cs_y[id],cs_z[id]);
	return mm*(ps-cs_center[id]);//(mm*vec4(ps,1.0)-gpu_box1[id])/(gpu_box2[id]-gpu_box1[id]);
}
vec3 ToTextureSpace1(vec3 ps,int id)
{
	mat3 mm = mat3(cs_x[id],cs_y[id],cs_z[id]);
	return mm*ps;
}

float FastEqu(vec3 arg,int id)
{
	vec3 coord=ToTextureSpace(arg,id);
	return texture3D(f_text[id], coord).x;
}
float Equ(vec3 arg,int id)
{
	vec3 coord=ToTextureSpace(arg,id);
	if(coord==clamp(coord,vec3(0.01),vec3(0.99)))
	{
		#if $use_cubic_filt==0
			return texture3D(f_text[id], coord).x;
		#else
			return interpolate_cubic(f_text[id],coord,cell_size[id]);
		#endif
	}
	else
		return 0.0;
}
float EquTF(vec3 arg,int id)
{
	vec3 coord=ToTextureSpace(arg,id);
	if(coord==clamp(coord,vec3(0.0),vec3(1.0)))
	{
		#if $use_cubic_filt==0
			return texture3D(f_text_tf[id], coord).x;
		#else
			return interpolate_cubic(f_text_tf[id],coord,vec3(1.0/256.0));
		#endif
	}
	else
		return 0.0;
}
/*
vec2 Equ2(vec3 arg,int id)
{
	vec3 coord=ToTextureSpace(arg,id);
	if(coord==clamp(coord,vec3(0.0),vec3(1.0)))
	{
		#if $use_cubic_filt==0
			return vec2(texture3D(f_text[id], coord).x,texture3D(f_text_tf[id], coord).x);
		#else
			return vec2(interpolate_cubic(f_text[id],coord,cell_size[id]),interpolate_cubic(f_text_tf[id],coord,vec3(1.0/256.0)));
		#endif
	}
	else
		return 0.0;
}
*/
float EquI(vec3 arg,int id)
{
	vec3 coord=ToTextureSpace(arg,id);
	if(coord==clamp(coord,vec3(0.0),vec3(1.0)))
		return texture3D(f_text_i[id], coord).y;
	else
		return 0.0;
}
/*
float EquF(sampler3D tex, vec3 arg)
{
vec3 coord=ToTextureSpace(arg);
	if(coord==clamp(coord,vec3(0.0),vec3(1.0)))
#if $use_cubic_filt==0
	return texture3D(tex, ToTextureSpace(arg)).x;
#else
	return interpolate_cubic(tex, ToTextureSpace(arg));
#endif
	else
		return 0.0;
}*/

vec3 GradEqu(in vec3 arg,int id)
{
	float delta = cell_size[id].x;
	vec3 res;
	res.x = Equ(vec3(arg.x+delta,arg.y,arg.z),id)-Equ(vec3(arg.x-delta,arg.y,arg.z),id);
	res.y = Equ(vec3(arg.x,arg.y+delta,arg.z),id)-Equ(vec3(arg.x,arg.y-delta,arg.z),id);
	res.z = Equ(vec3(arg.x,arg.y,arg.z+delta),id)-Equ(vec3(arg.x,arg.y,arg.z-delta),id);
	return res;

}


vec3 GradEqu1(float ee,in vec3 arg, int id)
{
	float delta = cell_size[id].x;
	vec3 res;
	res.x = FastEqu(vec3(arg.x+delta,arg.y,arg.z),id)-ee;
	res.y = FastEqu(vec3(arg.x,arg.y+delta,arg.z),id)-ee;
	res.z = FastEqu(vec3(arg.x,arg.y,arg.z+delta),id)-ee;
	return res;

}

#define K_A 0.1
#define K_D 0.9
#define K_S 0.3
#define P 32.0

vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
	float diffuse = dot ( LightDir, normal )*0.6+0.4;
	vec3 reflect = reflect ( -LightDir, normal );
	float specular = pow ( max (0.0, dot ( reflect, normalize(pos-point) ) ), P );
	return K_A * vec3(1.0) + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}



float GetDepth(vec3 ps)
{
	ps -= pos;
#if $IsPerspective==1
	return clamp(z_far/(z_far-z_near) - (z_far*z_near/(z_far-z_near))/(dot(ps,nav)),0.01,0.99);
#else
	return clamp((dot(ps,nav)-z_near)/(z_far-z_near),0.01,0.99);
#endif
}


bool IntersectBox ( vec3 Origin,vec3 Direction, vec3 minimum, vec3 maximum, out float start, out float final )
{
	vec3 OMAX = ( minimum - Origin ) / Direction;
	vec3 OMIN = ( maximum - Origin ) / Direction;
	vec3 MAX = max ( OMAX, OMIN );
	vec3 MIN = min ( OMAX, OMIN );
	final = min ( MAX.x, min ( MAX.y, MAX.z ) );
	start = max ( max ( MIN.x, 0.0), max ( MIN.y, MIN.z ) );	
	return final > start;
}

#define ACC_STRUCT_SIZE 32.0


float rand(vec4 co)
{
	return fract(sin(dot(co,random_seed)) * 1000.0);
}
