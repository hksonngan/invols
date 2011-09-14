varying vec3 vertex;
varying vec2 text_coord;

uniform sampler2D f_text;
uniform vec4 txt_size;



vec4 interpolate_cubic(sampler2D tex, vec2 coord_grid,vec2 tex_size)
{
	// transform the coordinate from [0,extent] to [-0.5, extent-0.5]
	coord_grid -= vec2(0.5);
	vec2 index = floor(coord_grid);
	vec2 fraction = coord_grid - index;
	vec2 one_frac = 1.0 - fraction;
	vec2 one_frac2 = one_frac * one_frac;
	vec2 fraction2 = fraction * fraction;
	vec2 w0 = 1.0/6.0 * one_frac2 * one_frac;
	vec2 w1 = 2.0/3.0 - 0.5 * fraction2 * (2.0-fraction);
	vec2 w2 = 2.0/3.0 - 0.5 * one_frac2 * (2.0-one_frac);
	vec2 w3 = 1.0/6.0 * fraction2 * fraction;
	vec2 g0 = w0 + w1;
	vec2 g1 = w2 + w3;
	// h0 = w1/g0 - 1, move from [-0.5, extent-0.5] to [0, extent]
	vec2 h0 = (w1 / g0) - 0.5 + index;
	vec2 h1 = (w3 / g1) + 1.5 + index;
	// fetch the four linear interpolations
	h0 /= tex_size;
	h1 /= tex_size;

	vec4 tex00 = texture2D(tex,vec2(h0.x, h0.y)).xyzw;
	vec4 tex10 = texture2D(tex,vec2(h1.x, h0.y)).xyzw;
	vec4 tex01 = texture2D(tex,vec2(h0.x, h1.y)).xyzw;
	vec4 tex11 = texture2D(tex,vec2(h1.x, h1.y)).xyzw;
	// weigh along the y-direction
	tex00 = mix(tex01, tex00, g0.y);
	tex10 = mix(tex11, tex10, g0.y);
	// weigh along the x-direction
	return mix(tex10, tex00, g0.x);
}

void main()
{
	vec4 res;
	res = interpolate_cubic(f_text,text_coord.xy*txt_size.xy,txt_size.xy).xyzw;
	if(txt_size.z>0.5)
		res = texture2D(f_text,text_coord).xyzw;

	gl_FragColor = vec4(res);
}