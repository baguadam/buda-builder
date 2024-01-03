#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"
#include "Camera.h"
#include "FlatHouse.hpp"
#include "LittleHouse.hpp"
#include "FamilyHouse.hpp"
#include "Tower.hpp"

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Program indulása óta eltelt idő
	float DeltaTimeInSec   = 0.0f; // Előző Update óta eltelt idő
};

struct StoredBuilding {
	glm::vec3 buildingPosition;
	BuildingType type;

	StoredBuilding(glm::vec3 _buildingPosition, BuildingType _type) : buildingPosition(_buildingPosition), type(_type) { };
};

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update( const SUpdateInfo& );
	void Render();
	void RenderGUI();

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	// ablak mérete
	int width = 800;
	int height = 600;

	FlatHouse m_flatHouse{};
	LittleHouse m_littleHouse{};
	FamilyHouse m_familyHouse{};

	void SetupDebugCallback();
	std::vector<float> GenerateHeightMap();
	std::vector<float> m_heightMapData;
	std::vector<glm::vec4> m_splatMapData;
	std::vector<glm::vec4> GenerateSplatMap();
	void InitHeightMap();
	void InitSplatMap();
	// FBO létrehozása
	void CreateFrameBuffer(int width, int height);
	// épületek renderelése
	void RenderFlatAndBlockHouse(glm::vec3 buildingPosition, BuildingType type);
	void RenderLittleHouse(glm::vec3 buildingPosition);
	void RenderFamilyHouse(glm::vec3 buildingPosition);
	// magasságtérkép lesimítása az épületek alatt, beton elhelyezése
	glm::vec2 GetRadiusPixels(BuildingType type);
	void FlattenTerrainUnderBuilding(glm::vec2 uv, BuildingType type);
	void PlaceConcreteUnderBuilding(glm::vec2 uv, BuildingType type);
	// AABB
	glm::vec3 GetBuildingDimensions(BuildingType type);
	bool CheckBuildingCollisions(const glm::vec3 newPosition, BuildingType type);

	// FBO-hoz szükséges változók
	bool m_frameBufferCreated{ false };	// Korábban hoztunk már létre FBO-t?
	GLuint m_depthBuffer;				// FBO Mélységbuffer textúra azonosító
	GLuint m_colorBuffer;				// FBO Színbuffer textúra azonosító
	GLuint m_frameBuffer;				// FBO azonosító
	glm::vec3* m_data;  // itt fogjuk eltárolni az olvasott koordináta-hármast

	// épület lehelyezés
	BuildingType selectedBuilding = FLAT_HOUSE;
	std::vector<StoredBuilding> m_buildingTypePositionVector{}; // ebben tároljuk az újonnan létrehozott épületek koordinátáit koordinátáit

	// napszakváltás, eltelt idő
	float m_dayDuration = 30.0f;
	float m_sunRadius = 200.0f;
	float m_time;
	glm::vec4 m_sunMoonDirectionalLight = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	glm::vec3 m_lightColor;
	glm::vec3 CMyApp::InterpolateColor(glm::vec3 color1, glm::vec3 color2, float t);
	glm::vec3 CMyApp::CalculateLightColor(float timeOfDay);
	int m_currentHours;
	int m_currentMinutes;

	// A parametrikus felülettel kapcsolatos változók
	static constexpr float       TABLE_RESOLUTION = 256.0f;
	static constexpr int	     SCALE_VALUE = 200;
	static constexpr glm::vec3   TABLE_SCALE = glm::vec3(700.0f, 1.0f, 700.0f);

	static constexpr float	   FLAT_BUILDING_RADIUS   = 4.0;
	static constexpr float	   FLAT_BUILDING_RADIUS_Y = 2.2;

	// heightmap
	std::vector<float> noiseData;

	float m_ElapsedTimeInSec = 0.0f;

	// Kamera
	Camera m_camera;

	//
	// OpenGL-es dolgok
	//
	
	// uniform location lekérdezése
	GLint ul( const char* uniformName ) noexcept;

	// shaderekhez szükséges változók
	GLuint m_programID  = 0; // shaderek programja
	GLuint m_buildingID = 0;
	GLuint m_FBOID = 0;

	// Fényforrás- ...
	glm::vec4 m_lightPos       = glm::vec4(0.0f, 0.0f,  1.0f, 0.0f);
	glm::vec4 m_lightPosSecond = glm::vec4(-5.0f, 5.0f, 10.0f, 0.0f);

	glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0 );
	glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0 );
	glm::vec3 m_Ls = glm::vec3(1.0, 1.0, 1.0 );

	float m_lightConstantAttenuation    = 1.0;
	float m_lightLinearAttenuation      = 0.0;
	float m_lightQuadraticAttenuation   = 0.0;

	// ... és anyagjellemzők
	glm::vec3 m_Ka = glm::vec3( 1.0 );
	glm::vec3 m_Kd = glm::vec3( 1.0 );
	glm::vec3 m_Ks = glm::vec3( 1.0 );

	float m_Shininess = 1.0;

	// Shaderek inicializálása, és törtlése
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók

	OGLObject m_paramSurfaceGPU = {};
	OGLObject m_flatHoustGPU = {};
	OGLObject m_littleHouseGPU = {};
	OGLObject m_familyHouseGPU = {};

	// Geometria inicializálása, és törtlése
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói
	GLuint m_greenerGrass = 0;
	GLuint m_greenTexture = 0;
	GLuint m_grassTexture = 0;
	GLuint m_seamlessGrassTexture = 0;
	GLuint m_brownTexture = 0;
	GLuint m_snowTexture = 0;
	GLuint m_sandTexture = 0;
	GLuint m_houseTexture = 0;
	GLuint m_heightMapTexture = 0;
	GLuint m_splatMapTexture = 0;
	GLuint m_concreteTexture = 0;

	void InitTextures();
	void CleanTextures();
};

