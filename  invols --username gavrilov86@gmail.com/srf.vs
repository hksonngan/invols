
varying vec3 vertex,normal;

void main(void)
{
	vertex=vec3(gl_Vertex);
	normal=vec3(gl_Normal);
    gl_Position     = ftransform();

}
