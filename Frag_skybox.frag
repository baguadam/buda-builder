#version 430

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 vs_out_pos;

out vec4 fs_out_col;

// procedur�lis sz�nek
uniform vec3 lightColor = vec3(1.0, 0.0, 0.0);

void main()
{
	vec3 groundColor = vec3(lightColor.x, lightColor.y, lightColor.z + 0.2);
	fs_out_col = vec4( mix(groundColor, lightColor, normalize(vs_out_pos).y * 0.5 + 0.5 ), 1.0);
}