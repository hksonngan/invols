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

uniform sampler3D f_text1;
uniform sampler3D f_text2;


uniform sampler3D f_text_i;
uniform sampler3D f_text_tf;
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
/*
vec3 ToTextureSpace(vec3 ps,int id)
{return ps;}
vec3 ToTextureSpace1(vec3 ps,int id)
{return ps;}
*/

vec3 ToTextureSpace(vec3 ps,int id)
{

	mat3 mm = mat3(cs_x[id],cs_y[id],cs_z[id]);
	return mm*(ps-cs_center[id]);//(mm*vec4(ps,1.0)-gpu_box1[id])/(gpu_box2[id]-gpu_box1[id]);
}
vec3 ToTextureSpace_1(vec3 ps,int id)
{
	mat3 mm = mat3(cs_x[id],cs_y[id],cs_z[id]);
	
	return ps*mm+cs_center[id];
}
vec3 ToTextureSpace1(vec3 ps,int id)
{
	mat3 mm = mat3(cs_x[id],cs_y[id],cs_z[id]);
	return mm*ps;
}

float FastEqu(vec3 arg,int id,sampler3D tex)
{
	vec3 coord=ToTextureSpace(arg,id);
	return texture3D(tex, coord).x;
}
float Equ(vec3 arg,int id,sampler3D tex)
{
//return sin(length(arg)/0.2);

	vec3 coord=ToTextureSpace(arg,id);
	if(coord==clamp(coord,vec3(0.01),vec3(0.99)))
	{
		#if $use_cubic_filt==0
			return texture3D(tex, coord).x;
		#else
			return interpolate_cubic(tex,coord,cell_size[id]);
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
			return texture3D(f_text_tf, coord).x;
		#else
			return interpolate_cubic(f_text_tf,coord,vec3(1.0/256.0));
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
			return vec2(texture3D(f_text, coord).x,texture3D(f_text_tf[id], coord).x);
		#else
			return vec2(interpolate_cubic(f_text,coord,cell_size[id]),interpolate_cubic(f_text_tf[id],coord,vec3(1.0/256.0)));
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
		return texture3D(f_text_i, coord).y;
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

vec3 GradEqu(in vec3 arg,int id,sampler3D tex)
{
	float delta = cell_size[id].x;
	vec3 res;
	res.x = Equ(vec3(arg.x+delta,arg.y,arg.z),id,tex)-Equ(vec3(arg.x-delta,arg.y,arg.z),id,tex);
	res.y = Equ(vec3(arg.x,arg.y+delta,arg.z),id,tex)-Equ(vec3(arg.x,arg.y-delta,arg.z),id,tex);
	res.z = Equ(vec3(arg.x,arg.y,arg.z+delta),id,tex)-Equ(vec3(arg.x,arg.y,arg.z-delta),id,tex);
	return res;

}


vec3 GradEqu1(float ee,in vec3 arg, int id,sampler3D tex)
{
	float delta = cell_size[id].x;
	vec3 res;
	res.x = FastEqu(vec3(arg.x+delta,arg.y,arg.z),id,tex)-ee;
	res.y = FastEqu(vec3(arg.x,arg.y+delta,arg.z),id,tex)-ee;
	res.z = FastEqu(vec3(arg.x,arg.y,arg.z+delta),id,tex)-ee;
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



float GetShadness1(vec3 pt,vec3 step)
{
	float ddd=step_length*300.0;
	
	float e,alpha=0.0,ww;
	
	for ( float itt = 0.0; itt < float ( 10.0 ); itt+=1.0 )
	{
		pt += step;
		if(!(clamp(pt,box1,box2)==pt))break;
		step*=1.3;
		e = Equ(pt,0,f_text1);
		ww = GetLevelColor(e,0).w*ddd;
		alpha = alpha + (1.0-alpha) * ww;
	}
	return alpha;

}
void main()
{
#if $IsPerspective==1
	vec3 ray = normalize(vertex-pos);
	vec4 ps=vec4(pos,0.0);
#else
	vec3 ray = nav;
	vec4 ps = vec4(vertex,0.0);
#endif
	float start=1000.0,final=0.0,s,f;
	int id;
	//vec2 livetime[$VD_NUMBER];

	vec3 color = vec3(0.0),norm;
	vec4 step  = vec4(ray*step_length,step_length),cl;
	float ddd=step_length*1500.0;
	float alpha = 0.0;
	float e0[$VD_NUMBER];

	{
		
		
		if(IntersectBox (ToTextureSpace(ps.xyz,0), ToTextureSpace1(ray,0),vec3(0.0),vec3(1.0), s,f ))
		{
			start = min(start,s);
			final = max(final,f);
		}
		#if $VD_NUMBER>1
		if(IntersectBox (ToTextureSpace(ps.xyz,1), ToTextureSpace1(ray,1),vec3(0.0),vec3(1.0), s,f ))
		{
			start = min(start,s);
			final = max(final,f);
		}
		#endif
	}
	
	id=0;
	if(IntersectBox (ps.xyz,ray,box1,box2,s,f ))
	{
		start = max(start,s);
		final = min(final,f);
		ps = ps + step*(start/step_length);
		
		ps = ps + step*(1.0+rand(vec4(vertex,ps.w)));

		vec3 step1;
		step1.x=(rand(vec4(vertex,ps.x))-0.5)*step_length;
		step1.y=(rand(vec4(ps.z,vertex.xz,ps.x))-0.5)*step_length;
		step1.z=(rand(vec4(ps.y,vertex))-0.5)*step_length;
		
		step1+=LightDir*step_length*1.1;
		
			
			for ( float itt = 0.0; itt < float ( 512.0 ); itt+=1.0 )
			{
				if(ps.w >=final)break;
				//alpha+=0.1;
			
				//for(id=0;id<$VD_NUMBER;id++)
				//if(inter[id])

				
					e0[id] = Equ(ps.xyz,id,f_text1);
					//if(e0[id] == clamp(e0[id],tf_ww[id].x,tf_ww.y)
					
						cl =  GetLevelColor(e0[id],id);

						cl.w *= ddd*opacity[id];
						#if $shade_mode==1
							norm = normalize(GradEqu1(e0[id],ps.xyz,id,f_text1)+vec3(0.000001));
							//cl.xyz = mix(Phong(ps.xyz,-norm,cl.xyz),Phong(ps.xyz,nrm,cl.xyz),max(start-ps.w+0.02,0.0)/0.02);
							if(dot(norm,ray)<0)norm=-norm;
							cl.xyz = Phong(ps.xyz,-norm,cl.xyz);
						#endif
						float d_alpha = (1.0-alpha) * cl.w;
						//if(alpha==0.0 && d_alpha!=0.0)gl_FragDepth = max(GetDepth(ps.xyz),0.1);
						//mid_t += d_alpha*ps.w;
						if(d_alpha>ddd*0.005)
							cl.xyz = cl.xyz*(1.0-GetShadness1(ps.xyz,step1));
						color = color + d_alpha * cl.xyz;
						
						alpha = alpha + d_alpha;
					
					
						#if $VD_NUMBER>1
				id=1;
					e0[id] = Equ(ps.xyz,id,f_text2);
					//if(e0[id] == clamp(e0[id],tf_ww[id].x,tf_ww.y)
					
						cl =  GetLevelColor(e0[id],id);

						cl.w *= ddd*opacity[id];
						#if $shade_mode==1
							norm = normalize(GradEqu1(e0[id],ps.xyz,id,f_text2)+vec3(0.000001));
							//cl.xyz = mix(Phong(ps.xyz,-norm,cl.xyz),Phong(ps.xyz,nrm,cl.xyz),max(start-ps.w+0.02,0.0)/0.02);
							if(dot(norm,ray)<0)norm=-norm;
							cl.xyz = Phong(ps.xyz,-norm,cl.xyz);
						#endif
						d_alpha = (1.0-alpha) * cl.w;
						//if(alpha==0.0 && d_alpha!=0.0)gl_FragDepth = max(GetDepth(ps.xyz),0.1);
						//mid_t += d_alpha*ps.w;
						color = color + d_alpha * cl.xyz;
						
						alpha = alpha + d_alpha;

				id=0;
				#endif
				
						if(alpha>0.95)
						{
							alpha=1.0;
							break;
						}

				ps += step;
			}
			
		//color = color*(1.0-GetShadness(ps.xyz));

		//		color.x = texture2D(front_dist_txt,text_coord).x;
	//	color.y = texture2D(back_dist_txt,text_coord).x;
//		gl_FragDepth = max(GetDepth(ps0.xyz + min(ps.w,step.xyz*(mid_t/step_length))),0.1);
		gl_FragDepth = max(GetDepth(ps.xyz),0.1);

		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,alpha);
		
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}
	
}
