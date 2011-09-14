
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

const float delta=1.0/512.0;
vec3 GradEqu(in vec3 arg)
{
		vec3 res;
		res.x = Equ(vec3(arg.x+delta,arg.y,arg.z))-Equ(vec3(arg.x-delta,arg.y,arg.z));
		res.y = Equ(vec3(arg.x,arg.y+delta,arg.z))-Equ(vec3(arg.x,arg.y-delta,arg.z));
		res.z = Equ(vec3(arg.x,arg.y,arg.z+delta))-Equ(vec3(arg.x,arg.y,arg.z-delta));
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
	return vec3(l * c_contr + c_br);
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
		vec3 norm;
		vec3 step  = ray*step_length;
		vec3 stepi  = ray*0.01;
		float ddd=clamp(step_length/0.05,0.0,1.0);
		float op = 1.0;
		ps += ray*start;
		float e = Equ(ps),e0;
	
		ps+=step;
	
	


		if(e >= level1)
		{
			color = GetLevelColorBC(e);
			op = 0.0;
			gl_FragDepth = GetDepth(ps);
		}else
		{
	#if $use_accel_struct==1
				do
				{
					ps+=stepi;
				}while((ps == clamp(ps,box1,box2)) && (texture3D(f_text_i, ps/scale).x<level1));
				
				ps-=stepi;
	#endif
				gl_FragDepth=1.0;
				while(true)
				{
					
						e0=e;
						e = Equ(ps);
	#if $use_accel_struct==1
						if(e >= level1)
	#endif
						{
							/*
						vec4 cl;
						
						cl = GetLevelColor(e);

						//color = mix(color , cl.xyz ,op * cl.w);

						norm = -GradEqu(ps);
						
						float diffuse = clamp(max(dot ( LightDir, norm )*80.0,0.5),0.0,1.0);
						cl.w*=diffuse*ddd;
						color = mix(color , cl.xyz*diffuse , op * cl.w); 

						op *= (1.0-cl.w);
						*/

	#if $IsoNum>0
						for(int i=0;i<$IsoNum;i++)
						{
						vec3 tmp = vec3(level[i],e0,e);
						if(clamp(tmp.x,tmp.y,tmp.z) == tmp.x)
						{
							vec3 ps11 = ps + step*((tmp.x-tmp.z)/(tmp.z-tmp.y));
							norm = -normalize(GradEqu(ps11));
							color = mix(color , Phong(ps11,norm,my_color[i].xyz) , op * my_color[i].w); 
							op *= 1.0-my_color[i].w;
							break;
						}else
						if(clamp(tmp.x,tmp.z,tmp.y) == tmp.x)
						{
							vec3 ps11 = ps + step*((tmp.x-tmp.z)/(tmp.z-tmp.y));
							norm = normalize(GradEqu(ps11));
							color = mix(color , Phong(ps11,norm,my_color[i].xyz) , op * my_color[i].w); 
							op *= 1.0-my_color[i].w;
							break;
						}
						}
	#endif
						}
						
						ps+=step;
					

					
					if(ps != clamp(ps,box1,box2) || op<0.02)
					{
						gl_FragDepth = max(GetDepth(ps),0.1);
						
						break;
						
					}
						


				}
				
		
		}
		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,1.0-op);
		
	}	else
	{
		gl_FragColor = vec4(1.0,0.0,0.0,1.0);
		gl_FragDepth=1.0;
	}
	
}
