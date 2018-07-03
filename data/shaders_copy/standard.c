//vertex_shader

uniform   mat4 u_mvMatrix; 
uniform   mat4 u_mvpMatrix; 

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying   vec3 v_pos;
varying   vec3 v_normal;
varying   vec2 v_texcoord;

void main()
{
	v_texcoord = a_texcoord;
	v_normal = vec3( u_mvMatrix * vec4( a_normal, 0.0 ) );
	v_pos = vec3( u_mvMatrix * a_vertex );

	gl_Position = u_mvpMatrix * a_vertex;
}

//fragment_shader

uniform sampler2D s_texture0;
varying vec2 v_texcoord;
varying vec3 v_pos;
varying vec3 v_normal;
uniform vec4 u_matambient = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform vec4 u_matdiffuse = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform vec4 u_matspecular = vec4( 1.0, 1.0, 1.0, 1.0 );
uniform float u_matshininess = 64.0;

//repeat light_dir
uniform vec3 u_lightdir_#id;
uniform vec3 u_lighthalfdir_#id;
uniform vec3 u_lightdiffuse_#id;
uniform vec3 u_lightambient_#id;
uniform vec3 u_lightspecular_#id;
//repeat done

//repeat light_point
uniform vec3 u_lightpos_#id;
uniform vec3 u_lightdiffuse_#id;
uniform vec3 u_lightambient_#id;
uniform vec3 u_lightspecular_#id;
uniform vec3 u_lightattenuation_#id = vec3( 1.0, 0.0, 0.0 );
//repeat done

void main()
{
	vec3 l_normal = normalize( v_normal );
	vec4 l_result = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec4 l_color = texture2D( s_texture0, vec2( v_texcoord.x, 1.0 - v_texcoord.y ) );

//repeat light_dir
 	vec3 l_light_#id = u_lightdiffuse_#id * max( dot( l_normal, u_lightdir_#id ), 0.0 );
 	vec3 l_lightspec_#id = u_lightspecular_#id * pow( max( dot( l_normal, u_lighthalfdir_#id ), 0.0 ), u_matshininess );
 	l_result += vec4( l_light_#id, 1.0 ) * u_matdiffuse * l_color;
 	l_result += vec4( l_lightspec_#id, 1.0 ) * u_matspecular;
 	l_result += vec4( u_lightambient_#id, 1.0 ) * u_matambient;
//repeat done

//repeat light_point
	vec3 l_lightdir_#id = u_lightpos_#id - v_pos;
	float l_dist_#id = length( l_lightdir_#id );
	float l_scale_#id = 1.0 / ( u_lightattenuation_#id.x + l_dist_#id * u_lightattenuation_#id.y + l_dist_#id * l_dist_#id * u_lightattenuation_#id.z );
	l_lightdir_#id = normalize( l_lightdir_#id );
 	vec3 l_light_#id = u_lightdiffuse_#id * max( dot( l_normal, l_lightdir_#id ), 0.0 );
	vec3 l_specdir_#id = reflect( -l_lightdir_#id, l_normal );
	vec3 l_eyedir_#id = normalize( -v_pos );
 	vec3 l_lightspec_#id = u_lightspecular_#id * pow( max( dot( l_eyedir_#id, l_specdir_#id ), 0.0 ), u_matshininess * 2.1 );
 	l_result += vec4( l_light_#id * l_scale_#id, 1.0 ) * u_matdiffuse;
 	l_result += vec4( l_lightspec_#id * l_scale_#id, 1.0 ) * u_matspecular;
 	l_result += vec4( u_lightambient_#id, 1.0 ) * u_matambient;
//repeat done

	gl_FragColor = l_result;
}
