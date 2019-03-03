#include "Tree.h"
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
//del
#include "glm/gtx/string_cast.hpp"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.h>


using namespace glm;
using namespace std;
GLuint treeshaderProgram;
GLuint treediffuceColorSampler;
GLuint treeModelMatrixLocation;
GLuint treeMLocation;
GLuint treeTexture;
GLuint treeVAO;
GLuint treeVerticiesVBO, treeUVVBO, treeNormalsVBO;

vec3 verticalVec= vec3(0, -1, 0);

Tree::Tree(GLuint treeshaderProgram)
{
	glUseProgram(treeshaderProgram);

	treediffuceColorSampler = glGetUniformLocation(treeshaderProgram, "diffuceColorSampler");

	treeTexture = loadSOIL("textures/tree_bark.bmp");
	glGenVertexArrays(1, &treeVAO);
	glBindVertexArray(treeVAO);

	//Load OBJ
	loadOBJWithTiny("models/tree.obj", treeVertices, treeUVs, treeNormals);

	// vertex VBO
	glGenBuffers(1, &treeVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, treeVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, treeVertices.size() * sizeof(glm::vec3),
		&treeVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &treeNormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, treeNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, treeNormals.size() * sizeof(glm::vec3),
		&treeNormals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// treetreeUVs VBO
	glGenBuffers(1, &treeUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, treeUVVBO);
	glBufferData(GL_ARRAY_BUFFER, treeUVs.size() * sizeof(glm::vec2),
		&treeUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);


}
Tree::~Tree() {
}

void Tree::draw(GLuint treeshaderProgram) {
	glUseProgram(treeshaderProgram);
	treeMLocation = glGetUniformLocation(treeshaderProgram, "M");
	// bind
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, treeTexture);
	glUniform1i(treediffuceColorSampler, 0);
	glBindVertexArray(treeVAO);
	//treePosition = vec3(3, 0, 0);
	mat4 treeTranslation = glm::translate(mat4(), treePosition);
	treeModelMatrix = treeTranslation;
	glUniformMatrix4fv(treeMLocation, 1, GL_FALSE, &treeModelMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, treeVertices.size());
}

void Tree::genBuffersForShadow(GLuint treeshaderProgram) {
	glUseProgram(treeshaderProgram);
	glGenBuffers(1, &treeVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, treeVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, treeVertices.size() * sizeof(glm::vec3),
		&treeVertices[0], GL_STATIC_DRAW);
}

void Tree::free() {
	glDeleteBuffers(1, &treeVerticiesVBO);
	glDeleteBuffers(1, &treeUVVBO);
	glDeleteBuffers(1, &treeNormalsVBO);
	glDeleteVertexArrays(1, &treeVAO);
	glDeleteTextures(1, &treeTexture);
}

//Find edge for fruits and leaves
void Tree::edgeDetection(Tree tr, int Nleaf, int Nfrouts)
{
	for (int i=0; i < Nleaf; i++) {
		int RandIndex = rand() % treeVertices.size();
		if ((pow(treeVertices[RandIndex].x, 2) + pow(treeVertices[RandIndex].z, 2) > 0.3)) {
			vec4 transformedVector = treeModelMatrix * vec4(treeVertices[RandIndex], 1.0);
			leafsArray.push_back(vec3(transformedVector.x, transformedVector.y, transformedVector.z));
		}
	}
	for (int i=0; i < Nfrouts; i++) {
		int RandIndex = rand() % treeVertices.size();
		if (dot(treeNormals[RandIndex], verticalVec) > 0.5 && (pow(treeVertices[RandIndex].x, 2) + pow(treeVertices[RandIndex].z, 2) > 0.3)) {
			vec4 transformedVector = treeModelMatrix * vec4(treeVertices[RandIndex], 1.0);
			fruitsArray.push_back(vec3(transformedVector.x, transformedVector.y, transformedVector.z));
		}
	}
}

