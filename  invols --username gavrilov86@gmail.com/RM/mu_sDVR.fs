

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
	vec4 step  = vec4(ray*step_length,step_length),cl;
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
			id=1;
			sampler3D tex[2];
			tex[0] = f_text1;
			tex[1] = f_text2;
			while(ps.w <final)
			{
			
				for(id=0;id<$VD_NUMBER;id++)
				//if(inter[id])

				
				{
					e0[id] = Equ(ps.xyz,id,tex[id]);
					e[id] = ((e0[id]-min_level[id])/(max_level[id]-min_level[id]));
					if(e[id]>0.0)
					{
						#if $tf_type==0
						cl =  vec4(e[id]);
						#endif
						#if $tf_type==1
						cl =  GetLevelColor(e0[id],id);
						#endif

						cl.w *= ddd*opacity[id];
						#if $shade_mode==1
							norm = normalize(GradEqu1(e0[id],ps.xyz,id,tex[id])+vec3(0.000001));
							//cl.xyz = mix(Phong(ps.xyz,-norm,cl.xyz),Phong(ps.xyz,nrm,cl.xyz),max(start-ps.w+0.02,0.0)/0.02);
							if(dot(norm,ray)<0)norm=-norm;
							cl.xyz = Phong(ps.xyz,-norm,cl.xyz);
						#endif
						float d_alpha = (1.0-alpha) * cl.w;
						//if(alpha==0.0 && d_alpha!=0.0)gl_FragDepth = max(GetDepth(ps.xyz),0.1);
						mid_t += d_alpha*ps.w;
						color = color + d_alpha * cl.xyz;
						
						alpha = alpha + d_alpha;
						
					}
						
				}
				if(alpha>0.95)
				{
					alpha=1.0;
					break;
				}
				
				
				
				ps += step;
			}
			
#if USE_BOUNDING_MESH!=0

			if(alpha==1.0)break;
		}
#endif		
		

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
