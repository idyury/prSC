//vertex_shader

uniform   mat4 u_mvpMatrix; 
attribute vec4 a_vertex;
attribute vec4 a_color;
varying   vec4 v_color;

void main()
{
	gl_Position = u_mvpMatrix * a_vertex;
	v_color = a_color;
}

//fragment_shader

varying vec4 v_color;

void main()
{
	gl_FragColor = v_color;
}

