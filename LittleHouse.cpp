#include "LittleHouse.hpp"

LittleHouse::LittleHouse() : Building(LITTLE_HOUSE) {
	meshCPU.vertexArray =
	{
		// SZEMKÖZTI
		{ glm::vec3(-0.5,   0.0,     0.5),   glm::vec3(0.0, 0.447214, 0.894427), glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5,    0.0,     0.5),   glm::vec3(0.0, 0.447214, 0.894427), glm::vec2(1.0, 0.0) },
		{ glm::vec3(0.0,    1.0,     0.0),   glm::vec3(0.0, 0.447214, 0.894427), glm::vec2(0.75, 1.0) },

		// JOBB OLDAL
		{ glm::vec3(0.5,    0.0,     0.5),   glm::vec3(0.894427, 0.447214, 0.0), glm::vec2(0.5, 0.0)},
		{ glm::vec3(0.5,    0.0,    -0.5),   glm::vec3(0.894427, 0.447214, 0.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3(0.0,    1.0,     0.0),   glm::vec3(0.894427, 0.447214, 0.0), glm::vec2(0.75, 1.0) },

		// HÁTSÓ
		{ glm::vec3(0.5,    0.0,    -0.5),   glm::vec3(0.0, 0.447214, -0.894427), glm::vec2(0.5, 0.0) },
		{ glm::vec3(-0.5,   0.0,    -0.5),   glm::vec3(0.0, 0.447214, -0.894427), glm::vec2(1.0, 0.0) },
		{ glm::vec3(0.0,    1.0,     0.0),   glm::vec3(0.0, 0.447214, -0.894427), glm::vec2(0.75, 1.0) },

		// BAL OLDAL
		{ glm::vec3(-0.5,   0.0,    -0.5),   glm::vec3(-0.894427, 0.447214, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(-0.5,   0.0,     0.5),   glm::vec3(-0.894427, 0.447214, 0.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3(0.0,    1.0,     0.0),   glm::vec3(-0.894427, 0.447214, 0.0), glm::vec2(0.75, 1.0) },
	};

	meshCPU.indexArray =
	{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11
	};
}

LittleHouse::~LittleHouse() {}

MeshObject<Vertex> LittleHouse::GetMesh() { return meshCPU; }
int LittleHouse::GetRadiusX() const { return LITTLE_HOUSE_SCALE.x / 2; }
int LittleHouse::GetRadiusY() const { return LITTLE_HOUSE_SCALE.y / 2; }
int LittleHouse::GetRadiusZ() const { return LITTLE_HOUSE_SCALE.z / 2; }