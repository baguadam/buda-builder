#version 430

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimen� �rt�k - a fragment sz�ne
out vec4 fs_out_col;

// text�ra mintav�telez� objektum
uniform sampler2D texImage;

uniform vec3 cameraPos;

// fenyforras tulajdonsagok
uniform vec4 lightPosFirst = vec4(0.0, 1.0, 0.0, 0.0);
uniform vec4 lightPosSecond = vec4(0.0, 1.0, 0.0, 0.0);
uniform vec4 sunMoonDirection = vec4( 0.0, 1.0, 0.0, 0.0);

// nap/hold f�ny�nek aktu�lis sz�ne
uniform vec3 sunMoonLightColor = vec3(1.0, 0.0, 0.0);

uniform vec3 La = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls = vec3(1.0, 1.0, 1.0 );

uniform float lightConstantAttenuation    = 1.0;
uniform float lightLinearAttenuation      = 0.0;
uniform float lightQuadraticAttenuation   = 0.0;

// anyag tulajdonsagok
uniform vec3 Ka = vec3( 1.0 );
uniform vec3 Kd = vec3( 1.0 );
uniform vec3 Ks = vec3( 1.0 );

uniform float Shininess = 1.0;

void main()
{
	vec3 normal = normalize( vs_out_norm );
	vec3 Ambient = La * Ka;

	//***************************************************
	//***************************************************
	//*************** NAP/HOLD F�NYE ********************
	vec3  SunMoonDir = normalize(sunMoonDirection.xyz); // mivel ir�nyf�nyforr�s
	float SunMoonDiffuseFactor = max(dot(SunMoonDir, normal), 0.0);
	vec3  SunMoonDiffuse = SunMoonDiffuseFactor * sunMoonLightColor;

	// Spekul�ris komponens
	vec3  viewDir = normalize( cameraPos - vs_out_pos ); // A fragmentb�l a kamer�ba mutat� vektor
	vec3  reflectDirSunMoon = reflect( -SunMoonDir, normal ); // T�k�letes visszaver�d�s vektora
	float SpecularFactorSunMoon = pow(max(dot( viewDir, reflectDirSunMoon) ,0.0), Shininess);
	vec3  SpecularSunMoon = SpecularFactorSunMoon*Ls*Ks;

	//***************************************************
	//***************************************************
	//*************** ALAP F�NYFORR�S *******************
	vec3 ToLight; // A f�nyforr�sBA mutat� vektor
	float LightDistance=0.0; // A f�nyforr�st�l vett t�vols�g
	
	if ( lightPosFirst.w == 0.0 ) // ir�ny f�nyforr�s (directional light)
	{
		ToLight	= lightPosFirst.xyz;
	}
	else // pont f�nyforr�s (point light)
	{
		ToLight	= lightPosFirst.xyz - vs_out_pos;
		LightDistance = length(ToLight);
	}
	//  Normaliz�ljuk a f�nyforr�sba mutat� vektort
	ToLight = normalize(ToLight);
	
	// Attenu�ci� (f�nyelhal�s) kisz�m�t�sa
	float Attenuation = 1.0 / ( lightConstantAttenuation + lightLinearAttenuation * LightDistance + lightQuadraticAttenuation * LightDistance * LightDistance);

	// Diff�z komponens
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * Ld * Kd;
	
	// Spekul�ris komponens
	vec3 reflectDir = reflect( -ToLight, normal ); // T�k�letes visszaver�d�s vektora
	
	// A spekul�ris komponens
	float SpecularFactor = pow(max(dot( viewDir, reflectDir) ,0.0), Shininess) * Attenuation;
	vec3 Specular = SpecularFactor*Ls*Ks;

	fs_out_col = vec4(Ambient + Diffuse + SunMoonDiffuse + Specular + SpecularSunMoon, 1.0) * texture(texImage, vs_out_tex);
}