
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
	//vec2 livetime[$VD_NUMBER];
	bool inter[$VD_NUMBER];

	vec3 color = vec3(0.0),norm;
	vec4 step  = vec4(ray*step_length,step_length);
	float ddd=clamp(step_length/0.05,0.0,1.0);
	float op = 1.0,s,f;
	float e[$VD_NUMBER],e0[$VD_NUMBER];

	for(id=0;id<$VD_NUMBER;id++)
	{
		e[id] = 0.0;
		inter[id] = IntersectBox (ToTextureSpace(ps.xyz,id), ToTextureSpace1(ray,id),vec3(0.0),vec3(1.0), s,f );
		if(inter[id])
		{
			start = min(start,s);
			final = max(final,f);
		}
	}
	if(id!=0 && IntersectBox (ps.xyz,ray,box1,box2,s,f ))
	{
		start = max(start,s);
		final = min(final,f);
		float start0 = start;
		float final0 = final;
		
		f=0.0;
		vec4 ps0 = ps;
#if USE_BOUNDING_MESH!=0
		vec4 front_face_dist = texture2D(front_dist_txt,text_coord);
		vec4 back_face_dist = texture2D(back_dist_txt,text_coord);
		if(front_face_dist.x>back_face_dist.x)front_face_dist = vec4(0.0,front_face_dist.xyz);

		for(int otr=0;otr<4;otr++)
		{
//			vec3 nrm = CalcNormByDist(ps0.xyz,ray,otr);

			start = max(f,front_face_dist[otr]);
			final = back_face_dist[otr];
			f=final;
			
			if(final<=start)break;
			start = max(start,start0);
			final = min(final,final0);
#endif
		
			ps = ps0 + step*(start/step_length);
		
			ps = ps + step*(1.0+rand(vec4(vertex,ps.w)));
		
		
			for(id=0;id<$VD_NUMBER;id++)
				e[id] = Equ(ps.xyz,id);
			ps+=step;
		
			while(ps.w <final)
			{
				int cur_iso=0;
			
				for(id=0;id<$VD_NUMBER;id++)
				{
					e0[id]=e[id];
					e[id] = Equ(ps.xyz,id);

					//if(e[id] == clamp(e[id],min_level[id],max_level[id]))
					{
						for(;cur_iso<=IsoLast[id];cur_iso++)
						{
							vec3 tmp = vec3(level[cur_iso],e0[id],e[id]);
							float opp;
							if(clamp(tmp.x,tmp.y,tmp.z) == tmp.x)
							{
								vec3 ps11 = ps.xyz + step.xyz*((tmp.x-tmp.z)/(tmp.z-tmp.y));
								norm = -normalize(GradEqu(ps11,id));
								
								opp = dot(ray,norm);
								opp = 1.0-opp*opp;
								opp = opp*opp*opp*opp;
								opp = mix(my_color[cur_iso].w,1.0,opp);
								color = mix(color , Phong(ps11,norm,my_color[cur_iso].xyz) , op * opp); 
								op *= 1.0 - opp;
								//break;
							}else
							if(clamp(tmp.x,tmp.z,tmp.y) == tmp.x)
							{
								vec3 ps11 = ps.xyz + step.xyz*((tmp.x-tmp.z)/(tmp.z-tmp.y));
								norm = normalize(GradEqu(ps11,id));
								opp = dot(ray,norm);
								opp = 1.0-opp*opp;
								opp = opp*opp*opp*opp;
								opp = mix(my_color[cur_iso].w,1.0,opp);
								color = mix(color , Phong(ps11,norm,my_color[cur_iso].xyz) , op * opp); 
								op *= 1.0 - opp;
							//	break;
							}
						}
					}
				}
				
				if(op<0.05)
				{
					op=0.0;
					break;
				}
				
				
				ps+=step;
			}
		
		
#if USE_BOUNDING_MESH!=0

			if(op==0.0)break;
		}
#endif		
	
			
		
		gl_FragDepth = max(GetDepth(ps.xyz),0.1);

		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,1.0-op);
		
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}
	
}


