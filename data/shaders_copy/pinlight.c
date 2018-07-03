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
	vec4 l_colorblend = texture2D( s_texture0, v_texcoord );
	vec4 l_colorback  = texture2D( s_texture1, v_texcoord1 );
	vec4 l_colortarget = l_colorback;
	float l_brblend = 0.299 * l_colorblend.x + 0.587 * l_colorblend.y + 0.114 * l_colorblend.z;
	float l_brback = 0.299 * l_colorback.x + 0.587 * l_colorback.y + 0.114 * l_colorback.z;
	if ( l_brblend > 0.505 )
	{
		if( l_brblend > l_brback ) l_colortarget = l_colorblend;
	}
	else if ( l_brblend < 0.495 )
	{
		if( l_brblend < l_brback ) l_colortarget = l_colorblend;
	}
		
	gl_FragColor = l_colortarget * l_colorblend.w + l_colorback * (1.0 - l_colorblend.w);
}
