#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class FamilyHouse : Building {
public:
	FamilyHouse();
	~FamilyHouse();
	MeshObject<Vertex> GetMesh();
};