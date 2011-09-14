
varying vec3 vertex,normal;

uniform float level1,step_length;
uniform vec3 box1,box2;

uniform vec3 pos;
uniform vec3 nav;
uniform vec3 scale;

uniform vec3 RFrom;
uniform vec3 GFrom;
uniform vec3 BFrom;


uniform vec3 LightDir;
uniform sampler3D f_text;
uniform sampler3D f_text_i;
//uniform sampler3D f_text_n;

uniform float flag;
uniform vec3 level_color1;

uniform float c_br,c_contr;

uniform sampler2D txt_level_color;

#if $IsoNum>0
uniform vec4 my_color[$IsoNum];
uniform float level[$IsoNum];
#endif

const float z_near=0.3;
const float z_far=60.0;



vec4 GetLevelColor(float lv) 
{
	return texture2D(txt_level_color, vec2(lv,0.5));
}

float Equ(in vec3 arg)
{
	return texture3D(f_text, arg/scale).x;//*17.0;
}


float GetDepth(vec3 ps)
{
	ps -= pos;
	return z_far/(z_far-z_near) - (z_far*z_near/(z_far-z_near))/(dot(ps,nav));
}

const float delta=1.0/256.0;
vec3 GradEqu(in vec3 arg)
{
		vec3 res;
		res.x = Equ(vec3(arg.x+delta,arg.y,arg.z))-Equ(vec3(arg.x-delta,arg.y,arg.z));
		res.y = Equ(vec3(arg.x,arg.y+delta,arg.z))-Equ(vec3(arg.x,arg.y-delta,arg.z));
		res.z = Equ(vec3(arg.x,arg.y,arg.z+delta))-Equ(vec3(arg.x,arg.y,arg.z-delta));
		return res;

//		return normalize(texture3D(f_text_n, arg).xyz);
}
vec3 GradEqu1(in vec3 arg)
{
		vec3 res;
		float ee = Equ(vec3(arg.x,arg.y,arg.z));
		res.x = Equ(vec3(arg.x+delta,arg.y,arg.z))-ee;
		res.y = Equ(vec3(arg.x,arg.y+delta,arg.z))-ee;
		res.z = Equ(vec3(arg.x,arg.y,arg.z+delta))-ee;
		return res;

//		return normalize(texture3D(f_text_n, arg).xyz);
}

#define K_A 0.1
#define K_D 0.9
#define K_S 0.1
#define P 23.0

vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
	float diffuse = dot ( LightDir, normal )*0.5+0.5;
	vec3 reflect = reflect ( -LightDir, normal );
	float specular = pow ( max (0.0, dot ( reflect, LightDir ) ), P );
	return K_A * vec3(1.0) + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}


vec3 GetLevelColorBC(float l)
{
	return vec3(l * 0.6 + 0.05);
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

//const float ICellSize = 16.0/256.0;

void main()
{
	vec3 ray = normalize(vertex-pos);
	vec3 ps=pos;
	float start,final;
	
	if( IntersectBox (ps,ray,box1,box2, start, final ))
	{
		vec3 color = vec3(0.0);
		vec3 step  = ray*step_length;
		float op = 1.0;
		ps += ray*start;
		float e = Equ(ps),e0;
	
		ps+=step;
	
	
		float max_i=0.0;

		gl_FragDepth=1.0;
		while(true)
		{
					
			e0=e;
			e = Equ(ps);
	
						
//			vec4 cl = GetLevelColor(e);
				
			max_i=max(max_i,e);
			ps+=step;
			if(ps != clamp(ps,box1,box2))
			{
				break;
			}
		}
		gl_FragDepth = max(GetDepth(ps),0.1);
		op=0.0;
		color = vec3(max_i);
		
		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,1.0-op);
		
	}	else
	{
		gl_FragColor = vec4(1.0,0.0,0.0,1.0);
		gl_FragDepth=1.0;
	}
	
}
