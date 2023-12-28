#version 430

// pipeline-b�l bej�v� per-fragment attrib�tumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimen� �rt�k - a fragment sz�ne
out vec2 fs_out_col;

// text�ra mintav�telez� objektum
uniform sampler2D heightMapTexture;
uniform sampler2D splatMapTexture;
uniform sampler2D greenerGrass;
uniform sampler2D greenTexture;
uniform sampler2D grassTexture;
uniform sampler2D seamlessGrass;
uniform sampler2D brownTexture;
uniform sampler2D snowTexture;
uniform sampler2D sandTexture;

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
   // elmozdulva a heightmapen a lenti ir�nyokba, megn�zz�k, hogyan v�ltozik a mintav�telezett �rt�k
   float heightRight = texture(heightMapTexture,  vec2(u + 0.01, v)).r;
   float heightLeft  = texture(heightMapTexture,  vec2(u - 0.01, v)).r;
   float heightUp    = texture(heightMapTexture,  vec2(u, v + 0.01)).r;
   float heightDown  = texture(heightMapTexture,  vec2(u, v - 0.01)).r;
   
   // meghat�rozzuk az ir�nyvektort matematikailag
   vec3 gradient = vec3((heightRight - heightLeft) / (2.0 * 0.01), 
                       (heightUp - heightDown) / (2.0 * 0.01), 
                       0.0);

   return normalize(gradient);
}

void main()
{
	// A fragment norm�lvektora
	// MINDIG normaliz�ljuk!
	vec3 normal = normalize( vs_out_norm );
	
	vec3 ToLight; // A f�nyforr�sBA mutat� vektor
	float LightDistance=0.0; // A f�nyforr�st�l vett t�vols�g
	
	if ( lightPos.w == 0.0 ) // ir�ny f�nyforr�s (directional light)
	{
		// Ir�nyf�nyforr�s eset�n minden pont ugyan abb�la az ir�nyb�l van megvil�g�tva
		ToLight	= lightPos.xyz;
		// A t�vols�got 0-n hagyjuk, hogy az attenu�ci� ne v�ltoztassa a f�nyt
	}
	else				  // pont f�nyforr�s (point light)
	{
		// Pontf�nyforr�s eset�n kkisz�moljuk a fragment pontb�l a f�nyforr�sba mutat� vektort, ...
		ToLight	= lightPos.xyz - vs_out_pos;
		// ... �s a t�vols�got a f�nyforr�st�l
		LightDistance = length(ToLight);
	}
	//  Normaliz�ljuk a f�nyforr�sba mutat� vektort
	ToLight = normalize(ToLight);
	
	// Attenu�ci� (f�nyelhal�s) kisz�m�t�sa
	float Attenuation = 1.0 / ( lightConstantAttenuation + lightLinearAttenuation * LightDistance + lightQuadraticAttenuation * LightDistance * LightDistance);
	
	// Ambiens komponens
	// Ambiens f�ny mindenhol ugyanakkora
	vec3 Ambient = La * Ka;

	// Diff�z komponens
	// A diff�z f�nyforr�sb�l �rkez� f�ny mennyis�ge ar�nyos a f�nyforr�sba mutat� vektor �s a norm�lvektor skal�ris szorzat�val
	// �s az attenu�ci�val
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * Ld * Kd;
	
	// Spekul�ris komponens
	vec3 viewDir = normalize( cameraPos - vs_out_pos ); // A fragmentb�l a kamer�ba mutat� vektor
	vec3 reflectDir = reflect( -ToLight, normal ); // T�k�letes visszaver�d�s vektora
	
	// A spekul�ris komponens a t�k�letes visszaver�d�s ir�nya �s a kamera ir�ny�t�l f�gg.
	// A koncentr�lts�ga cos()^s alakban sz�moljuk, ahol s a f�nyess�get meghat�roz� param�ter.
	// Szint�n f�gg az attenu�ci�t�l.
	float SpecularFactor = pow(max(dot( viewDir, reflectDir) ,0.0), Shininess) * Attenuation;
	vec3 Specular = SpecularFactor*Ls*Ks;

	// normal vector debug:
	// fs_out_col = vec4( normal * 0.5 + 0.5, 1.0 );

	// ************** TEXT�R�Z�S ******************
	// kiszedj�k az �rt�keket a splatMapTexture-b�l
	vec4 splatMapValues = texture(splatMapTexture, vs_out_tex);
	// majd az ar�nyokat figyelembe v�ve interpol�ljuk a finalColor v�ltoz�ba a n�gy text�r�t 
	// mindegy egyes text�r�t interpol�lunk a feket�vel a megfelel� splatmap �rt�k alapj�n,
	// majd a visszakapott vec4-et hozz�adjuk a finalColorhoz
	vec4 finalColor = vec4(0.0);
	finalColor += mix(vec4(0.0), texture(greenerGrass, vs_out_tex), splatMapValues.r);	
	finalColor += mix(vec4(0.0), texture(greenTexture, vs_out_tex), splatMapValues.g);
	finalColor += mix(vec4(0.0), texture(grassTexture, vs_out_tex), splatMapValues.b);
	finalColor += mix(vec4(0.0), texture(seamlessGrass, vs_out_tex), splatMapValues.a);

	// ************** BARNA DOMBORZAT A MEREDEK HELYEKEN ******************
	vec3 gradient = GetGradient(vs_out_tex.x, vs_out_tex.y); // ir�nyvektor
	float steepness = abs(dot(normal, gradient));			 // a norm�lvektor �s az ir�nyvektor skal�ris szorzata a meredeks�g
	vec4 brownColor = texture(brownTexture, vs_out_tex);	 // mintav�telez�nk a barna text�r�b�l
	float blend = smoothstep(0.9, 1.0, steepness);			 // ha a meredeks�g 0.7 �s 1 k�z�tt van, akkor ez lesz a blend �rt�ke
	finalColor = mix(finalColor, brownColor, blend);	     // interpol�ljuk a finalColorral, a blend 0 lesz, ha 0.95 alatt van

	// ************** H� �S HOMOK TEXT�R�K *********************
	float height = texture(heightMapTexture, vs_out_tex).r;
	vec4  snowColor = texture(snowTexture, vs_out_tex);
	vec4  sandColor = texture(sandTexture, vs_out_tex);
	float snowBlend = smoothstep(0.75, 0.80, height);
	float sandBlend = 1.0 - smoothstep(0.15, 0.2, height); // kivonjuk 1-b�l, hogy ha magasabban vagyunk, mint a hat�r, 0 legyen

	finalColor = mix(finalColor, sandColor, sandBlend);
	finalColor = mix(finalColor, snowColor, snowBlend);
	
	fs_out_col = vec2(vs_out_tex.x, vs_out_tex.y); 
}