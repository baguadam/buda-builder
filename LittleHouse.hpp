#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class LittleHouse : Building {
private:
	const glm::vec3 LITTLE_HOUSE_SCALE = glm::vec3(4.0f, 2.0f, 4.0f);
public:
	LittleHouse();
	~LittleHouse();
	MeshObject<Vertex> GetMesh();

	int GetRadiusX() const;
	int GetRadiusY() const;
	int GetRadiusZ() const;
};