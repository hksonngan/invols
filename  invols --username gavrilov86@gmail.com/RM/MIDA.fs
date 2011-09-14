
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
	
		ps+=step*(start/step_length);
	
		gl_FragDepth = 1.0;
		/*
		#if $use_accel_struct==1
			for(float i=0.0;i<100.0;i++)
			{
				if(!(ps.w <final) || EquI(ps.xyz,0)>=min_level[0])break;
				//if(!(ps.w <final) || GetLevelColor(EquI(ps.xyz,0),0).w>=0.01)break;
				
				ps += step*5.0;
			}		
				ps -= step*5.0;
		#endif*/
		ps+=step*rand(vec4(vertex,ps.w));
		float max_i[$VD_NUMBER];
		for(id=0;id<$VD_NUMBER;id++)
			max_i[id] = clamp((Equ(ps.xyz,id)-min_level[id])/(max_level[id]-min_level[id]),0.0,1.0);
		


			while(ps.w < final)
			{
			
				for(id=0;id<$VD_NUMBER;id++)
				{
					e0[id] = Equ(ps.xyz,id);
		
					e[id] = min((e0[id]-min_level[id])/(max_level[id]-min_level[id]),1.0);
					if(e[id]>0.0)
					{
						#if $IsoNum>0
						float bb = 1.0 - my_color[0].w*2.0*max(e[id] - max_i[id],0.0);
						#else
						float bb = 1.0 - max(e[id] - max_i[id],0.0);
						#endif
						
						max_i[id] = max(max_i[id],e[id]);
						#if $tf_type==0
						vec4 cl =  vec4(e[id]);
						#endif
						#if $tf_type==1
						vec4 cl = GetLevelColor(e0[id],id);
						#endif
						cl.w *= ddd*opacity[id];
						#if $shade_mode==1
							norm = GradEqu(ps.xyz,id);
							cl.xyz = Phong(ps.xyz,-normalize(norm+vec3(0.0001)),cl.xyz);
						#endif
						color *= bb;
						alpha *= bb;
						color = color + (1.0-alpha)*cl.w*cl.xyz;
						alpha = alpha + (1.0-alpha)*cl.w;
					}
					
					
				}
				
				if(alpha*max_i[0]>0.99)
				{
					break;
				}
				
				
				ps+=step;
			}
			
	
			
		
		gl_FragDepth = max(GetDepth(ps.xyz),0.1);

		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,alpha);
		
		
	}	else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}
	
}


