#pragma once
#include <iostream>
#include <GLUtils.hpp>
#include "BuildingTypes.hpp"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"
#include "Camera.h"

class Building {

protected:
	MeshObject<Vertex> meshCPU;
	BuildingType type;

public:
	Building(BuildingType _type);
	BuildingType GetType() const;
	~Building();

	virtual MeshObject<Vertex> GetMesh() = 0;
};