//vertex_shader

uniform   mat4  u_mvMatrix;
uniform   mat4  u_mvpMatrix;
uniform   float u_depth;
uniform   float u_step;
uniform   vec2  u_direction;
uniform   vec2  u_startpoint;
uniform sampler2D s_texture1;

attribute vec4 a_channel1;
attribute vec2 a_channel2;

varying   vec3 v_pos;
varying   float v_light;
varying   float v_type;

void main()
{
  vec4 vertex;
  vec4 pixel = texture2D(s_texture1, vec2(a_channel1.x * 0.5, a_channel1.y * 0.5));
  vertex.x = a_channel1.x * u_step;
  vertex.y = 4.0 * pixel.r * u_depth;
  vertex.z = a_channel1.y * u_step;
  vertex.w = 1.0;
  v_light = a_channel1.z;
  v_type = 0.0625 + a_channel1.w * 28.0;
  gl_Position = u_mvpMatrix * vertex;
}

//fragment_shader

uniform sampler2D s_texture0;

varying float v_light;
varying float v_type;

void main()
{
  vec4 color = texture2D(s_texture0, vec2(v_type, 0.5));
  gl_FragColor = color * v_light;
}
