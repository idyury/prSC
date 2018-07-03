//vertex_shader
attribute vec2 a_vertex;
attribute vec2 a_texcoord;

uniform mat4 u_mvpMatrix;
uniform mat4 u_texMatrix;
varying vec2 v_texcoord;
varying vec2 v_texcoord2;

void main()
{
	v_texcoord = a_texcoord;
	vec4 l_texc = u_texMatrix * vec4( v_texcoord.x, v_texcoord.y, 0.0, 1.0 );
	v_texcoord2 = vec2( l_texc.x, l_texc.y );
	gl_Position = u_mvpMatrix * vec4( a_vertex.x, a_vertex.y, 0.0, 1.0 );
}

//fragment_shader
#ifdef OPENGL_ES
precision lowp float;
#endif

uniform sampler2D s_texture0;
uniform sampler2D s_texture1;
varying vec2 	  v_texcoord;
varying vec2 	  v_texcoord2;

void main()
{
	vec4 l_color = texture2D( s_texture0, v_texcoord2 );
	vec4 l_color2 = texture2D( s_texture1, v_texcoord );
	l_color.w = l_color.w * (1.0 - l_color2.w);
	gl_FragColor = l_color;
}
