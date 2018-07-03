//vertex_shader
attribute vec2 a_vertex;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;

void main()
{
	v_texcoord = a_texcoord;
	gl_Position = vec4( a_vertex.x, a_vertex.y, 0.0, 1.0 );
}

//fragment_shader
#ifdef OPENGL_ES
precision lowp float;
#endif

uniform sampler2D s_texture0;
varying vec2 v_texcoord;

void main()
{
	gl_FragColor = texture2D( s_texture0, v_texcoord );
}
