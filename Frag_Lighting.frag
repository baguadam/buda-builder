#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenő érték - a fragment színe
out vec4 fs_out_col;

// textúra mintavételező objektum
uniform sampler2D heightMapTexture;
uniform sampler2D splatMapTexture;
uniform sampler2D greenerGrass;
uniform sampler2D greenTexture;
uniform sampler2D grassTexture;
uniform sampler2D concreteTexture;
uniform sampler2D brownTexture;
uniform sampler2D snowTexture;
uniform sampler2D sandTexture;

uniform vec3 cameraPos;

// fenyforras tulajdonsagok
uniform vec4 lightPosFirst = vec4( 0.0, 1.0, 0.0, 0.0);
uniform vec4 lightPosSecond = vec4( 0.0, 1.0, 0.0, 0.0);

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

vec3 GetGradient(float u, float v)
{	
   // elmozdulva a heightmapen a lenti irányokba, megnézzük, hogyan változik a mintavételezett érték
   float heightRight = texture(heightMapTexture, vec2(u + 0.01, v)).r;
   float heightLeft = texture(heightMapTexture, vec2(u - 0.01, v)).r;
   float heightUp = texture(heightMapTexture, vec2(u, v + 0.01)).r;
   float heightDown = texture(heightMapTexture, vec2(u, v - 0.01)).r;
   
   // meghatározzuk az irányvektort matematikailag
   vec3 gradient = vec3((heightRight - heightLeft) / (2.0 * 0.01), 
                       (heightUp - heightDown) / (2.0 * 0.01), 
                       0.0);

   return normalize(gradient);
}

void main()
{
	// A fragment normálvektora
	// MINDIG normalizáljuk!
	vec3 normal = normalize( vs_out_norm );
	// Ambiens komponens
	vec3 Ambient = La * Ka;

	//***************************************************
	//***************************************************
	//*************** MÁSODLAGOS FÉNY *******************
	vec3 ToLightSecond = lightPosSecond.xyz;
	ToLightSecond = normalize(ToLightSecond);
	float AttenuationSecond = 1.0 / lightConstantAttenuation;

	float DiffuseFactorSecond = max(dot(ToLightSecond, normal), 0.0) * AttenuationSecond;
	vec3 DiffuseSecond = DiffuseFactorSecond * Ld * Kd;

	// Spekuláris komponens
	vec3 viewDirSecond = normalize( cameraPos - vs_out_pos ); // A fragmentből a kamerába mutató vektor
	vec3 reflectDirSecond = reflect( -ToLightSecond, normal ); // Tökéletes visszaverődés vektora

	// A spekuláris komponens
	float SpecularFactorSecond = pow(max(dot( viewDirSecond, reflectDirSecond) ,0.0), Shininess) * AttenuationSecond;
	vec3 SpecularSecond = SpecularFactorSecond*Ls*Ks;
	
	//***************************************************
	//***************************************************
	//*************** ALAP FÉNYFORRÁS *******************
	vec3 ToLight; // A fényforrásBA mutató vektor
	float LightDistance=0.0; // A fényforrástól vett távolság
	
	if ( lightPosFirst.w == 0.0 ) // irány fényforrás (directional light)
	{
		ToLight	= lightPosFirst.xyz;
	}
	else				  // pont fényforrás (point light)
	{
		ToLight	= lightPosFirst.xyz - vs_out_pos;
		LightDistance = length(ToLight);
	}
	//  Normalizáljuk a fényforrásba mutató vektort
	ToLight = normalize(ToLight);
	
	// Attenuáció (fényelhalás) kiszámítása
	float Attenuation = 1.0 / ( lightConstantAttenuation + lightLinearAttenuation * LightDistance + lightQuadraticAttenuation * LightDistance * LightDistance);

	// Diffúz komponens
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * Ld * Kd;
	
	// Spekuláris komponens
	vec3 viewDir = normalize( cameraPos - vs_out_pos ); // A fragmentből a kamerába mutató vektor
	vec3 reflectDir = reflect( -ToLight, normal ); // Tökéletes visszaverődés vektora
	
	// A spekuláris komponens
	float SpecularFactor = pow(max(dot( viewDir, reflectDir) ,0.0), Shininess) * Attenuation;
	vec3 Specular = SpecularFactor*Ls*Ks;

	// normal vector debug:
	// fs_out_col = vec4( normal * 0.5 + 0.5, 1.0 );

	// ************** TEXTÚRÁZÁS ******************
	// kiszedjük az értékeket a splatMapTexture-ből
	vec4 splatMapValues = texture(splatMapTexture, vs_out_tex);
	// majd az arányokat figyelembe véve interpoláljuk a finalColor változóba a négy textúrát 
	// mindegy egyes textúrát interpolálunk a feketével a megfelelő splatmap érték alapján,
	// majd a visszakapott vec4-et hozzáadjuk a finalColorhoz
	vec4 finalColor = vec4(0.0);
	finalColor += mix(vec4(0.0), texture(greenerGrass, vs_out_tex),    splatMapValues.r);	
	finalColor += mix(vec4(0.0), texture(greenTexture, vs_out_tex),    splatMapValues.g);
	finalColor += mix(vec4(0.0), texture(grassTexture, vs_out_tex),	   splatMapValues.b);
	finalColor += mix(vec4(0.0), texture(concreteTexture, vs_out_tex), splatMapValues.a);

	// ************** BARNA DOMBORZAT A MEREDEK HELYEKEN ******************
	vec3 gradient = GetGradient(vs_out_tex.x, vs_out_tex.y); // irányvektor
	float steepness = abs(dot(normal, gradient));			 // a normálvektor és az irányvektor skaláris szorzata a meredekség
	vec4 brownColor = texture(brownTexture, vs_out_tex);	 // mintavételezünk a barna textúrából
	float blend = smoothstep(0.9, 1.0, steepness);			 // ha a meredekség 0.7 és 1 között van, akkor ez lesz a blend értéke
	finalColor = mix(finalColor, brownColor, blend);	     // interpoláljuk a finalColorral, a blend 0 lesz, ha 0.95 alatt van

	// ************** HÓ ÉS HOMOK TEXTÚRÁK *********************
	float height = texture(heightMapTexture, vs_out_tex).r;
	vec4 snowColor = texture(snowTexture, vs_out_tex);
	vec4 sandColor = texture(sandTexture, vs_out_tex);
	float snowBlend = smoothstep(0.75, 0.80, height);
	float sandBlend = 1.0 - smoothstep(0.15, 0.2, height); // kivonjuk 1-ből, hogy ha magasabban vagyunk, mint a határ, 0 legyen

	finalColor = mix(finalColor, sandColor, sandBlend);
	finalColor = mix(finalColor, snowColor, snowBlend);
	
	fs_out_col = vec4(Ambient + Diffuse + DiffuseSecond + Specular + SpecularSecond, 1.0 ) * finalColor;
}