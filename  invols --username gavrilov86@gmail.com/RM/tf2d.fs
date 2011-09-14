
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
	float ddd=step_length*1500.0;
	float alpha = 0.0,s,f;
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
		float mid_t=0.0;
		f=0.0;
		vec4 ps0 = ps;
//		gl_FragDepth = 1.0;
		
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
			while(ps.w <final)
			{
				int cur_quad=0;
			
				for(id=0;id<$VD_NUMBER;id++)
				{
					e[id] = Equ(ps.xyz,id);
					for(;cur_quad<=QuadLast[id];cur_quad++)
					{
						#if $tf_type==0
						if(e[id] >= q_level[cur_quad].x)
						{

							norm = GradEqu1(e[id],ps.xyz,id);
							
							float gm = length(norm);
							
							if(gm >= q_level[cur_quad].z)
							{
								vec4 cl = q_color[cur_quad] * min(1.0,(e[id] - q_level[cur_quad].x)/(q_level[cur_quad].y - q_level[cur_quad].x));
//								cl = mix(cl,vec4(1.0,1.0,1.0,cl.w), min(1.0,(gm - q_level[cur_quad].z)/(q_level[cur_quad].w - q_level[cur_quad].z)));
								cl *=  min(1.0,(gm - q_level[cur_quad].z)/(q_level[cur_quad].w - q_level[cur_quad].z));

								cl.w *= ddd*opacity[id];
								#if $shade_mode==1
								if(dot(norm,ray)<0)norm=-norm;
								cl.xyz = Phong(ps.xyz,-normalize(norm),cl.xyz);
								#endif

								color = color + (1.0-alpha) * cl.w * cl.xyz;
								
								alpha = alpha + (1.0-alpha)*cl.w;
							}
						}
						#endif

						#if $tf_type==1
						if(e[id] == clamp(e[id],q_level[cur_quad].x,q_level[cur_quad].y))
						{

							norm = GradEqu1(e[id],ps.xyz,id);
							
							float gm = length(norm);
							
							if(gm == clamp(gm,q_level[cur_quad].z,q_level[cur_quad].w))
							{
								vec4 cl;
								cl =  GetLevelColor(e[id],id);
								cl.w *= ddd*opacity[id];
								#if $shade_mode==1
								if(dot(norm,ray)<0)norm=-norm;
								cl.xyz = Phong(ps.xyz,-normalize(norm),cl.xyz);
								#endif

								color = color + (1.0-alpha) * cl.w * cl.xyz;
								
								alpha = alpha + (1.0-alpha)*cl.w;
							}
						}
						#endif
						
					}
					
						
				}
				
				if(alpha>255.0/256.0)
				{
					alpha=1.0;
					break;
				}
				
				
				ps+=step;
			}
				
#if USE_BOUNDING_MESH!=0

			if(alpha==1.0)break;
		}
#endif		
	
			
		
		gl_FragDepth = max(GetDepth(ps.xyz),0.1);

		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,alpha);
		
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}
	
}


