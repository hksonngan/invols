varying vec3 vertex;
uniform sampler3D f_text;

uniform vec3 cell_size0,cell_size1;

float interpolate_cubic(sampler3D tex, vec3 coord,vec3 cell_size)
{
	coord/=cell_size;
	// transform the coordinate from [0,extent] to [-0.5, extent-0.5]
	vec3 coord_grid = coord-vec3(0.5);
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
	h0*=cell_size;
	h1*=cell_size;

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

void main()
{
	//float res = texture3D(f_text,vertex).x;
	float res = interpolate_cubic(f_text,vertex,cell_size0).x;

	gl_FragColor = vec4(res);//vec4(cur_coord.x);
}