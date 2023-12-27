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
uniform sampler2D seamlessGrass;
uniform sampler2D brownTexture;

uniform vec3 cameraPos;

// fenyforras tulajdonsagok
uniform vec4 lightPos = vec4( 0.0, 1.0, 0.0, 0.0);

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
   // mintacételezünk az adott pontban a heightmapból
   float height = texture(heightMapTexture, vec2(u, v)).r;
   
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
	
	vec3 ToLight; // A fényforrásBA mutató vektor
	float LightDistance=0.0; // A fényforrástól vett távolság
	
	if ( lightPos.w == 0.0 ) // irány fényforrás (directional light)
	{
		// Irányfényforrás esetén minden pont ugyan abbóla az irányból van megvilágítva
		ToLight	= lightPos.xyz;
		// A távolságot 0-n hagyjuk, hogy az attenuáció ne változtassa a fényt
	}
	else				  // pont fényforrás (point light)
	{
		// Pontfényforrás esetén kkiszámoljuk a fragment pontból a fényforrásba mutató vektort, ...
		ToLight	= lightPos.xyz - vs_out_pos;
		// ... és a távolságot a fényforrástól
		LightDistance = length(ToLight);
	}
	//  Normalizáljuk a fényforrásba mutató vektort
	ToLight = normalize(ToLight);
	
	// Attenuáció (fényelhalás) kiszámítása
	float Attenuation = 1.0 / ( lightConstantAttenuation + lightLinearAttenuation * LightDistance + lightQuadraticAttenuation * LightDistance * LightDistance);
	
	// Ambiens komponens
	// Ambiens fény mindenhol ugyanakkora
	vec3 Ambient = La * Ka;

	// Diffúz komponens
	// A diffúz fényforrásból érkező fény mennyisége arányos a fényforrásba mutató vektor és a normálvektor skaláris szorzatával
	// és az attenuációval
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * Ld * Kd;
	
	// Spekuláris komponens
	vec3 viewDir = normalize( cameraPos - vs_out_pos ); // A fragmentből a kamerába mutató vektor
	vec3 reflectDir = reflect( -ToLight, normal ); // Tökéletes visszaverődés vektora
	
	// A spekuláris komponens a tökéletes visszaverődés iránya és a kamera irányától függ.
	// A koncentráltsága cos()^s alakban számoljuk, ahol s a fényességet meghatározó paraméter.
	// Szintén függ az attenuációtól.
	float SpecularFactor = pow(max(dot( viewDir, reflectDir) ,0.0), Shininess) * Attenuation;
	vec3 Specular = SpecularFactor*Ls*Ks;

	// normal vector debug:
	// fs_out_col = vec4( normal * 0.5 + 0.5, 1.0 );

	// kiszedjük az értékeket a splatMapTexture-ből
	vec4 splatMapValues = texture(splatMapTexture, vs_out_tex);
	// majd az arányokat figyelembe véve interpoláljuk a finalColor változóba a négy textúrát 
	// mindegy egyes textúrát interpolálunk a feketével a megfelelő splatmap érték alapján,
	// majd a visszakapott vec4-et hozzáadjuk a finalColorhoz
	vec4 finalColor = vec4(0.0);
	finalColor += mix(vec4(0.0), texture(greenerGrass, vs_out_tex), splatMapValues.r);	
	finalColor += mix(vec4(0.0), texture(greenTexture, vs_out_tex), splatMapValues.g);
	finalColor += mix(vec4(0.0), texture(grassTexture, vs_out_tex), splatMapValues.b);
	finalColor += mix(vec4(0.0), texture(seamlessGrass, vs_out_tex), splatMapValues.a);

	vec3 gradient = GetGradient(vs_out_tex.x, vs_out_tex.y); // irányvektor
	float steepness = abs(dot(normal, gradient));			 // a normálvektor és az irányvektor skaláris szorzata a meredekség
	float blend = smoothstep(0.80, 1.0, steepness);			 // ha a meredekség 0.7 és 1 között van, akkor ez lesz a blend értéke
	vec4 brownColor = texture(brownTexture, vs_out_tex);	 // mintavételezünk a barna textúrából
	finalColor = mix(finalColor, brownColor, blend);	     // interpoláljuk a finalColorral, a blend 0 lesz, ha 0.95 alatt van

	fs_out_col = vec4( Ambient+Diffuse+Specular, 1.0 ) * finalColor;
}