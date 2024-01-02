#include "FlatHouse.hpp"

FlatHouse::FlatHouse() : Building(FLAT_HOUSE) {
	meshCPU.vertexArray =
	{
		// SZEMK�ZTI OLDAL
		{ glm::vec3(-0.5, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(0.5, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(-0.5,  0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(0.5,  0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 1.0) },

		// TET�
		{ glm::vec3(-0.5,  0.5, 0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5,  0.5, 0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3(-0.5,  0.5, -0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.5, 1.0) },
		{ glm::vec3(0.5,  0.5, -0.5),   glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0) },

		// JOBB OLDAL
		{ glm::vec3(0.5, -0.5,   0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(0.5, -0.5,  -0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5,  0.5,   0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(0.5,  0.5,  -0.5),   glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.5, 1.0) },

		// H�TLAP
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
		// SZEMK�ZTI OLDAL
		0, 1, 2,
		1, 3, 2,

		// TET�
		16, 17, 18,
		17, 19, 18,

		// JOBB OLDAL
		4, 5, 6,
		5, 7, 6,

		// H�TLAP
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