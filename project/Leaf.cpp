#include "Leaf.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>
#include <common/texture.h>
#include <common/shader.h>
#include <common/util.h>
#include "Tree.h"
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
GLuint leafshaderProgram;
GLuint leafdiffuceColorSampler;
GLuint leafModelMatrixLocation;
GLuint leafMLocation;
GLuint leafTexture;
GLuint leafVAO;
GLuint leafVerticiesVBO, leafUVVBO, leafNormalsVBO;
#define PI 3.141592653589793238463


Leaf::Leaf(GLuint leafshaderProgram)
{
	glUseProgram(leafshaderProgram);
	leafdiffuceColorSampler = glGetUniformLocation(leafshaderProgram, "diffuceColorSampler");
	leafModelMatrixLocation = glGetUniformLocation(leafshaderProgram, "M");

	leafTexture = loadSOIL("textures/leaf.bmp");
	glGenVertexArrays(1, &leafVAO);
	glBindVertexArray(leafVAO);
	//Load Obj
	loadOBJWithTiny("models/leaf.obj", leafVertices, leafUVs, leafNormals);
	// vertex VBO
	glGenBuffers(1, &leafVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, leafVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, leafVertices.size() * sizeof(glm::vec3),
		&leafVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &leafNormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, leafNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, leafNormals.size() * sizeof(glm::vec3),
		&leafNormals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	// leafleafUVs VBO
	glGenBuffers(1, &leafUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, leafUVVBO);
	glBufferData(GL_ARRAY_BUFFER, leafUVs.size() * sizeof(glm::vec2),
		&leafUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

}
Leaf::~Leaf() {
}

#include <list>
std::list<int> fallenLeaves;
std::list<int> leavesOnGround;
float stratTime = 0.0;
float FzNormLeaf;
float FxNormLeaf;


void Leaf::draw(Tree tr,GLuint leafshaderProgram, float Fx, float Fz) {

	glUseProgram(leafshaderProgram);
	leafMLocation = glGetUniformLocation(leafshaderProgram, "M");
	// bind
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, leafTexture);
	glUniform1i(leafdiffuceColorSampler, 0);
	glBindVertexArray(leafVAO);


	//Normalize Fz anf Fx
	FzNormLeaf = PI * Fz / 200; //F between -100 to 100 and angle from -pi/2 to pi/2
	FxNormLeaf = PI * Fx / 200;


	
	float Sf = pow(pow(Fx, 2) + pow(Fz, 2), 0.5);
	//if SF > 40 make leaves fall
	if (Sf > 40) {
		stratTime = glfwGetTime();
 		//find fallen leafs
		for (int i = 0; i < tr.leafsArray.size(); i++){
				fallenLeaves.push_back(i);
		}
	}
	//find delta time
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - stratTime;

	for (int i = 0; i < tr.leafsArray.size(); i++)
	{
		//found if leaf is in fallen leafs
		bool foundFalling = (std::find(fallenLeaves.begin(), fallenLeaves.end(), i) != fallenLeaves.end());
		bool foundOnGround = (std::find(leavesOnGround.begin(), leavesOnGround.end(), i) != leavesOnGround.end());
		//if leaf is falling make this fall or keep it to the ground
		if (foundFalling) {
			tr.leafsArray[i] = tr.leafsArray[i] + vec3(0, -10 * pow(deltaTime, 2), 0); //dx = 10 * dt ( 10 = Gravitational constant)
			if (foundOnGround) {
				mat4 leafTranslation = glm::translate(mat4(), tr.leafsArray[i] + vec3(0, -tr.leafsArray[i].y + 0.1, 0));
				mat4 leafScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
				mat4 leafRotation = glm::rotate(mat4(), i * 3.14f / 4.0f, vec3(0.7, 0.2, 0));
				leafModelMatrix = leafTranslation * leafRotation * leafScaling;
			}
			else if (tr.leafsArray[i].y < 0.1) { //if y<0.1 ->> ground
				mat4 leafTranslation = glm::translate(mat4(), tr.leafsArray[i] + vec3(0, -tr.leafsArray[i].y + 0.1, 0));
				mat4 leafScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
				mat4 leafRotation = glm::rotate(mat4(), i * 3.14f / 4.0f, vec3(0.7, 0.2, 0));
				leafModelMatrix = leafTranslation * leafRotation * leafScaling;
				leavesOnGround.push_back(i);
			}
			else { // if y>0.1 make it fall
				//tr.leafsArray[i] = tr.leafsArray[i] + vec3(0, -10 * deltaTime, 0);
				mat4 leafTranslation = glm::translate(mat4(), tr.leafsArray[i] * vec3(1, 1, 1));
				mat4 leafScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
				mat4 leafRotation = glm::rotate(mat4(), i * 3.14f / 4.0f, vec3(0.7, 0.2, 0));
				leafModelMatrix = leafTranslation * leafRotation * leafScaling;
			}
		}
		else { //if is not falling 
			mat4 leafTranslation = glm::translate(mat4(), tr.leafsArray[i] * vec3(1, 1, 1));
			mat4 leafScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
			mat4 leafRotation = glm::rotate(mat4(), i * 3.14f / 4.0f, vec3(0.7, 0.2, 0)) * glm::rotate(mat4(),FxNormLeaf , vec3(0.0, 0.0, 1)) * glm::rotate(mat4(), FzNormLeaf, vec3(1, 0.0, 0)); //rotate extra when windy
			leafModelMatrix = leafTranslation * leafRotation * leafScaling;
		}
		//update modelMatrix
		glUniformMatrix4fv(leafMLocation, 1, GL_FALSE, &leafModelMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, leafVertices.size());

		}


}

void Leaf::genBuffersForShadow(GLuint leafshaderProgram) {
	glUseProgram(leafshaderProgram);
	glGenBuffers(1, &leafVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, leafVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, leafVertices.size() * sizeof(glm::vec3),
		&leafVertices[0], GL_STATIC_DRAW);
}

void Leaf::update(float t, float dt) {
}

void Leaf::free() {
	glDeleteBuffers(1, &leafVerticiesVBO);
	glDeleteBuffers(1, &leafUVVBO);
	glDeleteBuffers(1, &leafNormalsVBO);
	glDeleteVertexArrays(1, &leafVAO);
	glDeleteTextures(1, &leafTexture);
}