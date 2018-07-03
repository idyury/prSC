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
  v_normal = vec3( u_mvMatrix * vec4( a_normal, 0.0 ) );
  v_pos = vec3( u_mvMatrix * a_vertex );
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

//repeat light_dir
uniform vec3 u_lightdir_#id;
uniform vec3 u_lighthalfdir_#id;
uniform vec3 u_lightdiffuse_#id;
uniform vec3 u_lightambient_#id;
uniform vec3 u_lightspecular_#id;
//repeat done


void main()
{
  vec3 l_normal = normalize( v_normal );
  vec4 l_result = vec4( 0.0, 0.0, 0.0, 1.0 );
  vec4 l_color = v_color;

//repeat light_dir
  vec3 l_light_#id = u_lightdiffuse_#id * max( dot( l_normal, u_lightdir_#id ), 0.0 );
  //vec3 l_lightspec_#id = u_lightspecular_#id * pow( max( dot( l_normal, u_lighthalfdir_#id ), 0.0 ), u_matshininess );
  l_result += vec4( l_light_#id, 1.0 ) * u_matdiffuse * l_color;
  //l_result += vec4( l_lightspec_#id, 1.0 ) * u_matspecular;
  l_result += vec4( u_lightambient_#id, 1.0 ) * u_matambient * l_color;
//repeat done

  gl_FragColor = l_result;
}
