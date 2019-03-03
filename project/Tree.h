#ifndef TREE_H
#define TREE_H

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

class Tree {
public:
    //Drawable* sphere;
    float r;
	vec3 treePosition;
    glm::mat4 treeModelMatrix;
	//Tree
	//GLuint treeVAO, triangleVAO;
	//GLuint treeVerticiesVBO, treeUVVBO, treeNormalsVBO;

	std::vector<vec3> treeVertices;
	std::vector<vec3> treeNormals;
	std::vector<vec2> treeUVs;
	GLuint Texture;

	std::vector<vec3> leafsArray;
	std::vector<vec3> fruitsArray;

    Tree(GLuint treeshaderProgram);
    ~Tree();

    void draw(GLuint treeshaderProgram);
    void genBuffersForShadow(GLuint treeshaderProgram);
	void free();
	void edgeDetectionForLeaf(Tree tr);
	void edgeDetectionForFruit(Tree tr);
	void edgeDetection(Tree tr, int Nleaf, int Nfrouts);
	void estimateTranformendVertices(Tree tr, int Nleaf, int Nfrouts);
	void verticesDetection( int Nleaf, int Nfrouts);
};

#endif