//vertex_shader
uniform   mat4 u_mvpMatrix; 
attribute vec2 a_vertex;
attribute vec2 a_texcoord;
attribute vec4 a_color;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	v_texcoord = a_texcoord;
	v_color = a_color;
	gl_Position = u_mvpMatrix * vec4( a_vertex.x, a_vertex.y, 0.0, 1.0 );
}

//fragment_shader
#ifdef OPENGL_ES
precision lowp float;
#endif

uniform sampler2D s_texture0;
varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	float l_alpha = texture2D( s_texture0, vec2( v_texcoord.x, v_texcoord.y ) ).w;
	gl_FragColor = v_color * vec4( 1.0, 1.0, 1.0, l_alpha );
}
