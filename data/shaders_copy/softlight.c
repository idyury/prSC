//vertex_shader
uniform   mat4 u_mvpMatrix; 
attribute vec2 a_vertex;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec2 v_texcoord1;

void main()
{
	v_texcoord = a_texcoord;
	vec4 l_pos = u_mvpMatrix * vec4( a_vertex.x, a_vertex.y, 0.0, 1.0 );
	v_texcoord1 = vec2( 0.5 * ( l_pos.x + 1.0 ), 0.5 * ( 1.0 - l_pos.y ) );
	gl_Position = l_pos;
}

//fragment_shader
#ifdef OPENGL_ES
precision lowp float;
#endif

uniform sampler2D s_texture0;
uniform sampler2D s_texture1;
varying vec2 v_texcoord;
varying vec2 v_texcoord1;

void main()
{
	vec4 l_colorcor  = texture2D( s_texture0, v_texcoord );
	vec4 l_colorback = texture2D( s_texture1, v_texcoord1 );

	float c = 0.299 * l_colorcor.x + 0.587 * l_colorcor.y + 0.114 * l_colorcor.z;
	float s = 0.299 * l_colorback.x + 0.587 * l_colorback.y + 0.114 * l_colorback.z;

	float sc;
	if ( c >= 0.500 )
		sc =  ( 2.0 * c - 1.0 ) / sqrt( s ) + 2.0 * ( 1.0 - c );
	else 
		sc =  ( 1.0 - 2.0 * c ) * s + 2.0 * c;

	vec4 l_colortarget = l_colorback * sc;
	gl_FragColor = l_colortarget * l_colorcor.w + l_colorback * (1.0 - l_colorcor.w);
}
