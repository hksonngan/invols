varying vec3 vertex;
uniform sampler3D f_text;

uniform vec3 cell_size0,cell_size1;

#define kernel_size 2.0

void main()
{
	float res = 0.0,ww = 0.0;
	vec3 dd, kernel_size1 = cell_size0*kernel_size;
	for(dd.x = -kernel_size1.x; dd.x<=kernel_size1.x; dd.x+=cell_size0.x)
	for(dd.y = -kernel_size1.y; dd.y<=kernel_size1.y; dd.y+=cell_size0.y)
	for(dd.z = -kernel_size1.z; dd.z<=kernel_size1.z; dd.z+=cell_size0.z)
	{
		float w = 1.0/(1.0+length(dd));
		ww+=w;
		res += texture3D(f_text,vertex + dd).x*w;
	}

	gl_FragColor = vec4(res/ww);//vec4(cur_coord.x);
}