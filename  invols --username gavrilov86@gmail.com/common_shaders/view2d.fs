varying vec3 vertex,normal;

uniform sampler2D txt;
uniform float max_level,min_level;
uniform sampler2D txt_level_color;
uniform vec4 params;
uniform vec2 size;
varying vec2 text_coord;

vec4 GetLevelColor(float lv) 
{
	return texture2D(txt_level_color, vec2((lv - min_level)/(max_level-min_level),0.5));
}

float interpolate_bicubic(sampler2D tex, vec2 coord)
{
coord*=size;
	// transform the coordinate from [0,extent] to [-0.5, extent-0.5]
	vec2 coord_grid = coord-vec2(0.5);
	vec2 index = floor(coord_grid);
	vec2 fraction = coord_grid - index;
	vec2 one_frac = vec2(1.0) - fraction;
	vec2 one_frac2 = one_frac * one_frac;
	vec2 fraction2 = fraction * fraction;

	vec2 w0 = 1.0/6.0 * one_frac2 * one_frac;
	vec2 w1 = vec2(2.0/3.0) - 0.5 * fraction2 * (2.0-fraction);
	vec2 w2 = vec2(2.0/3.0) - 0.5 * one_frac2 * (2.0-one_frac);
	vec2 w3 = 1.0/6.0 * fraction2 * fraction;
	vec2 g0 = w0 + w1;
	vec2 g1 = w2 + w3;
	// h0 = w1/g0 - 1, move from [-0.5, extent-0.5] to [0, extent]
	vec2 h0 = (w1 / g0) - vec2(0.5) + index;
	vec2 h1 = (w3 / g1) + vec2(1.5) + index;
	// fetch the four linear interpolations
	h0/=size;
	h1/=size;
	float tex00 = texture2D(tex, h0).x;
	float tex10 = texture2D(tex, vec2(h1.x, h0.y)).x;
	float tex01 = texture2D(tex, vec2(h0.x, h1.y)).x;
	float tex11 = texture2D(tex, h1).x;
	// weigh along the y-direction
	tex00 = mix(tex01, tex00, g0.y);
	tex10 = mix(tex11, tex10, g0.y);
	// weigh along the x-direction
	return mix(tex10, tex00, g0.x);
}
void main()
{
	float val = params.x>0.5 ? interpolate_bicubic(txt, text_coord.xy) : texture2D(txt, text_coord.xy).x;
	vec3 color = params.y>0.5 ? GetLevelColor(val).xyz : vec3((val - min_level)/(max_level-min_level));
	gl_FragColor = vec4(color,1.0);
	
}
