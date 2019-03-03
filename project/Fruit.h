#ifndef FRUIT_H
#define FRUIT_H

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

class Fruit {
public:
	//Drawable* sphere;
	float r;
	vec3 fruitPosition;
	glm::mat4 fruitModelMatrix;
	//fruit
	//GLuint fruitVAO, triangleVAO;
	//GLuint fruitVerticiesVBO, fruitUVVBO, fruitNormalsVBO;

	std::vector<vec3> fruitVertices;
	std::vector<vec3> fruitNormals;
	std::vector<vec2> fruitUVs;
	GLuint Texture;

	Fruit(GLuint fruitshaderProgram);
	~Fruit();

	void draw(Tree tr, GLuint fruitshaderProgram, float Fx, float Fz);
	void update(float t = 0, float dt = 0);
	void free();
	void genBuffersForShadow(GLuint fruitshaderProgram);
};

#endif