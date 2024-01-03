#version 430

// VBO-ból érkezõ változók
layout( location = 0 ) in vec2 vs_in_tex;

out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;

uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 viewProj;

uniform float elapsedTimeSec = 0.0; // leküldjük az eltelt idõt a hullámzáshoz

vec3 GetPos(float u, float v) 
{
	return vec3(u, (sin(v * 11 + elapsedTimeSec * 3) * cos(elapsedTimeSec)) * 10 + 35, -v); // húszszorozára vesszük a méretét, majd elotljuk középre
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

	gl_Position = viewProj * world * vec4(pos, 1);
	vs_out_pos  = (world   * vec4(pos,  1)).xyz;
	vs_out_norm = (worldIT * vec4(GetNorm(vs_in_tex.x, vs_in_tex.y), 0)).xyz;

	vs_out_tex = vs_in_tex;
}