#include "skeleton.h"
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
GLuint skeletonshaderProgram;
GLuint skeletondiffuceColorSampler;
GLuint skeletonModelMatrixLocation;
GLuint skeletonMLocation;
GLuint skeletonTexture;
GLuint skeletonVAO;
GLuint skeletonVerticiesVBO, skeletonUVVBO, skeletonNormalsVBO;

Drawable *segment;
float stepZ;
float stepX;
GLuint useSkinningLocation;
GLuint boneTransformationsLocation;
#define PI 3.141592653589793238463
vector<vec3> segmentVertices;

Skeleton::Skeleton(GLuint skeletonshaderProgram)
{
	glUseProgram(skeletonshaderProgram);

	// get pointers to uniforms
	skeletonModelMatrixLocation = glGetUniformLocation(skeletonshaderProgram, "M");
	useSkinningLocation = glGetUniformLocation(skeletonshaderProgram, "useSkinning");
	boneTransformationsLocation = glGetUniformLocation(skeletonshaderProgram, "boneTransformations");

	segmentVertices = {
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	};
	segment = new Drawable(segmentVertices);

	// surface coordinates (legacy initialization because we would
	// like to provide some extra attributes for the skinning)

	glGenVertexArrays(1, &skeletonVAO);
	glBindVertexArray(skeletonVAO);

	/* v: vertex, s: segment
	*  v1--s1--v2--s2--v3
	*  |                |
	*  s6              s3
	*  |                |
	*  v6--s5--v5--s4--v4
	*/
	static const GLfloat surfaceVerteces[] = {
		-1.0f, 0.0f, 0.0f,  // segment 1
		-1.0f, 5.0f, 0.0f,

		-1.0f, 5.0f, 0.0f,   // segment 2
		-1.0f, 10.0f, 0.0f,

		-1.0f, 10.0f, 0.0f,   // segment 3
		-1.0f, 15.0f, 0.0f,

		-1.0f, 15.0f, 0.0f,  // segment 4
		0.0f, 15.0f, 0.0f,

		0.0f, 15.0f, 0.0f,  // segment 5
		1.0f, 15.0f, 0.0f,

		1.0f, 15.0f, 0.0f, // segment 6
		1.0f, 10.0f, 0.0f,

		1.0f, 10.0f, 0.0f,  // segment 7
		1.0f, 5.0f, 0.0f,

		1.0f, 5.0f, 0.0f,  // segment 8
		1.0f, 0.0f, 0.0f,
	};
	glGenBuffers(1, &skeletonVerticiesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skeletonVerticiesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(surfaceVerteces), surfaceVerteces,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

}
Skeleton::~Skeleton() {
}
float Fsx = 0;
float Fsz = 0;

mat4 previousModelMatrix = glm::mat4(0.0);
vec4 segm_position=vec4(0,0,0,0);
float SFx;
float SFz;
float angleX;
float angleZ;
float FzNorm;
float FxNorm;



void Skeleton::draw(GLuint skeletonshaderProgram, float Z, float X, bool RENDER_SKELETON,float Fz,float Fx) {


	//Normalize Fz anf Fx
	FzNorm = PI * Fz / 200; //F between -100 to 100 and angle from -pi/2 to pi/2
	FxNorm = PI * Fx / 200;

	//Move the skeleton
	angleX = X;
	angleZ = Z;

	//Power
	angleX -= FxNorm;
	angleZ += FzNorm;


	glUseProgram(skeletonshaderProgram);
	glUniform1i(useSkinningLocation, 0);

	segment->bind();

	// first segment
	mat4 jointLocal0 = rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 bodyWorld0 = mat4(1.0) * jointLocal0;		//transformation matrtix
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld0[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);
	//segment->draw(GL_POINTS);

	//second sagement
	mat4 jointLocal1 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld1 = bodyWorld0 * jointLocal1;		//koitaei mono ton proigoumeno
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld1[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//third sagement
	mat4 jointLocal2 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld2 = bodyWorld1 * jointLocal2;		//koitaei mono ton proigoumeno
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld2[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//4th sagement
	mat4 jointLocal3 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld3 = bodyWorld2 * jointLocal3;		//koitaei mono ton proigoumeno
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld3[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//5th sagement
	mat4 jointLocal4 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld4 = bodyWorld3 * jointLocal4;		//koitaei mono ton proigoumeno
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld4[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);


	//6th sagement
	mat4 jointLocal5 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld5 = bodyWorld4 * jointLocal5;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld5[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//7th sagement
	mat4 jointLocal6 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld6 = bodyWorld5 * jointLocal6;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld6[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//8th sagement
	mat4 jointLocal7 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld7 = bodyWorld6 * jointLocal7;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld7[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//9th sagement
	mat4 jointLocal8 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld8 = bodyWorld7 * jointLocal8;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld8[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//10th sagement
	mat4 jointLocal9 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld9 = bodyWorld8 * jointLocal9;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld9[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//11th sagement
	mat4 jointLocal10 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld10 = bodyWorld9 * jointLocal10;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld10[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//12th sagement
	mat4 jointLocal11 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld11 = bodyWorld10 * jointLocal11;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld11[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//13th sagement
	mat4 jointLocal12 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld12 = bodyWorld11 * jointLocal12;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld12[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//14th sagement
	mat4 jointLocal13 = translate(mat4(), vec3(0.0, 1.0, 0))  *rotate(mat4(), radians(2.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(2.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld13 = bodyWorld12 * jointLocal13;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld13[0][0]);
	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);

	//15th sagement
	mat4 jointLocal14 = translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(4.0f * sin(angleZ)), vec3(0, 0, 1)) * rotate(mat4(), radians(4.0f * sin(angleX)), vec3(1, 0, 0));
	mat4 bodyWorld14 = bodyWorld13 * jointLocal14;
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &bodyWorld14[0][0]);


	segm_position = vec4(segment->indexedVertices[1].x, segment->indexedVertices[1].y, segment->indexedVertices[1].z, 1);
	segm_position = bodyWorld14 * segm_position;


	// draw segment
	if (RENDER_SKELETON) segment->draw(GL_LINES);
	 
	//Calculate matrix for spring force
	previousModelMatrix += jointLocal14;

	// render the skin
	glBindVertexArray(skeletonVAO);

	// no need to use the model matrix, because each vertex will be
	// positioned using the linear blend skinning (LBS) method
	mat4 modelSurf = mat4(1);
	glUniformMatrix4fv(skeletonModelMatrixLocation, 1, GL_FALSE, &modelSurf[0][0]);


	// define the binding transformations (B0, B14)
	// The binding is the inverse of the body's world transformation
	// at the binding pose
	mat4 bodyWorld000 = mat4(1.0) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B0 = glm::inverse(bodyWorld000);

	mat4 bodyWorld001 = bodyWorld000 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B1 = glm::inverse(bodyWorld001);

	mat4 bodyWorld002 = bodyWorld001 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B2 = glm::inverse(bodyWorld002);

	mat4 bodyWorld003 = bodyWorld002 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B3 = glm::inverse(bodyWorld003);

	mat4 bodyWorld004 = bodyWorld003 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B4 = glm::inverse(bodyWorld004);

	mat4 bodyWorld005 = bodyWorld004 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B5 = glm::inverse(bodyWorld005);

	mat4 bodyWorld006 = bodyWorld005 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B6 = glm::inverse(bodyWorld006);

	mat4 bodyWorld007 = bodyWorld006 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B7 = glm::inverse(bodyWorld007);

	mat4 bodyWorld008 = bodyWorld007 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B8 = glm::inverse(bodyWorld008);

	mat4 bodyWorld009 = bodyWorld008 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B9 = glm::inverse(bodyWorld009);

	mat4 bodyWorld010 = bodyWorld009 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B10 = glm::inverse(bodyWorld010);

	mat4 bodyWorld011 = bodyWorld010 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B11 = glm::inverse(bodyWorld011);

	mat4 bodyWorld012 = bodyWorld011 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B12 = glm::inverse(bodyWorld012);

	mat4 bodyWorld013 = bodyWorld012 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B13 = glm::inverse(bodyWorld013);

	mat4 bodyWorld014 = bodyWorld013 * translate(mat4(), vec3(0.0, 1.0, 0)) * rotate(mat4(), radians(0.0f), vec3(0, 0, 1));
	mat4 B14 = glm::inverse(bodyWorld014);


	
	// define the bone transformations T and send them to the GPU
	// using uniform variables
	// Note that we can send an array of mat4, using glUniformMatrix4fv
	// by providing the size of the array in the second argument. Since
	// we have an array of 2D arrays T is 3D.
	vector<mat4> T = {
		bodyWorld0 * B0,
		bodyWorld1 * B1,
		bodyWorld2 * B2,
		bodyWorld3 * B3,
		bodyWorld4 * B4,
		bodyWorld5 * B5,
		bodyWorld6 * B6,
		bodyWorld7 * B7,
		bodyWorld8 * B8,
		bodyWorld9 * B9,
		bodyWorld10 * B10,
		bodyWorld11 * B11,
		bodyWorld12 * B12,
		bodyWorld13 * B13,
		bodyWorld14 * B14,
	};
	glUniformMatrix4fv(boneTransformationsLocation, T.size(),
		GL_FALSE, &T[0][0][0]);

	//enable the skinning "1"!
	glUniform1i(useSkinningLocation, 1);

}



void Skeleton::update(float t, float dt) {
}

void Skeleton::free() {
	glDeleteBuffers(1, &skeletonVerticiesVBO);
	glDeleteBuffers(1, &skeletonUVVBO);
	glDeleteBuffers(1, &skeletonNormalsVBO);
	glDeleteVertexArrays(1, &skeletonVAO);
}