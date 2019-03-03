#ifndef LEAF_H
#define LEAF_H

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

class Leaf {
public:
	//Drawable* sphere;
	float r;
	vec3 leafPosition;
	glm::mat4 leafModelMatrix;
	//leaf
	//GLuint leafVAO, triangleVAO;
	//GLuint leafVerticiesVBO, leafUVVBO, leafNormalsVBO;

	std::vector<vec3> leafVertices;
	std::vector<vec3> leafNormals;
	std::vector<vec2> leafUVs;
	GLuint Texture;

	Leaf(GLuint leafshaderProgram);
	~Leaf();

	void draw(Tree tr,GLuint leafshaderProgram, float Z, float X);
	void update(float t = 0, float dt = 0);
	void free();
	void genBuffersForShadow(GLuint leafshaderProgram);
};

#endif