
varying vec3 vertex,normal;

uniform sampler2D txt;
uniform float max_level,min_level;
uniform sampler2D txt_level_color;
uniform vec3 LightDir;
uniform vec3 pos,scale;

#define K_A 0.3
#define K_D 0.7
#define K_S 0.2
#define P 16.0

vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
	float diffuse = dot ( -LightDir, normal )*0.5+0.5;
	vec3 reflect = reflect ( LightDir, normal );
	float specular = pow ( max (0.0, dot ( reflect, normalize(pos-point) ) ), P );
	return K_A * color + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}
vec4 GetLevelColor(float lv) 
{
	return texture2D(txt_level_color, vec2((lv - min_level)/(max_level-min_level),0.5));
}

void main()
{
	vec3 vert = vertex/scale;
	vec2 vv = vert.xy;
	if(normal.yz == vec2(0.0)) vv = vert.yz;
	if(normal.xz == vec2(0.0)) vv = vert.xz;
//	vec3 color = GetLevelColor(texture2D(txt, vv).x).xyz;
	vec3 color = vec3((texture2D(txt, vv).x - min_level)/(max_level-min_level));
	
	gl_FragColor = vec4(Phong(vertex,normal,color),1.0);
	
}
