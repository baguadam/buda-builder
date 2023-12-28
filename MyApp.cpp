#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"
#include "ParametricSurfaceMesh.hpp"

#include <imgui.h>
#include <FastNoiseLite.h>
#include <iostream>

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
}

// Nyers parameterek
struct ParamPlane
{
	glm::vec3 GetPos(float u, float v) const noexcept
	{
		return glm::vec3(u, v, 0.0);
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

	// ************************* FLATHOUSE *******************************************
	const std::initializer_list<VertexAttributeDescriptor> buildingVertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal),   3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};

	MeshObject<Vertex> flatMeshCPU;

	flatMeshCPU.vertexArray =
	{
		// SZEMKÖZTI OLDAL
		{ glm::vec3(-1, -1, 0),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3( 1, -1, 0),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(-1,  1, 0),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3( 1,  1, 0),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 1.0) },

		// TETŐ
		{ glm::vec3(-1,  1,   0),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3( 1,  1,   0),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3(-1,  1,  -2),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.5, 1.0) },
		{ glm::vec3( 1,  1,  -2),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0) },

		// JOBB OLDAL
		{ glm::vec3(1, -1,   0),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(1, -1,  -2),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(1,  1,   0),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(1,  1,  -2),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.5, 1.0) },

		// HÁTLAP
		{ glm::vec3( 1, -1,  -2),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(-1, -1,  -2),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3( 1,  1,  -2),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(-1,  1,  -2),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.5, 1.0) },

		// BAL OLDAL
		{ glm::vec3(-1, -1,  -2),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(-1, -1,   0),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(-1,  1,  -2),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(-1,  1,   0),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.5, 1.0) },
	};

	flatMeshCPU.indexArray =
	{
		// SZEMKÖZTI OLDAL
		0, 1, 2,
		1, 3, 2,

		// TETŐ
		16, 17, 18,
		17, 19, 18,

		// JOBB OLDAL
		4, 5, 6,
		5, 7, 6,

		// HÁTLAP
		8, 9,  10,
		9, 11, 10,

		// BAL OLDAL
		12, 13, 14,
		13, 15, 14
	};

	m_flatHoustGPU = CreateGLObjectFromMesh(flatMeshCPU, buildingVertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_paramSurfaceGPU);
	CleanOGLObject(m_flatHoustGPU);
}

std::vector<float> CMyApp::GenerateHeightMap() {
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	std::vector<float> noiseData(TABLE_RESOLUTION * TABLE_RESOLUTION);
	int index = 0;
	for (int y = 0; y < TABLE_RESOLUTION; y++)
	{
		for (int x = 0; x < TABLE_RESOLUTION; x++)
		{
			float noiseValue = noise.GetNoise((float)x, (float)y);
			noiseData[index++] = (noiseValue + 1) / 2; // hogy a generált értékek 0 és 1 között legyenek
		}
	}

	return noiseData;
}

std::vector<glm::vec4> CMyApp::GenerateSplatMap() {
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	std::vector<glm::vec4> splatMapData(TABLE_RESOLUTION * TABLE_RESOLUTION);
	int index = 0;
	for (int y = 0; y < TABLE_RESOLUTION; y++)
	{
		for (int x = 0; x < TABLE_RESOLUTION; x++)
		{
			float noiseValue1 = noise.GetNoise((float)x, (float)y);
			float noiseValue2 = noise.GetNoise((float)(x + 1000), (float)(y + 1000));
			float noiseValue3 = noise.GetNoise((float)(x + 2000), (float)(y + 2000));
			float noiseValue4 = noise.GetNoise((float)(x + 3000), (float)(y + 3000));

			// kiszámoljuk a generált értékek összegét, majd a kapott értékell leosztunk mindent, így
			// normálva őket megfelelően, hogy az összegük mindig 1 legyen
			float total = noiseValue1 + noiseValue2 + noiseValue3 + noiseValue4;
			splatMapData[index++] = { (noiseValue1 / total), (noiseValue2 / total), (noiseValue3 / total), (noiseValue4 / total) };
		}
	}
	return splatMapData;
}

void CMyApp::InitHeightMap() {
	std::vector<float> noiseData = GenerateHeightMap(); // a legenerált a heightmap

	// Egycsatornás textúre létrehozása a heightmap-hez
	glGenTextures(1, &m_heightMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_heightMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TABLE_RESOLUTION, TABLE_RESOLUTION, 0, GL_RED, GL_FLOAT, noiseData.data());
	SetupTextureSampling(GL_TEXTURE_2D, m_heightMapTexture);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CMyApp::InitSplatMap() {
	std::vector<glm::vec4> splatMapData = GenerateSplatMap(); // a legenerált a splatmap

	// Négycsatornsá textúre létrehozása a splatmap-hez
	glGenTextures(1, &m_splatMapTexture);
	glBindTexture(GL_TEXTURE_2D, m_splatMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TABLE_RESOLUTION, TABLE_RESOLUTION, 0, GL_RGBA, GL_FLOAT, splatMapData.data());
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

	glGenTextures(1, &m_seamlessGrassTexture);
	TextureFromFile(m_seamlessGrassTexture, "Assets/seamless_grass.jpg");
	SetupTextureSampling(GL_TEXTURE_2D, m_seamlessGrassTexture);

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
	glDeleteTextures(1, &m_seamlessGrassTexture);
	glDeleteTextures(1, &m_heightMapTexture);
	glDeleteTextures(1, &m_splatMapTexture);
	glDeleteTextures(1, &m_brownTexture);
	glDeleteTextures(1, &m_snowTexture);
	glDeleteTextures(1, &m_sandTexture);
	glDeleteTextures(1, &m_houseTexture);
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
		glm::vec3(0.0, 300.0, 0.0),	  // honnan nézzük a színteret	     - eye
		glm::vec3(0.0, 280.0, 0.0),   // a színtér melyik pontját nézzük - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban - up

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
	glBindTexture(GL_TEXTURE_2D, m_greenerGrass);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_greenTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_grassTexture);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_seamlessGrassTexture);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_brownTexture);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, m_sandTexture);

	glUseProgram( m_FBOID );

	glUniform1i(ul("heightMapTexture"), 0); // heightmap leküldése a 0-s csatornán
	glUniform1i(ul("splatMapTexture"), 1);  // splatmap leküldése az 1-es csatornán
	glUniform1i(ul("greenerGrass"), 2);  
	glUniform1i(ul("greenTexture"), 3);  
	glUniform1i(ul("grassTexture"), 4);  
	glUniform1i(ul("seamlessGrass"), 5);
	glUniform1i(ul("brownTexture"), 6);
	glUniform1i(ul("snowTexture"), 7);
	glUniform1i(ul("sandTexture"), 8);

	glm::mat4 matWorld = glm::mat4(1.0f) * glm::scale(TABLE_SCALE);
	glUniformMatrix4fv( ul( "world" ),    1, GL_FALSE, glm::value_ptr( matWorld ) );
	glUniformMatrix4fv( ul( "worldIT" ),  1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( matWorld ) ) ) );
	glUniformMatrix4fv( ul( "viewProj" ), 1, GL_FALSE, glm::value_ptr( m_camera.GetViewProj() ) );

	// - Fényforrások beállítása
	glUniform3fv( ul( "cameraPos" ), 1, glm::value_ptr( m_camera.GetEye() ) );
	glUniform4fv( ul( "lightPos" ),  1, glm::value_ptr( m_lightPos ) );

	glUniform3fv( ul( "La" ),		 1, glm::value_ptr( m_La ) );
	glUniform3fv( ul( "Ld" ),		 1, glm::value_ptr( m_Ld ) );
	glUniform3fv( ul( "Ls" ),		 1, glm::value_ptr( m_Ls ) );

	glUniform1f( ul( "lightConstantAttenuation"	 ), m_lightConstantAttenuation );
	glUniform1f( ul( "lightLinearAttenuation"	 ), m_lightLinearAttenuation   );
	glUniform1f( ul( "lightQuadraticAttenuation" ), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glUniform3fv( ul( "Ka" ),		 1, glm::value_ptr( m_Ka ) );
	glUniform3fv( ul( "Kd" ),		 1, glm::value_ptr( m_Kd ) );
	glUniform3fv( ul( "Ks" ),		 1, glm::value_ptr( m_Ks ) );

	glUniform1f( ul( "Shininess" ),	m_Shininess );

	glDrawElements( GL_TRIANGLES,    
					m_paramSurfaceGPU.count,			 
					GL_UNSIGNED_INT,
					nullptr );

	/***********************************/
	/***********************************/
	/************ BUILDING *************/

	glBindVertexArray(m_flatHoustGPU.vaoID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_houseTexture);
	
	glUseProgram(m_buildingID);
	
	glUniform1i(ul("texImage"), 0);

	matWorld = glm::mat4(1.0f) * glm::scale(BUILDING_SCALE);
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glDrawElements(GL_TRIANGLES,
				   m_flatHoustGPU.count,
				   GL_UNSIGNED_INT,
				   nullptr);

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
	glBindTexture(GL_TEXTURE_2D, m_seamlessGrassTexture);
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
	glUniform1i(ul("seamlessGrass"), 5);
	glUniform1i(ul("brownTexture"), 6);
	glUniform1i(ul("snowTexture"), 7);
	glUniform1i(ul("sandTexture"), 8);

	matWorld = glm::mat4(1.0f) * glm::scale(TABLE_SCALE);
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása
	glUniform3fv(ul("cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPos"), 1, glm::value_ptr(m_lightPos));

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

	/***********************************/
	/***********************************/
	/************ BUILDING *************/

	glBindVertexArray(m_flatHoustGPU.vaoID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_houseTexture);

	glUseProgram(m_buildingID);

	glUniform1i(ul("texImage"), 0);

	matWorld = glm::mat4(1.0f) * glm::scale(BUILDING_SCALE);
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(matWorld));
	glUniformMatrix4fv(ul("worldIT"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(matWorld))));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glDrawElements(GL_TRIANGLES,
		m_flatHoustGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

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

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

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

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, (void*)m_data);

	std::cout << (*m_data).x << " " << (*m_data).y << "\n";
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

