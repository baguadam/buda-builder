#include "FamilyHouse.hpp"

FamilyHouse::FamilyHouse() : Building(FAMILY_HOUSE) {
	meshCPU.vertexArray =
	{
		// SZEMKÖZTI OLDAL
		{ glm::vec3(-1, -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(1,  -0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(-1, 0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(1, 0.5, 0.5),  glm::vec3(0.0, 0.0, 1.0),  glm::vec2(0.5, 1.0) },

		// JOBB OLDAL
		{ glm::vec3(1, -0.5, 0.5),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(1, -0.5, 0.0),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(1, 0.5, 0.5),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(1, 0.5, 0.0),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.5, 1.0) },

		// HÁTSÓ OLDAL
		{ glm::vec3(0.5, -0.5, 0.0),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(-1.0, -0.5, 0.0),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5, 0.5, 0.0),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(-1.0, 0.5, 0.0),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.5, 1.0) },

		// BAL OLDAL
		{ glm::vec3(-1.0, -0.5, 0.0),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(-1.0, -0.5, 0.5),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(-1.0, 0.5, 0.0),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(-1.0, 0.5, 0.5),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.5, 1.0) },

		// TETÕ
		{ glm::vec3(-1.0, 0.5, 0.5),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3( 1.0, 0.5, 0.5),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(1.0, 0.0) },
		{ glm::vec3(-1.0, 0.5, 0.0),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(0.5, 0.5) },
		{ glm::vec3( 1.0, 0.5, 0.0),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(1.0, 0.5) },

		// L-JOBB OLDAL
		{ glm::vec3(1, -0.5, 0.0),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(1, -0.5, -0.5),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(1, 0.5, 0.0),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(1, 0.5, -0.5),  glm::vec3(1.0, 0.0, 0.0),  glm::vec2(0.5, 1.0) },

		// L-HÁTSÓ OLDAL
		{ glm::vec3(1.0, -0.5, -0.5),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(0.5, -0.5, -0.5),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(1.0, 0.5, -0.5),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(0.5, 0.5, -0.5),  glm::vec3(0.0, 0.0, -1.0),  glm::vec2(0.5, 1.0) },

		// L-BAL
		{ glm::vec3(0.5, -0.5, -0.5),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.0, 0.0) },
		{ glm::vec3(0.5, -0.5, 0.0),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.5, 0.0) },
		{ glm::vec3(0.5, 0.5, -0.5),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.0, 1.0) },
		{ glm::vec3(0.5, 0.5, 0.0),  glm::vec3(-1.0, 0.0, 0.0),  glm::vec2(0.5, 1.0) },

		// L-TETÕ
		{ glm::vec3(0.5, 0.5, 0.0),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(0.875, 0.5) },
		{ glm::vec3(1.0, 0.5, 0.0),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(1, 0.5 ) },
		{ glm::vec3(0.5, 0.5, -0.5),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(0.875, 1.0) },
		{ glm::vec3(1.0, 0.5, -0.5),  glm::vec3(0.0, 1.0, 0.0),  glm::vec2(1.0, 1.0) },
	};

	meshCPU.indexArray =
	{
		// SZEMKÖZTI OLDAL
		0, 1, 2,
		1, 3, 2, 

		// JOBB OLDAL
		4, 5, 6,
		5, 7, 6,

		// HÁTSÓ OLDAL
		8, 9,  10,
		9, 11, 10,
		
		// BAL OLDAL
		12, 13, 14,
		13, 15, 14,

		// TETÕ
		16, 17, 18,
		17, 19, 18,

		// L-JOBB
		20, 21, 22,
		21, 23, 22,

		// L-HÁT
		24, 25, 26,
		25, 27, 26,

		// L-BAL
		28, 29, 30,
		29, 31, 30,

		// L-TETÕ
		32, 33, 34,
		33, 35, 34
	};
}

FamilyHouse::~FamilyHouse() {}

MeshObject<Vertex> FamilyHouse::GetMesh() { return meshCPU; }
int FamilyHouse::GetRadiusX() const { return radiusX; }
int FamilyHouse::GetRadiusY() const { return radiusY; }
int FamilyHouse::GetRadiusZ() const { return radiusZ; }