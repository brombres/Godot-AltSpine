shader_type canvas_item;
uniform sampler2D SCREEN_TEXTURE:hint_screen_texture;

varying flat int blend_mode;

void vertex()
{
  blend_mode = int(COLOR.b*255.0) & 3;
  COLOR.b /= (252.0/255.0);
}

void fragment()
{
  if (blend_mode == 0)
  {
    COLOR.rgb = vec3( 1, 0, 0 );
    COLOR.a = 0.0;
  }
  else if (blend_mode == 1)
  {
    COLOR.rgb = vec3( 1, 1, 0 );
    COLOR.a = 0.0;
  }
  else if (blend_mode == 2)
  {
    COLOR.rgb = vec3( 0, 0, 1 );
    COLOR.a = 0.0;
  }
  else if (blend_mode == 3)
  {
    COLOR.rgb = vec3( 0, 1, 1 );
  }
  else
  {
    // Screen
		//COLOR.rgb = 1.0 - (1.0 - bg_color.rgb) * (1.0 - COLOR.rgb);
    COLOR.rgb = vec3( 0, 1, 0 );
  }
}

