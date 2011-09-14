
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

	vec4 color = vec4(0.0),norm;
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
	if(start<final)
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

		float max_i[$VD_NUMBER];
		
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
			
				for(id=0;id<$VD_NUMBER;id++)
				{
					e[id] = Equ(ps.xyz,id);
					max_i[id] = max(max_i[id],e[id]);

				}
				
				ps+=step;
			}
			
			
#if USE_BOUNDING_MESH!=0

			
		}
#endif		
		for(id=0;id<$VD_NUMBER;id++)
		{
			#if $tf_type==1
				vec4 cc = GetLevelColor(max_i[id],id);
				max_i[id] = (max_i[id]-min_level[id])/(max_level[id]-min_level[id]);
				color +=  vec4(cc.xyz,max_i[id]*cc.w);
			#endif
		
			#if $tf_type==0
				max_i[id] = (max_i[id]-min_level[id])/(max_level[id]-min_level[id]);
				color +=  vec4(max_i[id]);
			#endif
		}
		color /= float($VD_NUMBER);

		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color.xyz*m_anag,color.w);
		gl_FragDepth = max(GetDepth(ps.xyz-step.xyz*5.0),0.1);
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,1.0);
		gl_FragDepth=1.0;
	}
	
}


