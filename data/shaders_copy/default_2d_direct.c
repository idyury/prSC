//vertex_shader
uniform   mat4 u_mMatrix; 
attribute vec2 a_vertex;
attribute vec2 a_texcoord;
attribute vec4 a_color;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	v_texcoord = a_texcoord;
	v_color = a_color;
	vec4 l_pos = vec4( a_vertex.x, a_vertex.y, 0.0, 1.0 );
	l_pos = u_mMatrix * l_pos;
	l_pos.y = - l_pos.y;
	gl_Position = l_pos;
}

//fragment_shader
uniform sampler2D s_texture0;
varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	vec4 l_color = texture2D( s_texture0, vec2( v_texcoord.x, v_texcoord.y ) );
	gl_FragColor = v_color * l_color;
}
