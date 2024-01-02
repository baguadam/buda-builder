#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class FlatHouse : Building {
public:
	FlatHouse();
	~FlatHouse();
	MeshObject<Vertex> GetMesh();
};