#version 430

// pipeline-ból bejövõ per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenõ érték - a fragment színe
out vec3 fs_out_col;

// textúra mintavételezõ objektum
uniform sampler2D heightMapTexture;
uniform sampler2D splatMapTexture;

void main()
{
	fs_out_col = vec3(vs_out_tex.x, vs_out_tex.y, 1); 
}