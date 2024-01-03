#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"
#include "Building.hpp"

class Tower : Building {

private:
	const int triangleCount = 10;
	const int bottomZ = -2;
	const int topZ = 2;

	void AddCircleCoordinates(int coordinateZ);
	void AddCircleIndexes(int from, int to, bool isBaseCircle);
	void AddWallIndexes();
public:
	Tower();
	~Tower();
	MeshObject<Vertex> GetMesh();
};