// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
//headers files
#include "Tree.h"
#include "Ground.h"
#include "Leaf.h"
#include "Fruit.h"
#include "Skeleton.h"
using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void printHelpMenu();


#define W_WIDTH 1366
#define W_HEIGHT 800
#define TITLE "Tree - Project"

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram;
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation ;
GLuint MLocation, MVPLocation;
GLuint lightLocation;
GLuint diffuceColorSampler, specularColorSampler;
GLuint KsLocation, KdLocation, KaLocation , Ns , spotlightAngle;
vec3 lightPos;
Tree* tree;
Ground* ground;
Leaf* leaf;
Fruit* fruit;
Skeleton* skeleton;
GLuint depthProgramID;
GLuint depthMatrixID;
GLuint DepthBiasID;
GLuint ShadowMapID;
GLuint FramebufferName;
GLuint depthTexture;


#define RENDER_TRIANGLE 0
#define PI 3.141592653589793238463
bool  RENDER_SKELETON = false;

int Nleafs=0;
int Nfruits=0;
float stepZforSkeleton=0.0;
float stepXforSkeleton=0.0;

extern float Fz = 0.0;
extern float Fx = 0.0;
float K = 2.5; //spring constant

void createContext()
{
	// Create and compile our GLSL program from the shaders
	shaderProgram = loadShaders(
		"StandardShading.vertexshader",
		"StandardShading.fragmentshader");
	

    // Pointers
    diffuceColorSampler = glGetUniformLocation(shaderProgram, "diffuceColorSampler");
    projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
    viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
    modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
    lightLocation = glGetUniformLocation(shaderProgram, "light_position_worldspace");
	KsLocation = glGetUniformLocation(shaderProgram, "Ks");
	KdLocation = glGetUniformLocation(shaderProgram, "Kd");
	KaLocation = glGetUniformLocation(shaderProgram, "Ka");
	Ns = glGetUniformLocation(shaderProgram, "Ns");
	spotlightAngle = glGetUniformLocation(shaderProgram, "spotlightAngle");
	DepthBiasID = glGetUniformLocation(shaderProgram, "DepthBiasMVP");
	ShadowMapID = glGetUniformLocation(shaderProgram, "shadowMap");

	//Generate buffers

	//Tree
	tree = new Tree(shaderProgram);
	//Ground 
	ground = new Ground(shaderProgram);
	//Leaf
	leaf = new Leaf(shaderProgram);
	//fruit
	fruit = new Fruit(shaderProgram);
	//skeleton
	skeleton = new Skeleton(shaderProgram);

	//Shaders for shadows
	depthProgramID = loadShaders("DepthRTT.vertexshader", "DepthRTT.fragmentshader");

	depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
	MVPLocation = glGetUniformLocation(depthProgramID, "MVP");
	MLocation = glGetUniformLocation(depthProgramID, "M");
	glm::mat4 modelMatrix = glm::mat4(1.0);
	glUniformMatrix4fv(MLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	
	//Work on shadow
	tree->genBuffersForShadow(depthProgramID);
	leaf->genBuffersForShadow(depthProgramID);
	fruit->genBuffersForShadow(depthProgramID);
	ground->genBuffersForShadow(depthProgramID);

	// ---------------------------------------------
	// ---------------------------------------------
	// Render to Texture - specific code begins here

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR ON framebuffer");


}

void free()
{
	tree->free();
	ground->free();
	leaf->free();
	fruit->free();
	skeleton->free();

    glDeleteProgram(shaderProgram);
	glDeleteProgram(depthProgramID);
    glfwTerminate();
}

glm::vec3 lightInvDir;
void mainLoop()
{
	glm::mat4 modelMatrix = glm::mat4(1.0);
	//Light Position
	lightPos = glm::vec3(0, 20, 20);
	
    do
    {
		// Render to our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, 1024, 1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(depthProgramID);

		//light direction normalize
		lightInvDir = glm::vec3(lightPos.x / 10.0, lightPos.y / 10.0, lightPos.z / 10.0);

		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
		glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0);
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
		glUniformMatrix4fv(MLocation, 1, GL_FALSE, &modelMatrix[0][0]);

		//Draw objects to create shadow
		tree->draw(depthProgramID);
		leaf->draw(*tree,depthProgramID, 0, 0);
		fruit->draw(*tree, depthProgramID,0,0);
		skeleton->draw(depthProgramID, stepZforSkeleton, stepXforSkeleton, RENDER_SKELETON, Fz, Fx);

		glUseProgram(depthProgramID);

		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, W_WIDTH, W_HEIGHT); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glEnable(GL_CULL_FACE);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use standard shader
		glUseProgram(shaderProgram);
        // camera
        camera->update();

        // bind
        mat4 projectionMatrix = camera->projectionMatrix;
        mat4 viewMatrix = camera->viewMatrix;
        
		mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
		glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &MVP[0][0]);

		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

        // Task 1.4c: transfer uniforms to GPU
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniform3f(lightLocation, lightPos.x, lightPos.y, lightPos.z); // light
		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(ShadowMapID, 1);

        // draw tree
		glUniform3f(KsLocation, 0.628281, 0.555802, 0.366065);
		glUniform3f(KdLocation, 0.75164, 0.60648, 0.22648);
		glUniform3f(KaLocation, 0.24725, 0.1995, 0.0745);
		glUniform1f(Ns, .25 * 128);
		tree->draw(shaderProgram);

		//ground
		glUniform3f(KsLocation, 0.0,	0.0,	0.0);
		glUniform3f(KdLocation, 0.1	,0.35	,0.1);
		glUniform3f(KaLocation, 0.45,	0.55	,0.45);
		glUniform1f(Ns, .25*128);
		ground->draw(shaderProgram);

		//leaf
		glUniform3f(KsLocation, 0.0, 0.0, 0.0);
		glUniform3f(KdLocation, 0.1, 0.35, 0.1);
		glUniform3f(KaLocation, 0.45, 0.55, 0.45);
		glUniform1f(Ns, .25 * 128);
		leaf->draw(*tree, shaderProgram, Fx, Fz);

		//fruit
		glUniform3f(KsLocation, 0.03	,0.0	,0.0);
		glUniform3f(KdLocation, 0.5,	0.4,	0.4);
		glUniform3f(KaLocation, 0.7	,0.04,	0.04);
		glUniform1f(Ns, .078125*128);
		fruit->draw(*tree, shaderProgram,Fx,Fz);

		//skeleton
		glUniform3f(KsLocation, 0.0, 0.0, 0.0);
		glUniform3f(KdLocation, 0.0, 0.0, 0.0);
		glUniform3f(KaLocation, 0.0, 0.0, 0.0);
		glUniform1f(Ns, .25 * 128);
		skeleton->draw(shaderProgram, stepZforSkeleton, stepXforSkeleton, RENDER_SKELETON,Fz,Fx);


		//spring constant Force
		//return the skeleton to its original state
		if (Fz < 0) {
			Fz += K;
		}
		if (Fz > 0) {
			Fz -= K;
		}
		if (Fx < 0) {
			Fx += K;
		}
		if (Fx > 0) {
			Fx -= K;
		}

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}

void initialize()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
            " If you have an Intel GPU, they are not 3.3 compatible." +
            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.1f, 0.3f, 0.8f, 0.0f);

	glfwSetKeyCallback(window, pollKeyboard);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable textures
    glEnable(GL_TEXTURE_2D);

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);
}

int main(void)
{
    try
    {

        initialize();
		
        createContext();
		printHelpMenu();
        mainLoop();
        free();
    }
    catch (exception& ex)
    {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}

float t;
float d; 

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) { 


	if (key == GLFW_KEY_T) {
		lightPos.y += 0.5;
	}
	if (key == GLFW_KEY_G) {
		lightPos.y -= 0.5;
	}
	if (key == GLFW_KEY_F) {
		lightPos.x -= 0.5;
	}
	if (key == GLFW_KEY_H) {
		lightPos.x += 0.5;
	}
	
	// Make lightPos move on a cycle
	if (key == GLFW_KEY_Q) {
		t += 0.1;
		float radius = pow(pow(lightPos.x,2) + pow(lightPos.z, 2),0.5);
		float camX = sin(t) * radius;
		float camZ = cos(t) * radius;
		lightPos.x = camX;
		lightPos.z = camZ;
	}
	if (key == GLFW_KEY_E) {
		t -= 0.1;
		float radius = pow(pow(lightPos.x, 2) + pow(lightPos.z, 2), 0.5);
		float camX = sin(t) * radius;
		float camZ = cos(t) * radius;
		lightPos.x = camX;
		lightPos.z = camZ;
	}

	//Add leafs/fruits
	if (key == GLFW_KEY_Z) {
		//Nleafs += 10;
		tree->edgeDetection(*tree, 10, 0);
	}
	if (key == GLFW_KEY_X) {
		//Nfruits += 10;
		tree->edgeDetection(*tree, 0, 10);
	}

	//Skinning
	if (key == GLFW_KEY_C) {
		if (stepZforSkeleton > - PI / 2) {
			stepZforSkeleton -= 0.01;
		}
		
	}
	if (key == GLFW_KEY_V) {
		if (stepZforSkeleton < PI / 2) {
			stepZforSkeleton += 0.01;
		}
	}
	if (key == GLFW_KEY_B) {
		if (stepXforSkeleton > -PI / 2) {
			stepXforSkeleton -= 0.01;
		}
	}
	if (key == GLFW_KEY_N) {
		if (stepXforSkeleton < PI / 2) {
			stepXforSkeleton += 0.01;
		}
	}

	//spring constant
	if (key == GLFW_KEY_Y) {
		K += 2.5;
	}
	//spring constant
	if (key == GLFW_KEY_U) {
		if (K > 0){
			K -= 2.5;
		}
		
	}

	//RENDER SCELETON
	if (key == GLFW_KEY_SPACE) {
		if (RENDER_SKELETON == false) RENDER_SKELETON = true;
		else RENDER_SKELETON = false;
	}


	//Force
	if (key == GLFW_KEY_1) {
		if (Fz > -100) {
			Fz -= 10;
		}
	}
	if (key == GLFW_KEY_2) {
		if (Fz < 100) {
			Fz += 10;
		}	
	}
	if (key == GLFW_KEY_3) {
		if (Fx > -100) {
			Fx -= 10;
		}	
	}
	if (key == GLFW_KEY_4) {
		if (Fx < 100) {
			Fx += 10;
		}
	}
	if (key == GLFW_KEY_5) {
		if (Fz > -100) {
			Fz -= 5;
		}
		if (Fx > -100) {
			Fx -= 5;
		}
	}
	if (key == GLFW_KEY_6){
		if (Fz < 100) {
			Fz += 5;
		}
		if (Fx < 100) {
			Fx += 5;
		}
	}
	if (key == GLFW_KEY_7){
		if (Fz > -100) {
			Fz -= 5;
		}
		if (Fx < 100) {
			Fx += 5;
		}
	}
	if (key == GLFW_KEY_8){
		if (Fz < 100) {
			Fz += 5;
		}
		if (Fx > -100) {
			Fx -= 5;
		}
	}

	

}


void printHelpMenu(void) {

	printf("****************** INSTRUCTIONS ******************");
	cout << "\n";
	cout << "KEY     FUNCTIONALITY\n";
	cout << "W       ";
	cout << "Transfer camera positions to the direction it is looking at \n";
	cout << "S       ";
	cout << "Transfer camera positions in the opposite direction it is looking at \n";
	cout << "D       ";
	cout << "Transfer camera positions to the right \n";
	cout << "A       ";
	cout << "Transfer camera positions to the left \n";
	cout << "UP      ";
	cout << "Reduce Field of View \n";
	cout << "DOWN    ";
	cout << "Increase Field of View \n";
	cout << "T       ";
	cout << "Transfer of light source to Y axis (upward) \n";
	cout << "G       ";
	cout << "Transfer of light source to Y axis (downward) \n";
	cout << "F       ";
	cout << "Transfer light source to X axis (to the left) \n";
	cout << "H       ";
	cout << "Transfer light source to X + axis (right) \n";
	cout << "Q       ";
	cout << "Move the light source in a clockwise circular path (fixed plane parallel to the XZ plane). \n";
	cout << "E       ";
	cout << "Movement of the light source on a left-hand circular track (fixed plane parallel to the XZ plane) \n";
	cout << "C       ";
	cout << "Counter-clockwise rotation of the model with respect to axis ZZ \n";
	cout << "V       ";
	cout << "Clockwise rotation of the model with respect to axis ZZ \n";
	cout << "B       ";
	cout << "Rotation of the model with respect to axis XX \n";
	cout << "N       ";
	cout << "Clockwise rotation of the model with respect to axis XX \n";
	cout << "G       ";
	cout << "Increase the number of leaves by 10 \n";
	cout << "X       ";
	cout << "Increase the number of fruits by 10 \n";
	cout << "Y       ";
	cout << "Increase the tree restoration constant to the initial state by 2.5 \n";
	cout << "U       ";
	cout << "Decrease the tree restoration constant to the initial state by 2.5 \n";
	cout << "Space   ";
	cout << "Show / Hide Model Skeleton \n";
	cout << "1-8     ";
	cout << "Increase of force (air) to various directions \n";
}