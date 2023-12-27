//// GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/transform2.hpp>
//
//// GLEW
//#include <GL/glew.h>
//
//// SDL
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
//
//// Utils
//#include "GLUtils.hpp"
//
//#include <cmath>
//#include <iostream>
//
//class PerlinNoise {
//public:
//	float fade(float t) {
//		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
//	}
//
//	float grad(float p) {
//		const float texture_width = 256.0;
//		float v = glm::texture2D(iChannel0, glm::vec2(p / texture_width, 0.0)).r;
//		return v > 0.5 ? 1.0 : -1.0;
//	}
//
//	float noise(float p) {
//		float p0 = floor(p);
//		float p1 = p0 + 1.0;
//
//		float t = p - p0;
//		float fade_t = fade(t);
//
//		float g0 = grad(p0);
//		float g1 = grad(p1);
//
//		return (1.0 - fade_t) * g0 * (p - p0) + fade_t * g1 * (p - p1);
//	}
//};