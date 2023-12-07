shader_type canvas_item;
uniform sampler2D SCREEN_TEXTURE:hint_screen_texture;
render_mode blend_mix;

void fragment()
{
	vec4 bg_color = texture( SCREEN_TEXTURE, SCREEN_UV );
	COLOR.rgb = 1.0 - (1.0 - bg_color.rgb) * (1.0 - COLOR.rgb);
}
