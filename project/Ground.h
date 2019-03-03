#ifndef GROUND_H
#define GROUND_H

#include <vector>
#include <stdio.h>

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

class Ground {
public:
	//Drawable* sphere;
	float r;
	vec3 groundPosition;
	glm::mat4 groundModelMatrix;
	//Ground
	//GLuint treeVAO, triangleVAO;
	//GLuint treeVerticiesVBO, treeUVVBO, treeNormalsVBO;

	std::vector<vec3> groundVertices;
	std::vector<vec3> groundNormals;
	std::vector<vec2> groundUVs;
	GLuint Texture;

	Ground(GLuint groundshaderProgram);
	~Ground();

	void draw(GLuint groundshaderProgram);
	void update(float t = 0, float dt = 0);
	void free();
	void genBuffersForShadow(GLuint groundshaderProgram);
};

#endif