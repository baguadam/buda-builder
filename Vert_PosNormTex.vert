#version 430

// VBO-ból érkező változók
layout( location = 0 ) in vec2 vs_in_tex;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;

// shader külső paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 viewProj;

uniform sampler2D heightMapTexture;

vec3 GetPos(float u, float v)
{
	float scaleValue = 100;
	float height = texture(heightMapTexture, vs_in_tex).r * scaleValue;
	return vec3(u, height, -v);
}

vec3 GetNorm(float u, float v)
{
	vec3 du = GetPos(u + 0.01, v) - GetPos(u - 0.01, v);
	vec3 dv = GetPos(u, v + 0.01) - GetPos(u, v - 0.01);

	return normalize(cross(du, dv));
}

void main()
{
	vec3 pos = GetPos(vs_in_tex.x, vs_in_tex.y);

	gl_Position = viewProj * world * vec4( pos, 1 );
	vs_out_pos  = (world   * vec4(pos,  1)).xyz;
	vs_out_norm = (worldIT * vec4(GetNorm(vs_in_tex.x, vs_in_tex.y), 0)).xyz;

	vs_out_tex = vs_in_tex;
}