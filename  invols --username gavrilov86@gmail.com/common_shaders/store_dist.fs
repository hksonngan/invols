varying vec3 vertex;
uniform vec3 pos,nav;

uniform sampler2D dist_txt;

uniform float screen_width;
uniform float screen_height;
uniform float free_depth;
uniform int fdepth;

const float z_near=0.1;
const float z_far=30.0;

void main()
{
	float dist = length(pos-vertex);
	vec2 text_coord = gl_FragCoord.xy/vec2(screen_width,screen_height);
	vec4 res = texture2D(dist_txt,text_coord);

	if(fdepth==0 || dist>res[fdepth-1])//+0.00001)
	{
		res[fdepth] = dist;
		gl_FragDepth = clamp(z_far/(z_far-z_near) - (z_far*z_near/(z_far-z_near))/(dot(vertex-pos,nav)),0.01,0.99);
	}else
	{
		gl_FragDepth = free_depth;
	}
	gl_FragColor = res;

}