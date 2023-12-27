#version 430

// VBO-ból érkező változók
layout( location = 0 ) in vec3 vs_in_pos;
layout( location = 1 ) in vec3 vs_in_norm;
layout( location = 2 ) in vec2 vs_in_tex;

// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec3 vs_out_norm;
out vec2 vs_out_tex;

// shader külső paraméterei - most a három transzformációs mátrixot külön-külön vesszük át
uniform mat4 world;
uniform mat4 worldIT;
uniform mat4 viewProj;

uniform sampler2D heightMapTexture;

void main()
{
   float scaleValue = 0.9;
   float height = texture(heightMapTexture, vs_in_tex).r * scaleValue;

   gl_Position = viewProj * world * vec4(vs_in_pos.x, vs_in_pos.y + height, vs_in_pos.z, 1);
   vs_out_pos = (world  * vec4(vs_in_pos.x, vs_in_pos.y + height, vs_in_pos.z, 1)).xyz;
   vs_out_norm = (worldIT * vec4(vs_in_norm, 0)).xyz;

   vs_out_tex = vs_in_tex;
}