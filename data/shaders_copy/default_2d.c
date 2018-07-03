//vertex_shader
//  Created by Yury Khmel on 21/6/12.
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
precision lowp float;

uniform sampler2D s_texture0;
varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	vec4 l_color = texture2D( s_texture0, vec2( v_texcoord.x, v_texcoord.y ) );
	gl_FragColor = l_color * v_color;
}
