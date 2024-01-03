#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class FamilyHouse : Building {
private:
	const glm::vec3 FAMILY_HOUSE_SCALE = glm::vec3(4.0f, 4.0f, 4.0f);

	int radiusX = 4.0;
	int radiusZ = 2.0;
	int radiusY = 2.0;
public:
	FamilyHouse();
	~FamilyHouse();
	MeshObject<Vertex> GetMesh();

	int GetRadiusX() const;
	int GetRadiusY() const;
	int GetRadiusZ() const;
	glm::vec3 GetScale() const;
};