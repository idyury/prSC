//vertex_shader

uniform   mat4 u_mvpMatrix; 
attribute vec4 a_vertex;
attribute vec4 a_color;
attribute vec3 a_normal;
varying   vec4 v_color;
varying   vec3 v_normal;

void main()
{
	gl_Position = u_mvpMatrix * a_vertex;
	v_color = a_color;
	v_normal = a_normal;
}

//fragment_shader

varying vec4 v_color;
varying vec3 v_normal;

void main()
{
	gl_FragColor = vec4( v_normal, 1.0 );
}

