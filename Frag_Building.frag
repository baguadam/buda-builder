#version 430

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenõ érték - a fragment színe
out vec4 fs_out_col;

// textúra mintavételezõ objektum
uniform sampler2D texImage;

void main()
{
	fs_out_col = texture(texImage, vs_out_tex);
}