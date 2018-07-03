//vertex_shader
uniform   mat4 u_mvpMatrix; 
attribute vec4 a_vertex;

void main()
{
	gl_Position = u_mvpMatrix * a_vertex;
}

//fragment_shader

void main()
{
	gl_FragColor = vec4( 1.0, 1.0, 1.0, 0.25 );
}
