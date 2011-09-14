
vec3 CalcNormByDist(vec3 ps,vec3 ray,int otr)
{
	vec2 pixx = vec2(0.2)/vec2(screen_width,screen_height);
	
	float d0 = texture2D(front_dist_txt,text_coord)[otr];
	float d1 = texture2D(front_dist_txt,text_coord+vec2(pixx.x,0.0))[otr];
	float d2 = texture2D(front_dist_txt,text_coord+vec2(0.0,pixx.y))[otr];
	
	//vec3 ray0 = nav + 0.4142 * (left * (text_coord.x*2.0-1.0)*(pixx.y/pixx.x) + top * (text_coord.y*2.0-1.0));
	vec3 ray1 = nav + 0.4142 * (left * (text_coord.x*2.0-1.0+pixx.x)*(pixx.y/pixx.x) + top * (text_coord.y*2.0-1.0));
	vec3 ray2 = nav + 0.4142 * (left * (text_coord.x*2.0-1.0)*(pixx.y/pixx.x) + top * (text_coord.y*2.0-1.0+pixx.y));
	vec3 pt = ps + normalize(ray)*d0;
	vec3 v1 = ps + normalize(ray1)*d1-pt;
	vec3 v2 = ps + normalize(ray2)*d2-pt;

	return -normalize(cross(v1,v2));

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
	float start=1000.0,final=0.0;
	int id;
	bool inter[$VD_NUMBER];

	vec3 color = vec3(0.0),norm;
	vec4 step  = vec4(ray*step_length,step_length);
	float op = 1.0;
	float e[$VD_NUMBER],e0[$VD_NUMBER],s,f;

	for(id=0;id<$VD_NUMBER;id++)
	{
		e[id] = 0.0;
		vec3 ray1 = ToTextureSpace1(ray,id);
		float ll = length(ray1);
		inter[id] = IntersectBox (ToTextureSpace(ps.xyz,id),ray1,vec3(0.0),vec3(1.0), s,f );
		if(opacity[id]<0.1)
			inter[id]=false;
		if(inter[id])
		{
		
			start = min(start,s);
			final = max(final,f);
		}
	}
	vec4 front_face_dist;
	vec4 back_face_dist;
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
		front_face_dist = texture2D(front_dist_txt,text_coord);
		back_face_dist = texture2D(back_dist_txt,text_coord);
		if(front_face_dist.x>back_face_dist.x)front_face_dist = vec4(0.0,front_face_dist.xyz);
		int otr=0;
		for(int otr;otr<4;otr++)
		{

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
			
				for(id = 0;id<$VD_NUMBER;id++)
				{
					e0[id]=e[id];
					e[id] = Equ(ps.xyz,id);
					if(e[id] == clamp(e[id], min_level[id], max_level[id]))
					{
						op = 0.0;
						//id1=id;
					}
				
				}
				
				
				ps+=step;
				if(op==0.0)break;
				
			}
			
	
	#if USE_BOUNDING_MESH!=0
			//if(op==0.0)break;

		}
#endif	
		id = -1;
		for(int ii = 0;ii<$VD_NUMBER;ii++)
		{
			if(e[ii] == clamp(e[ii], min_level[ii], max_level[ii]))
			{
				id=ii;
			}
				
		}
		if(id!=-1)
		{

			ps += step*((min_level[id]-e[id])/(e[id]-e0[id])-1.0);
			#if USE_BOUNDING_MESH!=0
			if(ps.w-front_face_dist[otr]<step.w*2.0)
				norm = CalcNormByDist(ps0.xyz,ray,otr);
			else
			#endif
				norm = sign(e0[id]-e[id])*normalize(GradEqu(ps.xyz,id));
			if(dot(norm,ray)>0.0)norm=-norm;
			color = Phong(ps.xyz,norm,GetLevelColor(e[id],id).xyz);
			gl_FragDepth = GetDepth(ps.xyz);
		}
		
		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,1.0-op);
		
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}
	
}


