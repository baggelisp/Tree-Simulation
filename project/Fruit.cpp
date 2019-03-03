#include "Fruit.h"
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
GLuint fruitshaderProgram;
GLuint fruitdiffuceColorSampler;
GLuint fruitModelMatrixLocation;
GLuint fruitMLocation;
GLuint fruitTexture;
GLuint fruitVAO;
GLuint fruitVerticiesVBO, fruitUVVBO, fruitNormalsVBO;

Fruit::Fruit(GLuint fruitshaderProgram)
{
	glUseProgram(fruitshaderProgram);
	fruitdiffuceColorSampler = glGetUniformLocation(fruitshaderProgram, "diffuceColorSampler");

	fruitTexture = loadSOIL("textures/fruit.jpg");
	glGenVertexArrays(1, &fruitVAO);
	glBindVertexArray(fruitVAO);
	//Load Obj
	loadOBJWithTiny("models/fruit.obj", fruitVertices, fruitUVs, fruitNormals);
	// vertex VBO
	glGenBuffers(1, &fruitVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fruitVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, fruitVertices.size() * sizeof(glm::vec3),
		&fruitVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &fruitNormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fruitNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, fruitNormals.size() * sizeof(glm::vec3),
		&fruitNormals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	// fruitUVs VBO
	glGenBuffers(1, &fruitUVVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fruitUVVBO);
	glBufferData(GL_ARRAY_BUFFER, fruitUVs.size() * sizeof(glm::vec2),
		&fruitUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

}
Fruit::~Fruit() {
}

#include <list>

std::list<int> fallenFruits;
std::list<int> fruitsOnGround;
float timeFruit = 0.0;
float currentTime = glfwGetTime();
float looptime = 0.0;
void Fruit::draw(Tree tr, GLuint fruitshaderProgram, float Fx, float Fz) {

	//fibd loop time
	looptime = glfwGetTime() - currentTime;
	currentTime = glfwGetTime();
	//cout << "looptime=" << looptime;

	glUseProgram(fruitshaderProgram);
	fruitMLocation = glGetUniformLocation(fruitshaderProgram, "M");
	// bind
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fruitTexture);
	glUniform1i(fruitdiffuceColorSampler, 0);
	glBindVertexArray(fruitVAO);

	float Sf = pow(pow(Fx, 2) + pow(Fz, 2), 0.5);

	if (Sf > 80) {
		timeFruit = glfwGetTime();
		//find fallen fruits
		for (int i = 0; i < tr.fruitsArray.size(); i++){
		fallenFruits.push_back(i);
		}
	}
	//find delta time
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - timeFruit;
	
	for (int i = 0; i < tr.fruitsArray.size(); i++)
	{
		//found if fruit is in fallen Fruits or if is on ground
		bool foundFalling = (std::find(fallenFruits.begin(), fallenFruits.end(), i) != fallenFruits.end());
		bool foundOnGround = (std::find(fruitsOnGround.begin(), fruitsOnGround.end(), i) != fruitsOnGround.end());
		//if fruit is falling make this fall or keep it to the ground
		if (foundFalling) {
			tr.fruitsArray[i] = tr.fruitsArray[i] + vec3(0, -10 * pow(deltaTime,2), 0); //dx = 10 * dt ( 10 = Gravitational constant)
			if (foundOnGround) {
				mat4 fruitTranslation = glm::translate(mat4(), tr.fruitsArray[i] + vec3(0, -tr.fruitsArray[i].y + 0.1, 0));
				mat4 fruitScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
				fruitModelMatrix = fruitTranslation * fruitScaling;
			}
			else if (tr.fruitsArray[i].y < 0.1){ //if y<0.1 ->> ground
				mat4 fruitTranslation = glm::translate(mat4(), tr.fruitsArray[i] + vec3(0, -tr.fruitsArray[i].y + 0.1 , 0) );
				mat4 fruitScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
				fruitModelMatrix = fruitTranslation * fruitScaling;
				fruitsOnGround.push_back(i);
			}
			else { // if y>0.1 make it fall
				//tr.fruitsArray[i] = tr.fruitsArray[i] + vec3(0, -10 * deltaTime, 0);
				mat4 fruitTranslation = glm::translate(mat4(), tr.fruitsArray[i] + vec3(0, -0.08, 0));
				mat4 fruitScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
				fruitModelMatrix = fruitTranslation * fruitScaling;
			}
		}
		else { //if is not falling 
			mat4 fruitTranslation = glm::translate(mat4(), tr.fruitsArray[i] + vec3(0, -0.08, 0));
			mat4 fruitScaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
			fruitModelMatrix = fruitTranslation * fruitScaling;
		}
		//update modelMatrix
		glUniformMatrix4fv(fruitMLocation, 1, GL_FALSE, &fruitModelMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, fruitVertices.size());
		
	}

}

void Fruit::genBuffersForShadow(GLuint fruitshaderProgram) {
	glUseProgram(fruitshaderProgram);
	glGenBuffers(1, &fruitVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fruitVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, fruitVertices.size() * sizeof(glm::vec3),
		&fruitVertices[0], GL_STATIC_DRAW);
}


void Fruit::update(float t, float dt) {
}

void Fruit::free() {
	glDeleteBuffers(1, &fruitVerticiesVBO);
	glDeleteBuffers(1, &fruitUVVBO);
	glDeleteBuffers(1, &fruitNormalsVBO);
	glDeleteVertexArrays(1, &fruitVAO);
	glDeleteTextures(1, &fruitTexture);
}