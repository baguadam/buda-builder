#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class LittleHouse : Building {
public:
	LittleHouse();
	~LittleHouse();
	MeshObject<Vertex> GetMesh();
};