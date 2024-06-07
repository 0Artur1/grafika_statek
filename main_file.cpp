/*
Grafika Komputerowa i Wizualizacja
Projekt: Statek
Wykonali: Kewin Jankowski 156025, Artur Strzelecki 155294
*/

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "obj_to_opengl.hpp"


const float renderDistance = 140.0f;
const int gridSize = 500; //Probably optimal amount for 30 fps
const int texRepeat = 8; // controls repeating the water texture
const float gridSpacing = 0.8f; // water triangle grid spacing
const float shipAccel = 200.0f;
const float shipDecel = 100.0f;
float waveAmplitude = 0.4f;
std::vector<float>waterUvs;
std::vector<float>waterVertices;
std::vector<float>waterNormals;
std::vector<float>heightMap(gridSize* gridSize, 0.0f);
ShaderProgram* sp;


float shipRotateSpeed = 0; //ship x angular speed in radians
float shipSwaySpeed = 0.5; // ship sway
float shipMoveSpeed = 0.0f; // ship forward/backward motion
float cameraRotateSpeed = 0; // camera x axis speed
float cameraRotateAngle = 0; // camera x axis angle
float cameraMoveSpeed = 0; // camera y axis speed
float cameraMovePosition = 1; // camera y axis position
float waterSpeed = 8; // water movement speed
float waterOffset = 0; // water position offset (illusion of animation)
float aspectRatio = 1;
float shipTrackX = 100; // ship x position for detectOcean tracking
float shipTrackY = 0; // ship y position for detectOcean tracking
float shipTrackAngle = 0; // ship angle for detectOcean tracking
int shipSwayAnimate = 1; // ship sway bool
float oarAnimSpeed = 0; // oar animation speed
int oarAnimState = 0; // oar animation state
int oarAnimate = 1; // oar animation bool
float oarLoopTarget = PI / 12; // oar height
float icebergSpeed = PI / 45; // iceberg speed
int isMoving = 0;
int isTurning = 0;
float icebergAngle[4] = { 0.0f, PI / 3, -PI / 2.5, PI * 6 / 5.0 };
float cloudAngle[5] = { 0.4 * PI, 0.8 * PI, 1.2 * PI, 1.6 * PI, 2 * PI};
float cloudSpeed[5] = { 0.05f, 0.04f, 0.056f, 0.043f, 0.06f};

glm::mat4 modelShip = glm::mat4(1.0f);
glm::mat4 modelLighthouse = glm::mat4(1.0f);

std::vector< float > shipVertices;
std::vector< float > shipUvs;
std::vector< float > shipNormals;
std::vector<int> shipNumber_vertex;

std::vector< float > lighthouseVertices;
std::vector< float > lighthouseUvs;
std::vector< float > lighthouseNormals;
std::vector<int> lighthouseNumber_vertex;

std::vector< float > drVertices;
std::vector< float > drUvs;
std::vector< float > drNormals;
std::vector<int> drNumber_vertex;

std::vector< float > icebergVertices;
std::vector< float > icebergUvs;
std::vector< float > icebergNormals; // Won't be used at the moment.
std::vector<int> icebergNumber_vertex;

std::vector< float > cloudVertices;
std::vector< float > cloudUvs;
std::vector< float > cloudNormals; // Won't be used at the moment.
std::vector<int> cloudNumber_vertex;

GLuint tex_s1, tex_s2, tex_s3, tex_s4, tex_s5, tex_s6, tex_s7, tex_s8, tex_s9, tex_s10, tex_s11, tex_s12, tex_s13, tex_s14;
GLuint tex_water, tex_lighthouse, tex_iceberg, tex_cloud; //texture handle
GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	//Read into computers memory
	std::vector<unsigned char> image; //Allocate memory
	unsigned width, height; //Variables for image size
	//Read the image
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import to graphics card memory
	glGenTextures(1, &tex); //Initialize one handle
	glBindTexture(GL_TEXTURE_2D, tex); //Activate handle
	//Copy image to graphics cards memory represented by the active handle
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,
		GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	return tex;
}

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

glm::vec4 generateNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 U = p2 - p1;
	glm::vec3 V = p3 - p1;
	return glm::vec4(U.y * V.z - U.z * V.y, U.z * V.x - U.x * V.z, U.x * V.y - U.y * V.x, 0.0f);
}

void generateWater(bool partial)
{
	glm::vec4 n;
	glm::vec3 p1, p2, p3;

	for (int i = 0; i < gridSize; i++) for (int j = 0; j < gridSize; j++) heightMap[i * gridSize + j] = sin(0.1f * i) * cos(0.1f * j) * waveAmplitude;
	if (!partial) // should happen only upon program startup
	{
		waterVertices.clear();
		waterUvs.clear();
		for (int i = 0; i < gridSize - 1; i += 1) {
			for (int j = 0; j < gridSize - 1; ++j) { // each loop builds a square of 2 triangles
				// point (i, j+1) (TRIANGLE 1) A
				waterVertices.push_back(i * gridSpacing);
				waterVertices.push_back(heightMap[(i)*gridSize + (j + 1)]);
				waterVertices.push_back((j + 1) * gridSpacing);
				waterVertices.push_back(1.0f);
				// point (i+1, j) (TRIANGLE 1) B
				waterVertices.push_back((i + 1) * gridSpacing);
				waterVertices.push_back(heightMap[(i + 1) * gridSize + j]);
				waterVertices.push_back(j * gridSpacing);
				waterVertices.push_back(1.0f);
				// point (i, j) (TRIANGLE 1) C
				waterVertices.push_back(i * gridSpacing);
				waterVertices.push_back(heightMap[(i)*gridSize + j]);
				waterVertices.push_back(j * gridSpacing);
				waterVertices.push_back(1.0f);
				// point (i, j+1) (TRIANGLE 2) A
				waterVertices.push_back(i * gridSpacing);
				waterVertices.push_back(heightMap[(i)*gridSize + (j + 1)]);
				waterVertices.push_back((j + 1) * gridSpacing);
				waterVertices.push_back(1.0f);
				// point (i+1, j+1) (TRIANGLE 2) D
				waterVertices.push_back((i + 1) * gridSpacing);
				waterVertices.push_back(heightMap[(i + 1) * gridSize + (j + 1)]);
				waterVertices.push_back((j + 1) * gridSpacing);
				waterVertices.push_back(1.0f);
				// point (i+1, j) (TRIANGLE 2) B
				waterVertices.push_back((i + 1) * gridSpacing);
				waterVertices.push_back(heightMap[(i + 1) * gridSize + j]);
				waterVertices.push_back(j * gridSpacing);
				waterVertices.push_back(1.0f);
				// texture coordinates for triangle 1, triangle 2: A B C, A D B
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * i);
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * (j + 1));
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * (i + 1));
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * j);
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * i);
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * j);
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * i);
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * (j + 1));
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * (i + 1));
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * (j + 1));
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * (i + 1));
				waterUvs.push_back(1.0f / (gridSize / texRepeat) * j);
			}
		}
		waterNormals.clear();
		p2 = glm::vec3(waterVertices[0], waterVertices[1], waterVertices[2]);
		p3 = glm::vec3(waterVertices[4], waterVertices[5], waterVertices[6]);
		for (int i = 0; i < waterVertices.size() - 8; i += 4) // generate normals for water
		{
			p1 = p2; // the triangle points behave like a queue, this speeds up generation
			p2 = p3;
			p3 = glm::vec3(waterVertices[i + 8], waterVertices[i + 9], waterVertices[i + 10]);
			n = generateNormal(p1, p2, p3);
			waterNormals.push_back(n.x);
			waterNormals.push_back(n.y);
			waterNormals.push_back(n.z);
			waterNormals.push_back(n.w);
		}
	}
	else
	{
		for (int x = 1; x < waterVertices.size(); x += 4) // modify current vertices list
			waterVertices[x] = heightMap[int(waterVertices[x - 1] / gridSpacing) * gridSize + int(waterVertices[x + 1] / gridSpacing)];
		p2 = glm::vec3(waterVertices[0], waterVertices[1], waterVertices[2]);
		p3 = glm::vec3(waterVertices[4], waterVertices[5], waterVertices[6]);
		for (int i = 0; i < waterVertices.size() - 8; i += 4) // modify normals for water
		{
			p1 = p2;
			p2 = p3;
			p3 = glm::vec3(waterVertices[i + 8], waterVertices[i + 9], waterVertices[i + 10]);
			n = generateNormal(p1, p2, p3);
			waterNormals[i] = n.x;
			waterNormals[i + 1] = n.y;
			waterNormals[i + 2] = n.z; // n.w is not needed
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) isTurning = 1;
		if (key == GLFW_KEY_RIGHT) isTurning = -1;
		if (key == GLFW_KEY_UP) // oar animation, move
		{
			isMoving = 1;
			oarAnimSpeed = PI / 8;
		}
		if (key == GLFW_KEY_DOWN)
		{
			isMoving = -1;
			oarAnimSpeed = -PI / 8;
		}
		if (key == GLFW_KEY_Z) shipSwayAnimate = !shipSwayAnimate; // toggle animation
		if (key == GLFW_KEY_X) oarAnimate = !oarAnimate;
		if (key == GLFW_KEY_A) cameraRotateSpeed = PI / 4.0; // camera rotation
		if (key == GLFW_KEY_D) cameraRotateSpeed = -PI / 4.0;
		if (key == GLFW_KEY_W) cameraMoveSpeed = PI * 2.0; // raise or lower camera
		if (key == GLFW_KEY_S) cameraMoveSpeed = -PI * 1.0;
		if (key == GLFW_KEY_O) // change water amplitude
		{
			waveAmplitude += 0.1f;
			if (waveAmplitude > 1.95f) waveAmplitude = 2.0f;
			generateWater(true);
		}
		if (key == GLFW_KEY_L)
		{
			waveAmplitude -= 0.1f;
			if (waveAmplitude < 0.05f) waveAmplitude = 0.1f;
			generateWater(true);
		}
		if (key == GLFW_KEY_I) // water speed
		{
			waterSpeed += 1.0f;
			if (waterSpeed > 40.0f) waterSpeed = 40.0f;
		}
		if (key == GLFW_KEY_K)
		{
			waterSpeed -= 1.0f;
			if (waterSpeed < 1.0f) waterSpeed = 1.0f;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN)
		{
			isMoving = 0;
			oarAnimSpeed = 0;
		}
		if (key == GLFW_KEY_LEFT) isTurning = 0;
		if (key == GLFW_KEY_RIGHT) isTurning = 0;
		if (key == GLFW_KEY_A) cameraRotateSpeed = 0;
		if (key == GLFW_KEY_D) cameraRotateSpeed = 0;
		if (key == GLFW_KEY_W) cameraMoveSpeed = 0;
		if (key == GLFW_KEY_S) cameraMoveSpeed = 0;
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}


//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, at the program start************
	bool res;
	glClearColor(0.6, 0.8, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex_water = readTexture("rough-sea-4096x4096.png");
	tex_s1 = readTexture("T_Ship08_WoodPlain_01_Diffuse.png");
	tex_s2 = readTexture("T_Ship08_Rope_02_Diffuse.png");
	tex_s3 = readTexture("T_Ship08_Metal_Diffuse.png");
	tex_s4 = readTexture("T_Ship08_WoodPlain_02_Diffuse.png");
	tex_s5 = readTexture("T_Ship08_Planks_Diffuse.png");
	tex_s6 = readTexture("T_Ship08_WoodPlain_03_Diffuse.png");
	tex_s7 = readTexture("T_Ship08_Rope_01_Diffuse.png");
	tex_s8 = readTexture("T_Ship08_Flag_Diffuse.png");
	tex_s9 = readTexture("T_Ship08_Cannon_Diffuse.png");
	tex_s10 = readTexture("T_Ship08_CannonSupport_Diffuse.png");
	tex_s11 = readTexture("T_Ship08_CannonAxel_Diffuse.png");
	tex_s12 = readTexture("T_Ship08_CannonWheels_Diffuse.png");
	tex_s13 = readTexture("T_Ship08_CannonSides_Diffuse.png");
	tex_s14 = readTexture("T_Ship08_CannonRope_Diffuse.png");
	tex_lighthouse = readTexture("lighthouse_DefaultMaterial_BaseColor.png");
	tex_iceberg = readTexture("gora.png");
	tex_cloud = readTexture("chmura.png");
	res = another_parse_from_obj("Ship08.obj", shipVertices, shipUvs, shipNormals, shipNumber_vertex); // ship model
	modelShip = glm::scale(modelShip, glm::vec3(0.005f, 0.005f, 0.005f));
	modelShip = glm::translate(modelShip, glm::vec3(0.0f, -4.3f * 200, 100.0f * 50));
	modelShip = glm::rotate(modelShip, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	res = parse_lighthouse("lighthouse.obj", lighthouseVertices, lighthouseUvs, lighthouseNormals, lighthouseNumber_vertex); // lighthouse model
	modelLighthouse = glm::scale(modelLighthouse, glm::vec3(0.5f, 0.5f, 0.5f));
	modelLighthouse = glm::translate(modelLighthouse, glm::vec3(40.0f, -14.0f, 160.0f));
	res = parse_from_obj("gora_lodowa.obj", icebergVertices, icebergUvs, icebergNormals, icebergNumber_vertex);
	bool res2 = parse_from_obj("Clouds.obj", cloudVertices, cloudUvs, cloudNormals, cloudNumber_vertex);
	generateWater(false);
	std::cout << "Init complete\n";
}


//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
	glDeleteTextures(1, &tex_water);
	glDeleteTextures(1, &tex_s1);
	glDeleteTextures(1, &tex_s2);
	glDeleteTextures(1, &tex_s3);
	glDeleteTextures(1, &tex_s4);
	glDeleteTextures(1, &tex_s5);
	glDeleteTextures(1, &tex_s6);
	glDeleteTextures(1, &tex_s7);
	glDeleteTextures(1, &tex_s8);
	glDeleteTextures(1, &tex_s9);
	glDeleteTextures(1, &tex_s10);
	glDeleteTextures(1, &tex_s11);
	glDeleteTextures(1, &tex_s12);
	glDeleteTextures(1, &tex_s13);
	glDeleteTextures(1, &tex_s14);
	glDeleteTextures(1, &tex_lighthouse);
	glDeleteTextures(1, &tex_iceberg);
	glDeleteTextures(1, &tex_cloud);
}

float detectOcean() // for snapping ship position to the water level below its center point
{
	float conv_x = shipTrackX; // -1600 -> 1600    0 - 1000 j
	float conv_y = shipTrackY; // -1600 -> 1600    0 - 1000 i
	conv_x = (-conv_x) / 159.0 * 50 + 500 + waterOffset / gridSpacing;
	conv_y = (conv_y) / 159.0 * 50 + 500 + waterOffset / gridSpacing;
	conv_y = -sin(0.1f * conv_y + 0.5) * cos(0.1f * conv_x + 0.5) * waveAmplitude;
	return conv_y;
}

void drawIceberg()
{
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &icebergVertices[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &icebergNormals[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &icebergUvs[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_iceberg);
	glUniform1i(sp->u("textureMap0"), 0);
	glDrawArrays(GL_TRIANGLES, 0, icebergNumber_vertex[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));
}

void drawCloud()
{
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &cloudVertices[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &cloudNormals[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &cloudUvs[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_cloud);
	glUniform1i(sp->u("textureMap0"), 0);
	glDrawArrays(GL_TRIANGLES, 0, cloudNumber_vertex[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));
}

//Drawing procedure
void drawScene(GLFWwindow* window, float shipRotateAngle, float shipSwayAngle, float shipMoveDistance, float wateroffset, float oarAngle) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, cameraMovePosition, 0.0f),
		glm::vec3(5.0f * sin(cameraRotateAngle), cameraMovePosition > 0.0f ? 0.0f : cameraMovePosition * 0.9, 5.0f * cos(cameraRotateAngle)),
		glm::vec3(0.0f, 1.0f, 0.0f)); //compute view matrix
	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 1.0f, renderDistance); //compute projection matrix
	glm::mat4 M = glm::mat4(1.0f);

	sp->use();//activate shading program
	//Send parameters to graphics card
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	// light parameters
	glUniform4f(sp->u("lp1"), 0.0f, 50.0f, 0.0f, 1.0f); // position
	glUniform4f(sp->u("lp2"), 20.0f, 12.0f, 80.0f, 1.0f);
	glUniform1i(sp->u("lpow1"), 25); // the light's Phong coefficient
	glUniform1i(sp->u("lpow2"), 250);
	glUniform4f(sp->u("ks1"), 1.0f, 1.0f, 1.0f, 1.0f); // reflection color
	glUniform4f(sp->u("ks2"), 1.0f, 1.0f, 0.5f, 1.0f);



	//// water
	glm::mat4 Mw = glm::translate(M, glm::vec3(wateroffset - gridSize * gridSpacing * 0.5, -5.0f, wateroffset - gridSize * gridSpacing * 0.5));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mw));
	glEnableVertexAttribArray(sp->a("vertex")); // pass vertices, normals, uvs
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, GL_FALSE, 0, &waterVertices[0]);
	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, GL_FALSE, 0, &waterNormals[0]);
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &waterUvs[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_water);
	glUniform1i(sp->u("textureMap0"), 0);
	glDrawArrays(GL_TRIANGLES, 0, waterVertices.size() / 4); //Draw the object
	glDisableVertexAttribArray(sp->a("texCoord0")); // finish drawing
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));



	//// ship oars
	//M = glm::mat4(1.0f);
	glm::mat4 Ms = modelShip;
	Ms = glm::rotate(Ms, shipRotateAngle, glm::vec3(0.0f, 1.0f, 0.0f)); //Compute model matrix
	Ms = glm::translate(Ms, glm::vec3(0.0f, 0.0f, shipMoveDistance));
	modelShip = Ms;
	Ms = glm::translate(Ms, glm::vec3(0.0f, detectOcean() * 200, 0.0f));

	if (shipSwayAnimate) Ms = glm::rotate(Ms, shipSwayAngle, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 Mo = Ms; //Mo=Oar rotation matrix
	if (oarAnimate)
	{
		Mo = glm::rotate(Mo, oarLoopTarget, glm::vec3(0.0f, -1.0f, -1.0f));
		Mo = glm::rotate(Mo, 2 * oarLoopTarget / 3, glm::vec3(0.0f, 1.0f, 0.0f));
		if (oarAnimState >= 1) Mo = glm::rotate(Mo, oarLoopTarget, glm::vec3(0.0f, 1.0f, 1.0f));
		if (oarAnimState >= 2) Mo = glm::rotate(Mo, oarLoopTarget, glm::vec3(0.0f, -1.0f, 1.0f));
		if (oarAnimState >= 3) Mo = glm::rotate(Mo, oarLoopTarget, glm::vec3(0.0f, -1.0f, -1.0f));
		if (oarAnimState == 0) Mo = glm::rotate(Mo, oarAngle, glm::vec3(0.0f, 1.0f, 1.0f));
		else if (oarAnimState == 1) Mo = glm::rotate(Mo, oarAngle, glm::vec3(0.0f, -1.0f, 1.0f));
		else if (oarAnimState == 2) Mo = glm::rotate(Mo, oarAngle, glm::vec3(0.0f, -1.0f, -1.0f));
		else if (oarAnimState == 3) Mo = glm::rotate(Mo, oarAngle, glm::vec3(0.0f, 1.0f, -1.0f));
	}

	int vertex_sum = 0;
	GLuint shipTextures[] = { tex_s1,tex_s2,tex_s3,tex_s4,tex_s5,tex_s6,tex_s7,tex_s8,tex_s9,tex_s10,tex_s11,tex_s12,tex_s13,tex_s14 };
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms));
	for (int j = 0; j < 14; j++)
	{
		glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &shipVertices[vertex_sum * 4]); //Specify source of the data for the attribute vertex
		glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &shipNormals[vertex_sum * 4]); //Specify source of the data for the attribute vertex
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &shipUvs[vertex_sum * 2]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shipTextures[j]);
		glUniform1i(sp->u("textureMap0"), 0);
		glDrawArrays(GL_TRIANGLES, 0, shipNumber_vertex[j]); //Draw the object
		glDisableVertexAttribArray(sp->a("texCoord0"));
		glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
		glDisableVertexAttribArray(sp->a("normal"));
		vertex_sum = vertex_sum + shipNumber_vertex[j];
	}

	//// ship
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mo));
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &shipVertices[vertex_sum * 4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &shipNormals[vertex_sum * 4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &shipUvs[vertex_sum * 2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_s4);
	glUniform1i(sp->u("textureMap0"), 0);
	glDrawArrays(GL_TRIANGLES, 0, shipNumber_vertex[14]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));


	//// lighthouse
	glm::mat4 Ml = modelLighthouse;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ml)); // lighthouse
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &lighthouseVertices[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &lighthouseNormals[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &lighthouseUvs[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_lighthouse);
	glUniform1i(sp->u("textureMap0"), 0);
	glDrawArrays(GL_TRIANGLES, 0, lighthouseNumber_vertex[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));


	//// icebergs: iceberg 1
	glm::mat4 Mg = M;
	Mg = glm::translate(Mg, glm::vec3(25.0f, -9.0f, 30.0f));
	Mg = glm::scale(Mg, glm::vec3(8.0f, 8.0f, 8.0f));
	Mg = glm::rotate(Mg, icebergAngle[0], glm::vec3(0.0f, 1.0f, 0.0f));
	Mg = glm::translate(Mg, glm::vec3(-icebergAngle[0] / 4, 0.0f, -icebergAngle[0] / 2));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mg));
	drawIceberg();
	//// iceberg 2
	glm::mat4 Mg2 = M;
	Mg2 = glm::translate(Mg2, glm::vec3(-50.0f, -9.0f, 50.0f));
	Mg2 = glm::scale(Mg2, glm::vec3(8.0f, 8.0f, 8.0f));
	Mg2 = glm::rotate(Mg2, icebergAngle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	Mg2 = glm::translate(Mg2, glm::vec3(-icebergAngle[1] / 4, 0.0f, -icebergAngle[1] / 2));
	Mg2 = glm::rotate(Mg2, -0.1f * PI, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mg2));
	drawIceberg();
	//// iceberg 3
	glm::mat4 Mg3 = M;
	Mg3 = glm::translate(Mg3, glm::vec3(40.0f, -9.0f, -25.0f));
	Mg3 = glm::scale(Mg3, glm::vec3(8.0f, 8.0f, 8.0f));
	Mg3 = glm::rotate(Mg3, icebergAngle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	Mg3 = glm::translate(Mg3, glm::vec3(-icebergAngle[2] / 4, 0.0f, -icebergAngle[2] / 2));
	Mg3 = glm::rotate(Mg3, 0.03f * PI, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mg3));
	drawIceberg();
	//// iceberg 4
	glm::mat4 Mg4 = M;
	Mg4 = glm::translate(Mg4, glm::vec3(-5.0f, -9.0f, -70.0f));
	Mg4 = glm::scale(Mg4, glm::vec3(8.0f, 8.0f, 8.0f));
	Mg4 = glm::rotate(Mg4, icebergAngle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	Mg4 = glm::translate(Mg4, glm::vec3(-icebergAngle[3] / 4, 0.0f, -icebergAngle[3] / 2));
	Mg4 = glm::rotate(Mg4, 0.1f * PI, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mg4));
	drawIceberg();


	//// clouds: cloud 1
	glm::mat4 Mch = M;
	Mch = glm::rotate(Mch, cloudAngle[0], glm::vec3(0.0f, 1.0f, 0.0f));
	Mch = glm::translate(Mch, glm::vec3(0.0f, 20.0f, 110.0f));
	Mch = glm::scale(Mch, glm::vec3(3.0f, 3.0f, 3.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch));
	drawCloud();
	//// cloud 2
	glm::mat4 Mch2 = M;
	Mch2 = glm::rotate(Mch2, cloudAngle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	Mch2 = glm::translate(Mch2, glm::vec3(0.0f, 18.0f, 120.0f));
	Mch2 = glm::scale(Mch2, glm::vec3(2.5f, 2.5f, 2.5f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch2));
	drawCloud();
	//// cloud 3
	glm::mat4 Mch3 = M;
	Mch3 = glm::rotate(Mch3, cloudAngle[2], glm::vec3(0.0f, 1.0f, 0.0f));
	Mch3 = glm::translate(Mch3, glm::vec3(0.0f, 22.0f, 100.0f));
	Mch3 = glm::scale(Mch3, glm::vec3(3.5f, 3.5f, 3.5f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch3));
	drawCloud();
	//// cloud 4
	glm::mat4 Mch4 = M;
	Mch4 = glm::rotate(Mch4, cloudAngle[3], glm::vec3(0.0f, 1.0f, 0.0f));
	Mch4 = glm::translate(Mch4, glm::vec3(0.0f, 21.0f, 105.0f));
	Mch4 = glm::scale(Mch4, glm::vec3(3.25f, 3.25f, 3.25f));
	Mch4 = glm::rotate(Mch4, PI, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch4));
	drawCloud();
	//// cloud 5
	glm::mat4 Mch5 = M;
	Mch5 = glm::rotate(Mch5, cloudAngle[4], glm::vec3(0.0f, 1.0f, 0.0f));
	Mch5 = glm::translate(Mch5, glm::vec3(0.0f, 19.0f, 115.0f));
	Mch5 = glm::scale(Mch5, glm::vec3(2.75f, 2.75f, 2.75f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch5));
	drawCloud();



	glfwSwapBuffers(window); //Copy back buffer to front buffer
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure


	float shipRotateAngle = 0; // ship rotation angle, x axis
	float shipSwayAngle = 0;
	float shipMoveDistance = 0;
	float waterTime = 0;
	float oarAngle = 0;
	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		cameraRotateAngle += cameraRotateSpeed * glfwGetTime();
		cameraMovePosition += cameraMoveSpeed * glfwGetTime();
		if (cameraMovePosition < -3) cameraMovePosition = -3; // position limits on camera Y position
		if (cameraMovePosition > 132) cameraMovePosition = 132;


		if (isMoving == 0 && shipMoveSpeed > 0) shipMoveSpeed = shipMoveSpeed - shipDecel * glfwGetTime() < 0 ? 0 : shipMoveSpeed - shipDecel * glfwGetTime();
		if (isMoving == 0 && shipMoveSpeed < 0) shipMoveSpeed = shipMoveSpeed + shipDecel * glfwGetTime() > 0 ? 0 : shipMoveSpeed + shipDecel * glfwGetTime();
		if (isMoving == 1) shipMoveSpeed = shipMoveSpeed + shipAccel * glfwGetTime();
		if (isMoving == -1) shipMoveSpeed = shipMoveSpeed - shipDecel * glfwGetTime();
		if (shipMoveSpeed > PI * 200) shipMoveSpeed = PI * 200;
		if (shipMoveSpeed < -PI * 200) shipMoveSpeed = -PI * 200;
		if (isTurning != 0) shipRotateSpeed = isTurning * shipMoveSpeed / 1600.0; // turning speed depends on forward momentum
		else shipRotateSpeed = 0;
		shipRotateAngle = shipRotateSpeed * glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		shipMoveDistance = shipMoveSpeed * glfwGetTime();
		shipTrackAngle += shipRotateAngle;
		shipTrackX -= shipMoveSpeed / 50.0 * sin(shipTrackAngle) * glfwGetTime();
		shipTrackY -= shipMoveSpeed / 50.0 * cos(shipTrackAngle) * glfwGetTime();


		for (int i = 0; i < 4; i++) icebergAngle[i] += icebergSpeed * glfwGetTime();
		for (int i = 0; i < 5; i++) cloudAngle[i] += cloudSpeed[i] * glfwGetTime();


		waterTime = glfwGetTime(); //water relies on illusion of movement, the entire object moves in a loop so that vertices need not be modified
		waterOffset += waterSpeed * waterTime;
		waterTime *= waterSpeed * 9.5 / gridSize;
		for (float& x : waterUvs) x += waterTime; // illusion of movement, edits water uvs
		while (waterOffset > gridSize / 40.0) { // reset position once animatoin loop is done
			waterOffset -= gridSize / 20.0;
			for (float& x : waterUvs) x -= 0.504;
		}

		shipSwayAngle += shipSwaySpeed * glfwGetTime();
		if (oarAnimate)
		{
			oarAngle += oarAnimSpeed * glfwGetTime();
			if (oarAngle > oarLoopTarget) {
				oarAngle = 0;
				oarAnimState = (++oarAnimState) % 4;
			}
			if (oarAngle < 0) {
				oarAngle = oarLoopTarget;
				oarAnimState--;
				if (oarAnimState < 0) oarAnimState = 3;
			}
		}

		glfwSetTime(0); //Zero the timer
		drawScene(window, shipRotateAngle, sin(shipSwayAngle) / 9.0, shipMoveDistance, waterOffset, oarAngle); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
