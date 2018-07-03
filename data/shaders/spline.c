//vertex_shader

uniform   mat4 u_mvMatrix;
uniform   mat4 u_mvpMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec4 a_color;

varying   vec3 v_pos;
varying   vec3 v_normal;
varying   vec4 v_color;

void main()
{
  v_color = a_color;
  gl_Position = u_mvpMatrix * a_vertex;
}

//fragment_shader

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_color;
uniform vec4 u_matambient;// = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform vec4 u_matdiffuse;// = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform vec4 u_matspecular;// = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform float u_matshininess;// = 64.0;

void main()
{
  gl_FragColor = v_color;
}
