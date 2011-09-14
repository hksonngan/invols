
varying vec3 vertex,normal;
varying vec2 text_coord;

void main(void)
{
	vertex=vec3(gl_Vertex);
	normal=vec3(gl_Normal);
	gl_TexCoord[0]  = gl_MultiTexCoord0;
	text_coord = vec2(gl_TexCoord[0].st);
    vec4 ff = ftransform(); 
	gl_Position = ff;

}
