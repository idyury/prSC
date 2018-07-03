//vertex_shader

uniform   mat4  u_mvMatrix;
uniform   mat4  u_mvpMatrix;
uniform   float u_depth;
uniform   float u_step;
uniform   vec2  u_direction;
uniform   vec2  u_startpoint;

attribute vec4 a_channel1;
attribute vec2 a_channel2;

varying   vec3 v_pos;
varying   float v_light;
varying   float v_type;

void main()
{
  vec4 vertex;
  vertex.x = a_channel1.x * u_step;
  vertex.y = a_channel2.x * u_depth;
  vertex.z = a_channel1.y * u_step;
  vertex.w = 1.0;
  v_light = a_channel1.z;
  v_type = 0.0625 + a_channel1.w * 28.0;
  gl_Position = u_mvpMatrix * vertex;
}

//fragment_shader
precision mediump float;

uniform sampler2D s_texture0;

varying float v_light;
varying float v_type;
uniform vec4 u_matambient;// = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform vec4 u_matdiffuse;// = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform vec4 u_matspecular;// = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform float u_matshininess;// = 64.0;

void main()
{
  vec4 color = texture2D(s_texture0, vec2(v_type, 0.5));
  gl_FragColor = color * v_light;
}
