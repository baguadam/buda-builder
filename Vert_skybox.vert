#version 430 core

// VBO-b�l �rkez� v�ltoz�k
layout (location = 0 ) in vec3 vs_in_pos;

// a pipeline-ban tov�bb adand� �rt�kek
out vec3 vs_out_pos;

// shader k�ls� param�terei - most a h�rom transzform�ci�s m�trixot k�l�n-k�l�n vessz�k �t
uniform mat4 world;
uniform mat4 viewProj;

void main()
{
	gl_Position = (viewProj * world * vec4( vs_in_pos, 1 )).xyww;

	vs_out_pos = vs_in_pos;
}