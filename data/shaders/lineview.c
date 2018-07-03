//vertex_shader

uniform   mat4 u_mvpMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec4 a_color;

varying   vec3 v_normal;
varying   vec4 v_color;

void main()
{
  v_normal = a_normal;
  v_color = a_color;
  gl_Position = u_mvpMatrix * a_vertex;
}

//fragment_shader

varying vec4 v_color;


void main()
{
  gl_FragColor = v_color;
}
