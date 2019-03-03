#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <stdio.h>
#include "Tree.h"
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Drawable;

class Skeleton {
public:
	//Drawable* sphere;
	float r;
	vec3 skeletonPosition;
	glm::mat4 skeletonModelMatrix;
	//skeleton
	//GLuint skeletonVAO, triangleVAO;
	//GLuint skeletonVerticiesVBO, skeletonUVVBO, skeletonNormalsVBO;

	std::vector<vec3> skeletonVertices;
	std::vector<vec3> skeletonNormals;
	std::vector<vec2> skeletonUVs;
	GLuint Texture;

	Skeleton(GLuint skeletonshaderProgram);
	~Skeleton();

	void draw(GLuint skeletonshaderProgram, float Z, float X, bool RENDER_SKELETON, float Fz, float Fx);
	void update(float t = 0, float dt = 0);
	void free();
	void genBuffersForShadow(GLuint skeletonshaderProgram);
};

#endif