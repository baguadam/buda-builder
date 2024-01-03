#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class FlatHouse : Building {
private:
	const glm::vec3 FLAT_HOUSE_SCALE  = glm::vec3(4.0f, 2.0f, 4.0f);
	const glm::vec3 BLOCK_HOUSE_SCALE = glm::vec3(4.0f, 4.0f, 2.0f);

public:
	FlatHouse();
	~FlatHouse();
	MeshObject<Vertex> GetMesh();
	glm::vec3 GetFlatScale() const;
	glm::vec3 GetBlockScale() const;
	int GetFlatRadiusX() const;
	int GetFlatRadiusY() const;
	int GetFlatRadiusZ() const;
	int GetBlockRadiusX() const;
	int GetBlockRadiusY() const;
	int GetBlockRadiusZ() const;
};