
void main()
{
#if $IsPerspective==1
	vec3 ray = normalize(vertex-pos);
	vec4 ps=vec4(pos,0.0);
#else
	vec3 ray = nav;
	vec4 ps = vec4(vertex,0.0);
#endif
	float start=1000.0,final=0.0;
	int id;

	vec3 color = vec3(0.0),norm;
	vec4 step  = vec4(ray*step_length,step_length);
	float ddd=clamp(step_length/0.05,0.0,1.0);
	float op = 1.0,s,f;
	float e[$VD_NUMBER],e0[$VD_NUMBER];

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
	
	if(IntersectBox (ps.xyz,ray,box1,box2,s,f ))
	{
		start = max(start,s);
		final = min(final,f);

		
		ps = ps + step*(start/step_length);
		
		ps = ps + step*(1.0+rand(vec4(vertex,ps.w)));
		
		id=0;
		e[0] = Equ(ps.xyz,0,f_text1);
		#if $VD_NUMBER>1
		e[1] = Equ(ps.xyz,1,f_text2);
		#endif
		ps+=step;
		
		int cur_iso=0,iso1=0,field1=0;
		for ( float itt = 0.0; itt < float ( 1024.0 ); itt+=1.0 )
		{
				if(ps.w >=final)break;
				
			
				cur_iso=0;
				{
					e0[id]=e[id];
					e[id] = Equ(ps.xyz,id,f_text1);
					//e[id] = Equ(ps.xyz,id);

					//if(e[id] == clamp(e[id],min_level[id],max_level[id]))
					{
						for(;cur_iso<=IsoLast[id];cur_iso++)
						{
							vec3 tmp = vec3(level[cur_iso],e0[id],e[id]);
							
							if((clamp(tmp.x,tmp.y,tmp.z) == tmp.x) || (clamp(tmp.x,tmp.z,tmp.y) == tmp.x))
							{
								op=0.0;
								field1=0;
								iso1=cur_iso;
							}
						}
					}
				}
				#if $VD_NUMBER>1
				id=1;
				{
					e0[id]=e[id];
					e[id] = Equ(ps.xyz,id,f_text2);
					//e[id] = Equ(ps.xyz,id);

					//if(e[id] == clamp(e[id],min_level[id],max_level[id]))
					{
						for(;cur_iso<=IsoLast[id];cur_iso++)
						{
							vec3 tmp = vec3(level[cur_iso],e0[id],e[id]);
							
							if((clamp(tmp.x,tmp.y,tmp.z) == tmp.x) || (clamp(tmp.x,tmp.z,tmp.y) == tmp.x))
							{
								op=0.0;
								field1=1;
								iso1=cur_iso;
							}
						}
					}
				}id=0;
				#endif
				
				if(op==0.0)				{					break;				}
				
				
				ps+=step;
			}
			if(op<0.05)
			{
			if(field1==0)
			{
				vec3 tmp = vec3(level[iso1],e0[field1],e[field1]);
				if(clamp(tmp.x,tmp.y,tmp.z) == tmp.x)
							{
								vec3 ps11 = ps.xyz + step.xyz*((tmp.x-tmp.z)/(tmp.z-tmp.y));
								norm = -normalize(GradEqu(ps11,field1,f_text1));
								color = Phong(ps11,norm,my_color[iso1].xyz); 
							}else
							if(clamp(tmp.x,tmp.z,tmp.y) == tmp.x)
							{
								vec3 ps11 = ps.xyz + step.xyz*((tmp.x-tmp.z)/(tmp.z-tmp.y));
								norm = normalize(GradEqu(ps11,field1,f_text1));
								color = Phong(ps11,norm,my_color[iso1].xyz) ; 
							}
			}
			if(field1==1)
			{
				vec3 tmp = vec3(level[iso1],e0[field1],e[field1]);
				if(clamp(tmp.x,tmp.y,tmp.z) == tmp.x)
							{
								vec3 ps11 = ps.xyz + step.xyz*((tmp.x-tmp.z)/(tmp.z-tmp.y));
								norm = -normalize(GradEqu(ps11,field1,f_text2));
								color = Phong(ps11,norm,my_color[iso1].xyz); 
							}else
							if(clamp(tmp.x,tmp.z,tmp.y) == tmp.x)
							{
								vec3 ps11 = ps.xyz + step.xyz*((tmp.x-tmp.z)/(tmp.z-tmp.y));
								norm = normalize(GradEqu(ps11,field1,f_text2));
								color = Phong(ps11,norm,my_color[iso1].xyz) ; 
							}
			}
			}
		
		gl_FragDepth = max(GetDepth(ps.xyz),0.1);

		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,1.0-op);
		
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}
	
}


