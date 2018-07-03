//vertex_shader
//  Created by Yury Khmel on 21/6/12.
uniform   mat4 u_mvpMatrix; 
attribute vec2 a_vertex;
attribute vec4 a_color;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	v_color = a_color;
	gl_Position = u_mvpMatrix * vec4( a_vertex.x, a_vertex.y, 0.0, 1.0 );
}

//fragment_shader
precision lowp float;

varying vec4 v_color;

void main()
{
	gl_FragColor = v_color;
}
