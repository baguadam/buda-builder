#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"
#include "ParametricSurfaceMesh.hpp"

#include <imgui.h>
#include <FastNoiseLite.h>
#include "Perlin.hpp"
#include <iostream>
#include "FlatHouse.hpp"

CMyApp::CMyApp()
{
	m_data = new glm::vec3(0.0);
}

CMyApp::~CMyApp()
{
	delete[] m_data;
}

void CMyApp::SetupDebugCallback()
{
	// engedélyezzük és állítsuk be a debug callback függvényt ha debug context-ben vagyunk 
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programID = glCreateProgram();
	AssembleProgram( m_programID, "Vert_PosNormTex.vert", "Frag_Lighting.frag" );

	m_buildingID = glCreateProgram();
	AssembleProgram(m_buildingID, "Vert_Building.vert", "Frag_Building.frag");

	m_FBOID = glCreateProgram();
	AssembleProgram(m_FBOID, "Vert_PosNormTex.vert", "Frag_FBO.frag");
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
	glDeleteProgram(m_buildingID);
	glDeleteProgram(m_FBOID);
}

// Nyers parameterek
struct ParamPlane
{
	glm::vec3 GetPos(float u, float v) const noexcept
	{
		return glm::vec3(u, 0, -v);
	}

	glm::vec3 GetNorm(float u, float v) const noexcept
	{
		return glm::vec3(0.0, 0.0, 1.0);
	}

	glm::vec2 GetTex(float u, float v) const noexcept
	{
		return glm::vec2(u, v);
	}
};

void CMyApp::InitGeometry()
{
	// hegihtmap inicializálása

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof( Vertex, texcoord ), 2, GL_FLOAT },
	};

	MeshObject<Vertex> surfaceMeshCPU = GetParamSurfMesh(ParamPlane(), TABLE_RESOLUTION, TABLE_RESOLUTION);
	m_paramSurfaceGPU = CreateGLObjectFromMesh( surfaceMeshCPU, vertexAttribList );

	// ************************* ÉPÜLETEK **********************************
	const std::initializer_list<VertexAttributeDescriptor> buildingVertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal),   3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};	
	// *********************************************************************
	// ************************* FLATHOUSE *********************************
	MeshObject<Vertex> flatMeshCPU = m_flatHouse.GetMesh();
	m_flatHoustGPU = CreateGLObjectFromMesh(flatMeshCPU, buildingVertexAttribList);

	MeshObject<Vertex> littleHouseCPU = m_littleHouse.GetMesh();
	m_littleHouseGPU = CreateGLObjectFromMesh(littleHouseCPU, buildingVertexAttribList);

	MeshObject<Vertex> familyHouseCPU = m_familyHouse.GetMesh();
	m_familyHouseGPU = CreateGLObjectFromMesh(familyHouseCPU, buildingVertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_paramSurfaceGPU);
	CleanOGLObject(m_flatHoustGPU);
	CleanOGLObject(m_littleHouseGPU);
	CleanOGLObject(m_familyHouseGPU);
}

std::vector<float> CMyApp::GenerateHeightMap() {
	Perlin perlinNoise{};
	std::vector<float> noiseData(TABLE_RESOLUTION * TABLE_RESOLUTION);

	int index = 0;
	const float invResolution = 1.0f / TABLE_RESOLUTION; // hogy ne kelljen minden iterációban osztani a felbontással
	// 0 és 1 közé normalizálás érdekében eltároljuk a maximum és a minimum értékeket
	float minValue = std::numeric_limits<float>::max();
	float maxValue = std::numeric_limits<float>::lowest();

	for (int x = 0; x < TABLE_RESOLUTION; x++) {
		for (int y = 0; y < TABLE_RESOLUTION; y++) {
			float val = 0; // kezdeti érték
			float freq = 1; // frekvencia
			float amp = 1; // amplitúdó

			// mintavételezünk tízszer, majd ezeket mossuk össze, a frekvenciát mindig növeljük, az amplitúdot csökkentjük
			for (int i = 0; i < 10; i++) {
				val += perlinNoise.perlin(x * freq * invResolution, y * freq * invResolution) * amp;

				freq *= 2;
				amp /= 2;
			}

			val = (val + 1) / 2;

			minValue = std::min(minValue, val);
			maxValue = std::max(maxValue, val);

			noiseData[index++] = val;
		}
	}

	// a legenerált noise értékeket 0 és 1 közé széroszlatjuk
	for (float& value : noiseData) {
		value = (value - minValue) / (maxValue - minValue);
	}

	return noiseData;
}


std::vector<glm::vec4> CMyApp::GenerateSplatMap() {

	Perlin perlinNoise{};
	std::vector<glm::vec4> splatMapData(TABLE_RESOLUTION * TABLE_RESOLUTION);
	int index = 0;
	const float invResolution = 1.0f / TABLE_RESOLUTION;

	for (int x = 0; x < TABLE_RESOLUTION; x++) {
		for (int y = 0; y < TABLE_RESOLUTION; y++) {
			// különböző frekvencia a három generált értéknek
			float freq1 = 1;
			float freq2 = 32;
			float freq3 = 64;

			// különböző amplitúdó a három generált értéknek
			float amp1 = 1;
			float amp2 = 0.5;
			float amp3 = 0.25;

			// legeneráljuk őket
			float noiseValue1 = perlinNoise.perlin(x * freq1 * invResolution, y * freq1 * invResolution) * amp1;
			float noiseValue2 = perlinNoise.perlin(x * freq2 * invResolution, y * freq2 * invResolution) * amp2;
			float noiseValue3 = perlinNoise.perlin(x * freq3 * invResolution, y * freq3 * invResolution) * amp3;

			// 0 és 1 közé normáljuk
			noiseValue1 = (noiseValue1 + 1) / 2;
			noiseValue2 = (noiseValue2 + 1) / 2;
			noiseValue3 = (noiseValue3 + 1) / 2;

			// kiszámoljuk a generált értékek összegét, majd a kapott értékell leosztunk mindent, így
			// normálva őket megfelelően, hogy az összegük mindig 1 legyen
			float total = noiseValue1 + noiseValue2 + noiseValue3;
			splatMapData[index++] = { (noiseValue1 / total), (noiseValue2 / total), (noiseValue3 / total), 0 };
		}
	}

	return splatMapData;
}

void CMyApp::InitHeightMap() {
	m_heightMapData = GenerateHeightMap(); // a legenerált a heightmap

	// Egycsatornás textúre létrehozása a heightmap-hez
	glGenTextures(1, &m_heightMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_heightMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TABLE_RESOLUTION, TABLE_RESOLUTION, 0, GL_RED, GL_FLOAT, m_heightMapData.data());
	SetupTextureSampling(GL_TEXTURE_2D, m_heightMapTexture);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CMyApp::InitSplatMap() {
	m_splatMapData = GenerateSplatMap(); // a legenerált a splatmap

	// Négycsatornsá textúre létrehozása a splatmap-hez
	glGenTextures(1, &m_splatMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_splatMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TABLE_RESOLUTION, TABLE_RESOLUTION, 0, GL_RGBA, GL_FLOAT, m_splatMapData.data());
	SetupTextureSampling(GL_TEXTURE_2D, m_splatMapTexture);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CMyApp::InitTextures()
{
	// diffuz texturák fájlból, ez a négy lesz interpolálva
	glGenTextures( 1, &m_greenerGrass );
	TextureFromFile( m_greenerGrass, "Assets/greener_grass.jpg" );
	SetupTextureSampling( GL_TEXTURE_2D, m_greenerGrass );

	glGenTextures(1, &m_grassTexture);
	TextureFromFile(m_grassTexture, "Assets/grass.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_grassTexture);

	glGenTextures(1, &m_greenTexture);
	TextureFromFile(m_greenTexture, "Assets/green.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_greenTexture);

	glGenTextures(1, &m_brownTexture);
	TextureFromFile(m_brownTexture, "Assets/brown.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_brownTexture);

	glGenTextures(1, &m_snowTexture);
	TextureFromFile(m_snowTexture, "Assets/snow.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_snowTexture);

	glGenTextures(1, &m_sandTexture);
	TextureFromFile(m_sandTexture, "Assets/sand.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_sandTexture);

	glGenTextures(1, &m_houseTexture);
	TextureFromFile(m_houseTexture, "Assets/House1_Diffuse.png");
	SetupTextureSampling(GL_TEXTURE_2D, m_houseTexture);

	glGenTextures(1, &m_concreteTexture);
	TextureFromFile(m_concreteTexture, "Assets/concrete.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_concreteTexture);

	// heightmap
	InitHeightMap();

	// splatmap
	InitSplatMap();
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_greenerGrass);
	glDeleteTextures(1, &m_greenTexture);
	glDeleteTextures(1, &m_grassTexture);
	glDeleteTextures(1, &m_heightMapTexture);
	glDeleteTextures(1, &m_splatMapTexture);
	glDeleteTextures(1, &m_brownTexture);
	glDeleteTextures(1, &m_snowTexture);
	glDeleteTextures(1, &m_sandTexture);
	glDeleteTextures(1, &m_houseTexture);
	glDeleteTextures(1, &m_concreteTexture);
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();

	//
	// egyéb inicializálás
	//

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	// kamera
	m_camera.SetView(
		glm::vec3(0.0,   250.0,  0.0),	       // honnan nézzük a színteret	     - eye
		glm::vec3(350.0, 150.0, -350.0),   // a színtér melyik pontját nézzük - at
		// glm::vec3(0.0, 0.0, 5.0),
		// glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.0, 1.0, 0.0));	       // felfelé mutató irány a világban - up

	// FBO - kezdeti
	CreateFrameBuffer(800, 600);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

void CMyApp::Update( const SUpdateInfo& updateInfo )
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	m_camera.Update( updateInfo.DeltaTimeInSec );
	m_lightPos = glm::vec4( m_camera.GetEye(), 1.0 );

	// 0 és 1 közé normáljuk az időt, hogy 360-nal megszorozva kapjunk egy szöget, éppen hol van a nap/hold
	m_time = fmod(m_ElapsedTimeInSec / m_dayDuration, 1.0f);
	float sunAngle = m_time * 360.0f;
	m_sunMoonDirectionalLight = glm::vec4(m_sunRadius * cosf(glm::radians(sunAngle)),
									      300.0f,
									      m_sunRadius * sinf(glm::radians(sunAngle)), 
									      0.0);
	m_lightColor = CalculateLightColor(m_time); // kiszmáoljuk az adott időponthoz tartozó fény színét

	// meghatározzuk a jelenlegi időt órában és percben
	int totalMinutes = static_cast<int>(m_time * 24.0f * 60.0f);
	m_currentHours = totalMinutes / 60;
	m_currentMinutes = totalMinutes % 60;
}

void CMyApp::Render()
{
	/***********************************/
	/***********************************/
	/************  FBO-ba  *************/
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// - VAO beállítása
	glBindVertexArray( m_paramSurfaceGPU.vaoID );

	// - Textúrák beállítása, minden egységre külön
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_heightMapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_splatMapTexture);
	glActiveTexture(GL_TEXTURE2);

	glUseProgram( m_FBOID );

	glUniform1i(ul("heightMapTexture"), 0); // heightmap leküldése a 0-s csatornán
	glUniform1i(ul("splatMapTexture"), 1);  // splatmap leküldése az 1-es csatornán

	glm::mat4 matWorld = glm::scale(TABLE_SCALE);
	glUniformMatrix4fv( ul( "world" ),    1, GL_FALSE, glm::value_ptr( matWorld ) );
	glUniformMatrix4fv( ul( "worldIT" ),  1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( matWorld ) ) ) );
	glUniformMatrix4fv( ul( "viewProj" ), 1, GL_FALSE, glm::value_ptr( m_camera.GetViewProj() ) );

	glDrawElements( GL_TRIANGLES,    
					m_paramSurfaceGPU.count,			 
					GL_UNSIGNED_INT,
					nullptr );

	/***********************************/
	/***********************************/
	/***********************************/
	/***********************************/
	/************  DEFAULT *************/
	// álljunk vissza a default FBO-ra (=frontbuffer)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// - VAO beállítása
	glBindVertexArray(m_paramSurfaceGPU.vaoID);

	// - Textúrák beállítása, minden egységre külön
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_heightMapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_splatMapTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_greenerGrass);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_greenTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_grassTexture);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_concreteTexture);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_brownTexture);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m_sandTexture);

	glUseProgram(m_programID);

	glUniform1i(ul("heightMapTexture"), 0); // heightmap leküldése a 0-s csatornán
	glUniform1i(ul("splatMapTexture"), 1);  // splatmap leküldése az 1-es csatornán
	glUniform1i(ul("greenerGrass"), 2);
	glUniform1i(ul("greenTexture"), 3);
	glUniform1i(ul("grassTexture"), 4);
	glUniform1i(ul("concreteTexture"), 5);
	glUniform1i(ul("brownTexture"), 6);
	glUniform1i(ul("snowTexture"), 7);
	glUniform1i(ul("sandTexture"), 8);

	matWorld = glm::mat4(1.0f) * glm::scale(TABLE_SCALE);
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása
	glUniform3fv(ul("cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosFirst"), 1, glm::value_ptr(m_lightPos));
	glUniform4fv(ul("lightPosSecond"), 1, glm::value_ptr(m_lightPosSecond));
	glUniform3fv(ul("sunMoonLightColor"), 1, glm::value_ptr(m_lightColor));
	glUniform4fv(ul("sunMoonDirection"), 1, glm::value_ptr(m_sunMoonDirectionalLight));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));

	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	glDrawElements(GL_TRIANGLES,
		m_paramSurfaceGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

	/************************************/
	/************************************/
	/************ BUILDINGS *************/
	for (auto pos : m_buildingTypePositionVector) {
		switch (pos.type) {
			case FLAT_HOUSE:
				RenderFlatAndBlockHouse(pos.buildingPosition, FLAT_HOUSE);
				break;
			case BLOCK_HOUSE:
				RenderFlatAndBlockHouse(pos.buildingPosition, BLOCK_HOUSE);
				break;
			case LITTLE_HOUSE:
				RenderLittleHouse(pos.buildingPosition);
				break;
			case FAMILY_HOUSE:
				RenderFamilyHouse(pos.buildingPosition);
				break;
		}
	}

	/***********************************/
	/***********************************/
	/***********************************/
	/***********************************/
	/***********************************/
	/***********************************/
	/*********** TAKARÍTÁS *************/

	// shader kikapcsolasa
	glUseProgram( 0 );

	// - Textúrák kikapcsolása, minden egységre külön
	glActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE1);
	glActiveTexture(GL_TEXTURE2);
	glActiveTexture(GL_TEXTURE3);
	glActiveTexture(GL_TEXTURE4);
	glActiveTexture(GL_TEXTURE5);
	glActiveTexture(GL_TEXTURE6);
	glActiveTexture(GL_TEXTURE7);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, 0);

	// VAO kikapcsolása
	glBindVertexArray( 0 );
}

void CMyApp::RenderFlatAndBlockHouse(glm::vec3 buildingPosition, BuildingType type) {
	glBindVertexArray(m_flatHoustGPU.vaoID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_houseTexture);

	glUseProgram(m_buildingID);

	glUniform1i(ul("texImage"), 0);

	glm::mat4 matWorld;
	if (type == FLAT_HOUSE) {
		matWorld = glm::translate(buildingPosition + glm::vec3(0.0, m_flatHouse.GetFlatRadiusY(), 0.0)) * glm::scale(m_flatHouse.GetFlatScale());
	}
	else {
		matWorld = glm::translate(buildingPosition + glm::vec3(0.0, m_flatHouse.GetBlockRadiusY(), 0.0)) * glm::scale(m_flatHouse.GetBlockScale());
	}

	//glm::mat4 matWorld = glm::translate(buildingPosition * TABLE_SCALE + glm::vec3(0.0, FLAT_BUILDING_RADIUS_Y, 0.0)) * ;
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása
	glUniform3fv(ul("cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosFirst"), 1, glm::value_ptr(m_lightPos));
	glUniform4fv(ul("lightPosSecond"), 1, glm::value_ptr(m_lightPosSecond));
	glUniform3fv(ul("sunMoonLightColor"), 1, glm::value_ptr(m_lightColor));
	glUniform4fv(ul("sunMoonDirection"), 1, glm::value_ptr(m_sunMoonDirectionalLight));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));

	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	glDrawElements(GL_TRIANGLES,
		m_flatHoustGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

	// Textúrák kikapcsolása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// VAO kikapcsolása
	glBindVertexArray(0);

	// Shader kikapcsolása
	glUseProgram(0);
}

void CMyApp::RenderLittleHouse(glm::vec3 buildingPosition) {
	RenderFlatAndBlockHouse(buildingPosition, FLAT_HOUSE); // először kirendereljük a flathouse-t

	// Ezt követően ehhez mérten rendereljük ki a kisház tetejét és skálázzuk azt
	glBindVertexArray(m_littleHouseGPU.vaoID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_houseTexture);

	glUseProgram(m_buildingID);

	glUniform1i(ul("texImage"), 0);

	// megfelelőre méretezzük, majd rátoljuk a kisház tetejére
	glm::mat4 matWorld = glm::translate(buildingPosition + glm::vec3(0.0, m_flatHouse.GetFlatRadiusY() * 2, 0.0)) * glm::scale(m_flatHouse.GetFlatScale());
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása
	glUniform3fv(ul("cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosFirst"), 1, glm::value_ptr(m_lightPos));
	glUniform4fv(ul("lightPosSecond"), 1, glm::value_ptr(m_lightPosSecond));
	glUniform3fv(ul("sunMoonLightColor"), 1, glm::value_ptr(m_lightColor));
	glUniform4fv(ul("sunMoonDirection"), 1, glm::value_ptr(m_sunMoonDirectionalLight));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));

	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	glDrawElements(GL_TRIANGLES,
		m_littleHouseGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

	// Textúrák kikapcsolása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// VAO kikapcsolása
	glBindVertexArray(0);

	// Shader kikapcsolása
	glUseProgram(0);
}

void CMyApp::RenderFamilyHouse(glm::vec3 buildingPosition) {
	// Ezt követően ehhez mérten rendereljük ki a kisház tetejét és skálázzuk azt
	glBindVertexArray(m_familyHouseGPU.vaoID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_houseTexture);

	glUseProgram(m_buildingID);

	glUniform1i(ul("texImage"), 0);

	// megfelelőre méretezzük
	glm::mat4 matWorld = glm::translate(buildingPosition + glm::vec3(0.0, m_familyHouse.GetRadiusY(), 0.0)) * glm::scale(m_familyHouse.GetScale());
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása
	glUniform3fv(ul("cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosFirst"), 1, glm::value_ptr(m_lightPos));
	glUniform4fv(ul("lightPosSecond"), 1, glm::value_ptr(m_lightPosSecond));
	glUniform3fv(ul("sunMoonLightColor"), 1, glm::value_ptr(m_lightColor));
	glUniform4fv(ul("sunMoonDirection"), 1, glm::value_ptr(m_sunMoonDirectionalLight));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));

	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	glDrawElements(GL_TRIANGLES,
		m_familyHouseGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

	// Textúrák kikapcsolása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// VAO kikapcsolása
	glBindVertexArray(0);

	// Shader kikapcsolása
	glUseProgram(0);
}

glm::vec2 CMyApp::GetRadiusPixels(BuildingType type) {
	// meghatározzuk az adott típusú épülethez tartozó X és Z sugarakat
	int radiusX;
	int radiusZ;
	switch (type) {
	case FLAT_HOUSE:
		radiusX = m_flatHouse.GetFlatRadiusX();
		radiusZ = m_flatHouse.GetFlatRadiusZ();
		break;
	case BLOCK_HOUSE:
		radiusX = m_flatHouse.GetBlockRadiusX();
		radiusZ = m_flatHouse.GetBlockRadiusZ();
		break;
	case LITTLE_HOUSE:
		radiusX = m_littleHouse.GetRadiusX();
		radiusZ = m_littleHouse.GetRadiusZ();
		break;
	case FAMILY_HOUSE:
		radiusX = m_familyHouse.GetRadiusX();
		radiusZ = m_familyHouse.GetRadiusZ();
		break;
	}

	// belehelyezzük a sugarakat a megfelelő folbontású rendszerbe
	float intermediateX = static_cast<float>(radiusX) / TABLE_SCALE.x;
	float intermediateZ = static_cast<float>(radiusZ) / TABLE_SCALE.z;
	int radiusPixelsX = static_cast<int>(intermediateX * TABLE_RESOLUTION);
	int radiusPixelsZ = static_cast<int>(intermediateZ * TABLE_RESOLUTION);

	return glm::vec2(radiusPixelsX, radiusPixelsZ);
}

void CMyApp::FlattenTerrainUnderBuilding(glm::vec2 uv, BuildingType type) {
	/******************************************************/
	/**************** KISIMÍTJUK A TALAJT *****************/
	// meghatározzuk az adott U, V, illetve az épület sugarának pixelkoordinátáit
	// u és v esetén megszorozzuk csak az adott koordinátákat a felbontással
	int uCoord = static_cast<int>(uv.x * TABLE_RESOLUTION);
	int vCoord = static_cast<int>(uv.y * TABLE_RESOLUTION);
	// épület "sugara" esetén előbb le kell osztani az terrain scale-jével
	glm::vec2 radiusPixelCoordinates = GetRadiusPixels(type);
	int radiusPixelsX = radiusPixelCoordinates.x;
	int radiusPixelsZ = radiusPixelCoordinates.y;

	// kiolvassuk az m_heightMapData-ból az adott koordinátákohoz tartozó értéket
	float totalHeight = 0.0f;
	int count = 0;

	// az adott sugár mentén kiolvassuk az értéket a m_heightMapData tömbből
	for (int i = -radiusPixelsX - 2; i <= radiusPixelsX + 2; ++i) {
		for (int j = -radiusPixelsZ - 2; j <= radiusPixelsZ + 2; ++j) {
			// végigmenyünk a teljes sugár mentén
			int x = uCoord + i;
			int y = vCoord + j;

			// biztonsági ellenőrzés
			if (x >= 0 && x < TABLE_RESOLUTION && y >= 0 && y < TABLE_RESOLUTION) {
				totalHeight += m_heightMapData[y * TABLE_RESOLUTION + x];
				count++;
			}
		}
	}

	// ha volt count, akkor kiszámoljuk az átlagot magasságot, majd újra végigmegyünk a textúrán, 
	// módosítjuk a megfelelő magasságértékeket az átlagra
	if (count > 0) {
		float averageHeight = totalHeight / static_cast<float>(count);

		// ismét végigmegyünk a heightmap tömbön, módosítunk minden értéket az átlagoltra
		for (int i = -radiusPixelsX - 2; i <= radiusPixelsX + 2; ++i) {
			for (int j = -radiusPixelsZ - 2; j <= radiusPixelsZ + 2; ++j) {
				// végigmegyünk a teljes sugár mentén
				int x = uCoord + i;
				int y = vCoord + j;

				// biztonsági ellenőrzés
				if (x >= 0 && x < TABLE_RESOLUTION && y >= 0 && y < TABLE_RESOLUTION) {
					m_heightMapData[y * TABLE_RESOLUTION + x] = averageHeight;
				}
			}
		}

		// módosítjuk a textúrát, ami majd a renderben le fog küldődni megfelelően
		glGenTextures(1, &m_heightMapTexture);
		glBindTexture(GL_TEXTURE_2D, m_heightMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TABLE_RESOLUTION, TABLE_RESOLUTION, 0, GL_RED, GL_FLOAT, m_heightMapData.data());
		SetupTextureSampling(GL_TEXTURE_2D, m_heightMapTexture);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CMyApp::PlaceConcreteUnderBuilding(glm::vec2 uv, BuildingType type) {
	/******************************************************/
	/**************** BETONT HELYEZÜNK LE *****************/
	// meghatározzuk az adott U, V, illetve az épület sugarának pixelkoordinátáit
	// u és v esetén megszorozzuk csak az adott koordinátákat a felbontással
	int uCoord = static_cast<int>(uv.x * TABLE_RESOLUTION);
	int vCoord = static_cast<int>(uv.y * TABLE_RESOLUTION);
	// épület "sugara" esetén előbb le kell osztani az terrain scale-jével
	glm::vec2 radiusPixelCoordinates = GetRadiusPixels(type);
	int radiusPixelsX = radiusPixelCoordinates.x;
	int radiusPixelsZ = radiusPixelCoordinates.y;

	// ismét végigmegyünk a splatmapen, kettő sugarú környezetben, majd módosítjuk a megfelelő értékeket
	for (int i = - radiusPixelsX - 2; i <= radiusPixelsX + 2; ++i) {
		for (int j = - radiusPixelsZ - 2; j <= radiusPixelsZ + 2; ++j) {
			// végigmegyünk a teljes sugár mentén
			int x = uCoord + i;
			int y = vCoord + j;

			// biztonsági ellenőrzés, majd kinullázzuk az első három csatornát, a negyediket egyre állítjuk
			if (x >= 0 && x < TABLE_RESOLUTION && y >= 0 && y < TABLE_RESOLUTION) {
				m_splatMapData[y * TABLE_RESOLUTION + x] = { 0.0, 0.0, 0.0, 1.0, };
			}
		}
	}

	// módosítjuk a textúrát, ami majd a renderben le fog küldődni megfelelően
	glBindTexture(GL_TEXTURE_2D, m_splatMapTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TABLE_RESOLUTION, TABLE_RESOLUTION, GL_RGBA, GL_FLOAT, m_splatMapData.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CMyApp::RenderGUI()
{
	// ImGui::ShowDemoWindow();
	if ( ImGui::Begin( "Lighting settings" ) )
	{
		ImGui::SeparatorText("Settings");
		
		//ImGui::SliderFloat("Shininess", &m_Shininess, 0.0001f, 10.0f );
		ImGui::InputFloat("Shininess", &m_Shininess, 0.1f, 1.0f, "%.1f" );
		static float Kaf = 1.0f;
		static float Kdf = 1.0f;
		static float Ksf = 1.0f;
		if ( ImGui::SliderFloat( "Ka", &Kaf, 0.0f, 1.0f ) )
		{
			m_Ka = glm::vec3( Kaf );
		}
		if ( ImGui::SliderFloat( "Kd", &Kdf, 0.0f, 1.0f ) )
		{
			m_Kd = glm::vec3( Kdf );
		}
		if ( ImGui::SliderFloat( "Ks", &Ksf, 0.0f, 1.0f ) )
		{
			m_Ks = glm::vec3( Ksf );
		}

		ImGui::SliderFloat( "Constant Attenuation", &m_lightConstantAttenuation, 0.001f, 2.0f );
		ImGui::SliderFloat( "Linear Attenuation", &m_lightLinearAttenuation, 0.001f, 2.0f );
		ImGui::SliderFloat( "Quadratic Attenuation", &m_lightQuadraticAttenuation, 0.001f, 2.0f );	
	}
	ImGui::End();

	if (ImGui::Begin("Building chooser")) {
		ImGui::SeparatorText("Buildings");

		// A gombokra kattintva beállítjuk a megfelelő épülettípust
		if (ImGui::Button("Flat House")) {
			selectedBuilding = FLAT_HOUSE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Family House")) {
			selectedBuilding = FAMILY_HOUSE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Little House")) {
			selectedBuilding = LITTLE_HOUSE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Block House")) {
			selectedBuilding = BLOCK_HOUSE;
		}

		// A választott épület alapján kiíratjuk a megfelelőt
		const char* buildingTypeText = "Unknown";
		switch (selectedBuilding) {
		case FLAT_HOUSE:
			buildingTypeText = "Flat House";
			break;
		case FAMILY_HOUSE:
			buildingTypeText = "Family House";
			break;
		case LITTLE_HOUSE:
			buildingTypeText = "Little House";
			break;
		case BLOCK_HOUSE:
			buildingTypeText = "Block House";
			break;
		}
		ImGui::Text("Selected Building Type: %s", buildingTypeText);
	}
	ImGui::End();

	if (ImGui::Begin("Time")) {
		ImGui::SeparatorText("Time of the day");
		ImGui::Text("Current Time: %02d:%02d", m_currentHours, m_currentMinutes);
	}
	ImGui::End();
}

glm::vec3 CMyApp::GetBuildingDimensions(BuildingType type) {
	switch (type) {
	case FLAT_HOUSE:
		return glm::vec3(m_flatHouse.GetFlatRadiusX(), m_flatHouse.GetFlatRadiusY(), m_flatHouse.GetFlatRadiusZ());
	case BLOCK_HOUSE:
		return glm::vec3(m_flatHouse.GetBlockRadiusX(), m_flatHouse.GetBlockRadiusY(), m_flatHouse.GetBlockRadiusZ());
	case LITTLE_HOUSE:
		return glm::vec3(m_littleHouse.GetRadiusX(), m_flatHouse.GetFlatRadiusY() + m_littleHouse.GetRadiusY() * 2, m_littleHouse.GetRadiusZ());
	case FAMILY_HOUSE:
		return glm::vec3(m_familyHouse.GetRadiusX(), m_familyHouse.GetRadiusY(), m_familyHouse.GetRadiusZ());
	}
}

bool CMyApp::CheckBuildingCollisions(glm::vec3 newPosition, BuildingType type) {
	// meghatározzuk az újonnan létrehozandó épület AABB-át
	glm::vec3 newBuildingMin = newPosition - GetBuildingDimensions(type);
	glm::vec3 newBuildingMax = newPosition + GetBuildingDimensions(type);

	// végigiterálunk a már eltárolt épületek listáján és megnézzük, hogy van-e bármelyikkel ütközés
	for (const auto& building : m_buildingTypePositionVector) {
		// meghatározzuk a már létező épület esetén is az AABB-t
		glm::vec3 existingBuildingMin = building.buildingPosition - GetBuildingDimensions(building.type);
		glm::vec3 existingBuildingMax = building.buildingPosition + GetBuildingDimensions(building.type);

		// AABB algoritmus, megnézzük, hogy van-e ütközés, ha igen, visszatérünk true-val
		if (newBuildingMax.x > existingBuildingMin.x && newBuildingMin.x < existingBuildingMax.x &&
			newBuildingMax.y > existingBuildingMin.y && newBuildingMin.y < existingBuildingMax.y &&
			newBuildingMax.z > existingBuildingMin.z && newBuildingMin.z < existingBuildingMax.z) {
			return true;
		}
	}

	// az egész végén, amennyiben nem volt ütközés, akkor visszatérünk false-szal
	return false;
}

glm::vec3 CMyApp::InterpolateColor(glm::vec3 color1, glm::vec3 color2, float t) {
	// lineáris interpoláció
	return (1.0f - t) * color1 + t * color2;
}

glm::vec3 CMyApp::CalculateLightColor(float timeOfDay) {
	// a megfelelő színek kódjai
	glm::vec3 paleWhite = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 deepBlue  = glm::vec3(0.0f, 0.0f, 0.5f);
	glm::vec3 yellow    = glm::vec3(1.0f, 1.0f, 0.0f);
	glm::vec3 white     = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 orange    = glm::vec3(1.0f, 0.5f, 0.0f);

	// interpoláljuk a színeket a napszaktól függően, hogy folyamatos átmenet legyen közöttük
	if (timeOfDay >= 0.0f && timeOfDay < 0.1f)
		return InterpolateColor(paleWhite, deepBlue, timeOfDay / 0.1f);
	else if (timeOfDay >= 0.1f && timeOfDay < 0.3f)
		return InterpolateColor(deepBlue, yellow, (timeOfDay - 0.1f) / 0.2f);
	else if (timeOfDay >= 0.3f && timeOfDay < 0.5f)
		return InterpolateColor(yellow, white, (timeOfDay - 0.3f) / 0.2f);
	else if (timeOfDay >= 0.5f && timeOfDay < 0.7f)
		return InterpolateColor(white, orange, (timeOfDay - 0.5f) / 0.2f);
	else if (timeOfDay >= 0.7f && timeOfDay < 0.9f)
		return InterpolateColor(orange, deepBlue, (timeOfDay - 0.7f) / 0.2f);
	else
		return InterpolateColor(deepBlue, paleWhite, (timeOfDay - 0.9f) / 0.1f);
}

GLint CMyApp::ul( const char* uniformName ) noexcept
{
	GLuint programID = 0;

	// Kérdezzük le az aktuális programot!
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
	glGetIntegerv( GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>( &programID ) );
	// A program és a uniform név ismeretében kérdezzük le a location-t!
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml
	return glGetUniformLocation( programID, uniformName );
}

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{	
	if ( key.repeat == 0 ) // Először lett megnyomva
	{
		if ( key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL )
		{
			CleanShaders();
			InitShaders();
		}
		if ( key.keysym.sym == SDLK_F1 )
		{
			GLint polygonModeFrontAndBack[ 2 ] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv( GL_POLYGON_MODE, polygonModeFrontAndBack ); // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat.
			GLenum polygonMode = ( polygonModeFrontAndBack[ 0 ] != GL_FILL ? GL_FILL : GL_LINE ); // Váltogassuk FILL és LINE között!
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode( GL_FRONT_AND_BACK, polygonMode ); // Állítsuk be az újat!
		}
	}
	m_camera.KeyboardDown( key );
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp( key );
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove( mouse );
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
	int x, y;
	SDL_GetMouseState(&x, &y); // kattintás koordinátáinak meghatározása
	std::cout << x << " " << y << "\n";

	// kiolvassuk a framebufferből az adott pixelhez tartozó UV értékeket
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glReadPixels(x, height - y, 1, 1, GL_RGB, GL_FLOAT, (void*)m_data);

	if ((*m_data).z == 1) {
		float u = (*m_data).x;
		float v = (*m_data).y;
		glm::vec3 pos = ParamPlane().GetPos(u, v); // meghatározzuk a pozíciót

		// meghatározzuk a jelenlegi magasság értéket, hogy ennek segítségével detektáljuk az ütközést, ha van
		int uCoord = static_cast<int>(u * TABLE_RESOLUTION);
		int vCoord = static_cast<int>(v * TABLE_RESOLUTION);
		float height = m_heightMapData[vCoord * TABLE_RESOLUTION + uCoord] * SCALE_VALUE;

		if (!CheckBuildingCollisions(glm::vec3(pos.x, height, pos.z) * TABLE_SCALE, selectedBuilding)) {
			FlattenTerrainUnderBuilding(glm::vec2(u, v), selectedBuilding); // ha tudunk lehelyezni épületet, kisimítjuk alatta a talajt
			PlaceConcreteUnderBuilding(glm::vec2(u, v), selectedBuilding);  // betont helyezünk le az épület alá
			// miután módosítottuk a magasságértéket, kiolvassuk a már módosítottat, hogy jó magasságra helyezzük le az épületeket
			height = m_heightMapData[vCoord * TABLE_RESOLUTION + uCoord] * SCALE_VALUE;

			StoredBuilding current{ glm::vec3(pos.x, height, pos.z) * TABLE_SCALE, selectedBuilding }; // eltároljuk a létrehozott épület típusát, illetve a lehelyezés pozícióit
			m_buildingTypePositionVector.push_back(current);							 // hozzáadjuk a tárolt épület pozícióhoz az újonnan rajzolandó épület koordinátáit
		}
	}
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_camera.MouseWheel( wheel );
}

// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	width = _w;
	height = _h;
	glViewport(0, 0, _w, _h);
	m_camera.Resize( _w, _h );
	CreateFrameBuffer(_w, _h);
}

void CMyApp::CreateFrameBuffer(int width, int height)
{
	// takarítsunk, ha nem először hívják ezt a függvényt
	if (m_frameBufferCreated)
	{
		glDeleteRenderbuffers(1, &m_depthBuffer);
		glDeleteTextures(1, &m_colorBuffer);
		glDeleteFramebuffers(1, &m_frameBuffer);
	}

	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_colorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Error creating color attachment" << std::endl;
		exit(1);
	}

	glGenRenderbuffers(1, &m_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Error creating depth attachment" << std::endl;
		exit(1);
	}

	// -- Completeness check
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Incomplete framebuffer (";
		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cout << "GL_FRAMEBUFFER_UNSUPPORTED";
			break;
		}
		std::cout << ")" << std::endl;
		char ch;
		std::cin >> ch;
		exit(1);
	}

	// -- Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_frameBufferCreated = true;
}

