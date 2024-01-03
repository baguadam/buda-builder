#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class FamilyHouse : Building {
private:
	int radiusX = 1;
	int radiusZ = 0.5;
	int radiusY = 0.5;
public:
	FamilyHouse();
	~FamilyHouse();
	MeshObject<Vertex> GetMesh();

	int GetRadiusX() const;
	int GetRadiusY() const;
	int GetRadiusZ() const;
};