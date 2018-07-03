//vertex_shader

uniform   mat4 u_mvpMatrix;

attribute vec4 a_vertex;


void main()
{
  gl_PointSize = 5.0;
  gl_Position = u_mvpMatrix * a_vertex;
}

//fragment_shader

void main()
{
  gl_FragColor = vec4(0.4, 0.45, 0.95, 0.25);
}
