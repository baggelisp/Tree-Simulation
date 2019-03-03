#include "Ground.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>
#include <common/texture.h>
#include <common/shader.h>
#include <common/util.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.h>

using namespace glm;

using namespace std;
GLuint groundshaderProgram;
GLuint grounddiffuceColorSampler;
GLuint groundModelMatrixLocation;
GLuint groundMLocation;
GLuint groundTexture;
GLuint groundVAO;
GLuint groundVerticiesVBO, groundUVVBO, groundNormalsVBO;


Ground::Ground(GLuint groundshaderProgram)
{
	glUseProgram(groundshaderProgram);
	grounddiffuceColorSampler = glGetUniformLocation(groundshaderProgram, "diffuceColorSampler");
	groundModelMatrixLocation = glGetUniformLocation(groundshaderProgram, "M");
	groundTexture = loadSOIL("textures/grass.png");
	glGenVertexArrays(1, &groundVAO);
	glBindVertexArray(groundVAO);
	loadOBJWithTiny("models/grass.obj", groundVertices, groundUVs, groundNormals);

	// vertex VBO
	glGenBuffers(1, &groundVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(glm::vec3),
		&groundVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &groundNormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, groundNormals.size() * sizeof(glm::vec3),
		&groundNormals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// groundUVs VBO
	glGenBuffers(1, &groundUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundUVVBO);
	glBufferData(GL_ARRAY_BUFFER, groundUVs.size() * sizeof(glm::vec2),
		&groundUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
}
Ground::~Ground() {
}

void Ground::draw(GLuint groundshaderProgram) {

	glUseProgram(groundshaderProgram);
	groundMLocation = glGetUniformLocation(groundshaderProgram, "M");
	// bind
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glUniform1i(grounddiffuceColorSampler, 0);
	glBindVertexArray(groundVAO);
	mat4 groundScaling = glm::scale(mat4(), vec3(1, 1, 1));
	mat4 groundTranslation = glm::translate(mat4(), groundPosition);
	groundModelMatrix = groundTranslation * groundScaling;
	glUniformMatrix4fv(groundMLocation, 1, GL_FALSE, &groundModelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, groundVertices.size());
}

void Ground::genBuffersForShadow(GLuint groundshaderProgram) {
	glUseProgram(groundshaderProgram);
	glGenBuffers(1, &groundVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, groundVertices.size() * sizeof(glm::vec3),
		&groundVertices[0], GL_STATIC_DRAW);
}



void Ground::update(float t, float dt) {
}

void Ground::free() {
	glDeleteBuffers(1, &groundVerticiesVBO);
	glDeleteBuffers(1, &groundUVVBO);
	glDeleteBuffers(1, &groundNormalsVBO);
	glDeleteVertexArrays(1, &groundVAO);
	glDeleteTextures(1, &groundTexture);
}