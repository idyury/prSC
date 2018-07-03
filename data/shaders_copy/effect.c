//vertex_shader

uniform   mat4 u_mvMatrix; 
uniform   mat4 u_mvpMatrix; 

attribute vec4 a_vertex;
attribute vec4 a_color;
attribute vec2 a_texcoord;

varying   vec3 v_pos;
varying   vec4 v_color;
varying   vec2 v_texcoord;

void main()
{
	v_texcoord = a_texcoord;
	v_color = a_color;
	gl_Position = u_mvpMatrix * a_vertex;
}

//fragment_shader

uniform sampler2D s_texture0;
varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	vec4 l_color = texture2D( s_texture0, vec2( v_texcoord.x, v_texcoord.y ) );
	l_color = l_color * v_color;

	gl_FragColor = l_color;
}
