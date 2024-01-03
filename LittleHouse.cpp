#include "LittleHouse.hpp"

LittleHouse::LittleHouse() : Building(LITTLE_HOUSE) {
	meshCPU.vertexArray =
	{
		// TETÕ
		{ glm::vec3(-0.5,   0,     0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3( 0.5,   0,     0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3( 0.5,   0,    -0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(1.0, 1.0) },
		{ glm::vec3(-0.5,   0,    -0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.5, 1.0) },
		{ glm::vec3(   0,   1,     0),     glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.75,0.5) },
	};

	meshCPU.indexArray =
	{
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
	};
}

LittleHouse::~LittleHouse() {}

MeshObject<Vertex> LittleHouse::GetMesh() { return meshCPU; }
int LittleHouse::GetRadiusX() const { return LITTLE_HOUSE_SCALE.x / 2; }
int LittleHouse::GetRadiusY() const { return LITTLE_HOUSE_SCALE.y / 2; }
int LittleHouse::GetRadiusZ() const { return LITTLE_HOUSE_SCALE.z / 2; }