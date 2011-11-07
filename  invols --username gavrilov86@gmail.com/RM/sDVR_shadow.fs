


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
