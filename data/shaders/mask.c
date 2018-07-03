//vertex_shader
uniform   mat4 u_mvpMatrix; 
attribute vec2 a_vertex;

void main()
{
	gl_Position = u_mvpMatrix * vec4( a_vertex.x, a_vertex.y, -0.5, 1.0 );
}

//fragment_shader
#ifdef OPENGL_ES
precision lowp float;
#endif

void main()
{
	gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
}
