#include "FlatHouse.hpp"

FlatHouse::FlatHouse() : Building(FLAT_HOUSE) {
	meshCPU.vertexArray =
	{
		// SZEMKÖZTI OLDAL
		{ glm::vec3(-0.5, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(0.5, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(-0.5,  0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(0.5,  0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 1.0) },

		// TETÕ
		{ glm::vec3(-0.5,  0.5, 0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5,  0.5, 0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3(-0.5,  0.5, -0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.5, 1.0) },
		{ glm::vec3(0.5,  0.5, -0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0) },

		// JOBB OLDAL
		{ glm::vec3(0.5, -0.5,   0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(0.5, -0.5,  -0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5,  0.5,   0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(0.5,  0.5,  -0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.5, 1.0) },

		// HÁTLAP
		{ glm::vec3(0.5, -0.5,  -0.5),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(-0.5, -0.5,  -0.5),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5,  0.5,  -0.5),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(-0.5,  0.5,  -0.5),   glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.5, 1.0) },

		// BAL OLDAL
		{ glm::vec3(-0.5, -0.5,  -0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(-0.5, -0.5,   0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(-0.5,  0.5,  -0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(-0.5,  0.5,   0.5),   glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.5, 1.0) },
	};

	meshCPU.indexArray =
	{
		// SZEMKÖZTI OLDAL
		0, 1, 2,
		1, 3, 2,

		// TETÕ
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
}

FlatHouse::~FlatHouse() {}

MeshObject<Vertex> FlatHouse::GetMesh() {
	return meshCPU;
}