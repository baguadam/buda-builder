#version 430

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimen� �rt�k - a fragment sz�ne
out vec3 fs_out_col;

// text�ra mintav�telez� objektum
uniform sampler2D heightMapTexture;
uniform sampler2D splatMapTexture;

void main()
{
	fs_out_col = vec3(vs_out_tex.x, vs_out_tex.y, 1); 
}