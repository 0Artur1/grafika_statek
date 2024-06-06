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
#include "myCube.h"
#include "myTeapot.h"
#include "obj_to_opengl.hpp"


const int gridSize = 500; //Probably optimal amount for 30 fps
const int texRepeat = 8; // controls repeating the water texture
const float gridSpacing = 0.8f;
float waveAmplitude = 0.4f;
std::vector<float>water_textures;
std::vector<float>water_vertices;
std::vector<float>water_normals;
std::vector<float> heightMap(gridSize* gridSize, 0.0f);
ShaderProgram* sp;


float speed_x = 0; //angular speed in radians
float speed_y = 0; //angular speed in radians
float speed_k = PI * 4;
float speed_f = 0.5;
float speed_p = 0;
float speed_cx = 0;
float angle_cx = 0;
float speed_cy = 0;
float angle_cy = 1;
float speed_water = 8;
float wateroffset = 0;
float aspectRatio = 1;
float pos_x = 100;
float pos_y = 0;
float pos_angle = 0;
int kolysanie = 1;
float speed_l = PI / 8;
int stan = 0;
int wioslowanie = 0;
float wioslo_height = PI / 12;
float speed_gora;

glm::mat4 modelShip = glm::mat4(1.0f);
glm::mat4 modelLighthouse = glm::mat4(1.0f);

std::vector< float > vertices;
std::vector< float > uvs;
std::vector< float > normals;
std::vector<int> number_vertex;

std::vector< float > LHvertices;
std::vector< float > LHuvs;
std::vector< float > LHnormals;
std::vector<int> LHnumber_vertex;

std::vector< float > DRvertices;
std::vector< float > DRuvs;
std::vector< float > DRnormals;
std::vector<int> DRnumber_vertex;

std::vector< float > vertices_gora;
std::vector< float > uvs_gora;
std::vector< float > normals_gora; // Won't be used at the moment.
std::vector<int> number_vertex_gora;

std::vector< float > vertices_chmura;
std::vector< float > uvs_chmura;
std::vector< float > normals_chmura; // Won't be used at the moment.
std::vector<int> number_vertex_chmura;

GLuint tex_s1, tex_s2, tex_s3, tex_s4, tex_s5, tex_s6, tex_s7, tex_s8, tex_s9, tex_s10, tex_s11, tex_s12, tex_s13, tex_s14;
GLuint texWater, texShip1, texShip2, texLighthouse, tex_gora, tex_chmura; //texture handle
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
		water_vertices.clear();
		water_textures.clear();
		for (int i = 0; i < gridSize - 1; i += 1) {
			for (int j = 0; j < gridSize - 1; ++j) { // each loop builds a square of 2 triangles
				// point (i, j+1) (TRIANGLE 1) A
				water_vertices.push_back(i * gridSpacing);
				water_vertices.push_back(heightMap[(i)*gridSize + (j + 1)]);
				water_vertices.push_back((j + 1) * gridSpacing);
				water_vertices.push_back(1.0f);
				// point (i+1, j) (TRIANGLE 1) B
				water_vertices.push_back((i + 1) * gridSpacing);
				water_vertices.push_back(heightMap[(i + 1) * gridSize + j]);
				water_vertices.push_back(j * gridSpacing);
				water_vertices.push_back(1.0f);
				// point (i, j) (TRIANGLE 1) C
				water_vertices.push_back(i * gridSpacing);
				water_vertices.push_back(heightMap[(i)*gridSize + j]);
				water_vertices.push_back(j * gridSpacing);
				water_vertices.push_back(1.0f);
				// point (i, j+1) (TRIANGLE 2) A
				water_vertices.push_back(i * gridSpacing);
				water_vertices.push_back(heightMap[(i)*gridSize + (j + 1)]);
				water_vertices.push_back((j + 1) * gridSpacing);
				water_vertices.push_back(1.0f);
				// point (i+1, j+1) (TRIANGLE 2) D
				water_vertices.push_back((i + 1) * gridSpacing);
				water_vertices.push_back(heightMap[(i + 1) * gridSize + (j + 1)]);
				water_vertices.push_back((j + 1) * gridSpacing);
				water_vertices.push_back(1.0f);
				// point (i+1, j) (TRIANGLE 2) B
				water_vertices.push_back((i + 1) * gridSpacing);
				water_vertices.push_back(heightMap[(i + 1) * gridSize + j]);
				water_vertices.push_back(j * gridSpacing);
				water_vertices.push_back(1.0f);
				// texture coordinates for triangle 1, triangle 2: A B C, A D B
				water_textures.push_back(1.0f / (gridSize / texRepeat) * i);
				water_textures.push_back(1.0f / (gridSize / texRepeat) * (j + 1));
				water_textures.push_back(1.0f / (gridSize / texRepeat) * (i + 1));
				water_textures.push_back(1.0f / (gridSize / texRepeat) * j);
				water_textures.push_back(1.0f / (gridSize / texRepeat) * i);
				water_textures.push_back(1.0f / (gridSize / texRepeat) * j);
				water_textures.push_back(1.0f / (gridSize / texRepeat) * i);
				water_textures.push_back(1.0f / (gridSize / texRepeat) * (j + 1));
				water_textures.push_back(1.0f / (gridSize / texRepeat) * (i + 1));
				water_textures.push_back(1.0f / (gridSize / texRepeat) * (j + 1));
				water_textures.push_back(1.0f / (gridSize / texRepeat) * (i + 1));
				water_textures.push_back(1.0f / (gridSize / texRepeat) * j);
			}
		}
		water_normals.clear();
		p2 = glm::vec3(water_vertices[0], water_vertices[1], water_vertices[2]);
		p3 = glm::vec3(water_vertices[4], water_vertices[5], water_vertices[6]);
		for (int i = 0; i < water_vertices.size() - 8; i += 4) // generate normals for water
		{
			p1 = p2; // the triangle points behave like a queue, this speeds up generation
			p2 = p3;
			p3 = glm::vec3(water_vertices[i + 8], water_vertices[i + 9], water_vertices[i + 10]);
			n = generateNormal(p1, p2, p3);
			water_normals.push_back(n.x);
			water_normals.push_back(n.y);
			water_normals.push_back(n.z);
			water_normals.push_back(n.w);
		}
	}
	else
	{
		for (int x = 1; x < water_vertices.size(); x += 4) // modify current vertices list
			water_vertices[x] = heightMap[int(water_vertices[x - 1] / gridSpacing) * gridSize + int(water_vertices[x + 1] / gridSpacing)];
		p2 = glm::vec3(water_vertices[0], water_vertices[1], water_vertices[2]);
		p3 = glm::vec3(water_vertices[4], water_vertices[5], water_vertices[6]);
		for (int i = 0; i < water_vertices.size() - 8; i += 4) // modify normals for water
		{
			p1 = p2;
			p2 = p3;
			p3 = glm::vec3(water_vertices[i + 8], water_vertices[i + 9], water_vertices[i + 10]);
			n = generateNormal(p1, p2, p3);
			water_normals[i] = n.x;
			water_normals[i + 1] = n.y;
			water_normals[i + 2] = n.z; // n.w is not needed
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = PI / 8 * 4; // turn around
		if (key == GLFW_KEY_RIGHT) speed_x = -PI / 8 * 4;
		if (key == GLFW_KEY_UP) // oar animation, move
		{
			speed_p = PI * 200;
			wioslowanie = 1;
		}
		if (key == GLFW_KEY_DOWN)
		{
			speed_p = -PI * 200;
			wioslowanie = 2;
		}
		if (key == GLFW_KEY_Z) kolysanie = ~kolysanie; // toggle animation
		if (key == GLFW_KEY_X) wioslowanie = ~wioslowanie;
		if (key == GLFW_KEY_A) speed_cx = PI / 2.0; // camera rotation
		if (key == GLFW_KEY_D) speed_cx = -PI / 2.0;
		if (key == GLFW_KEY_W) speed_cy = PI * 2.0; // raise or lower camera
		if (key == GLFW_KEY_S) speed_cy = -PI * 2.0;
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
			speed_water += 1.0f;
			if (speed_water > 40.0f) speed_water = 40.0f;
		}
		if (key == GLFW_KEY_K)
		{
			speed_water -= 1.0f;
			if (speed_water < 1.0f) speed_water = 1.0f;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP)
		{
			speed_p = 0;
			wioslowanie = 0;
		}
		if (key == GLFW_KEY_DOWN)
		{
			speed_p = 0;
			wioslowanie = 0;
		}
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_A) speed_cx = 0;
		if (key == GLFW_KEY_D) speed_cx = 0;
		if (key == GLFW_KEY_W) speed_cy = 0;
		if (key == GLFW_KEY_S) speed_cy = 0;
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
	texWater = readTexture("morze9.png");
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
	texLighthouse = readTexture("lighthouse_DefaultMaterial_BaseColor.png");
	tex_gora = readTexture("gora.png");
	tex_chmura = readTexture("chmura.png");
	res = another_parse_from_obj("Ship08.obj", vertices, uvs, normals, number_vertex); // ship model
	modelShip = glm::scale(modelShip, glm::vec3(0.005f, 0.005f, 0.005f));
	modelShip = glm::translate(modelShip, glm::vec3(0.0f, -4.3f * 200, 100.0f * 50));
	modelShip = glm::rotate(modelShip, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	res = parse_lighthouse("lighthouse.obj", LHvertices, LHuvs, LHnormals, LHnumber_vertex); // lighthouse model
	modelLighthouse = glm::scale(modelLighthouse, glm::vec3(0.5f, 0.5f, 0.5f));
	modelLighthouse = glm::translate(modelLighthouse, glm::vec3(40.0f, -14.0f, 160.0f));
	res = parse_from_obj("gora_lodowa.obj", vertices_gora, uvs_gora, normals_gora, number_vertex_gora);
	bool res2 = parse_from_obj("Clouds.obj", vertices_chmura, uvs_chmura, normals_chmura, number_vertex_chmura);
	generateWater(false);
	std::cout << "Init complete\n";
}


//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
	glDeleteTextures(1, &texWater);
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
	glDeleteTextures(1, &texShip1);
	glDeleteTextures(1, &texShip2);
	glDeleteTextures(1, &texLighthouse);
	glDeleteTextures(1, &tex_gora);
	glDeleteTextures(1, &tex_chmura);
}

void drawWater() {
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, GL_FALSE, 0, &water_vertices[0]);
	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, GL_FALSE, 0, &water_normals[0]);
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &water_textures[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWater);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, water_vertices.size() / 4);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("normal"));
}

float detectOcean()
{
	float conv_x = pos_x; // -1600 -> 1600    0 - 1000 j
	float conv_y = pos_y; // -1600 -> 1600    0 - 1000 i
	conv_x = (-conv_x) / 159.0 * 50 + 500 + wateroffset / gridSpacing;
	conv_y = (conv_y) / 159.0 * 50 + 500 + wateroffset / gridSpacing;
	conv_y = -sin(0.1f * conv_y + 0.5) * cos(0.1f * conv_x + 0.5) * waveAmplitude;
	return conv_y; // Snap ship position to the water level below its center point
}

//Drawing procedure
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_k, float angle_f, float angle_p, float wateroffset, float angle_wioslo, float angle_gora) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, angle_cy, 0.0f),
		glm::vec3(5.0f * sin(angle_cx), angle_cy > 0.0f ? 0.0f : angle_cy * 0.9, 5.0f * cos(angle_cx)),
		glm::vec3(0.0f, 1.0f, 0.0f)); //compute view matrix
	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 1.0f, 140.0f); //compute projection matrix

	sp->use();//activate shading program
	//Send parameters to graphics card
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 Mw = glm::translate(M, glm::vec3(wateroffset - gridSize * gridSpacing * 0.5, -5.0f, wateroffset - gridSize * gridSpacing * 0.5));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mw));

	// light parameters
	glUniform4f(sp->u("lp1"), 0.0f, 50.0f, 0.0f, 1.0f); // position
	glUniform4f(sp->u("lp2"), 20.0f, 12.0f, 80.0f, 1.0f);
	glUniform1i(sp->u("lpow1"), 25); // the light's Phong coefficient
	glUniform1i(sp->u("lpow2"), 250);
	glUniform4f(sp->u("ks1"), 1.0f, 1.0f, 1.0f, 1.0f); // reflection color
	glUniform4f(sp->u("ks2"), 1.0f, 1.0f, 0.5f, 1.0f);


	drawWater();

	//M = glm::mat4(1.0f);
	glm::mat4 Ms = modelShip;
	Ms = glm::rotate(Ms, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Compute model matrix
	Ms = glm::rotate(Ms, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Compute model matrix
	Ms = glm::translate(Ms, glm::vec3(0.0f, 0.0f, angle_p));
	modelShip = Ms;
	Ms = glm::translate(Ms, glm::vec3(0.0f, detectOcean() * 200, 0.0f));

	if (kolysanie) Ms = glm::rotate(Ms, angle_f, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 Mo = Ms; //Mo=Macierz modelu oars (wiosel statku)
	if (wioslowanie == 1)
	{
		Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, -1.0f));
		Mo = glm::rotate(Mo, 2 * wioslo_height / 3, glm::vec3(0.0f, 1.0f, 0.0f));
		if (stan == 0)
		{
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, 1.0f, 1.0f));
		}
		else if (stan == 1)
		{
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, -1.0f, 1.0f));
		}
		else if (stan == 2)
		{
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, -1.0f, -1.0f));
		}
		else
		{
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, -1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, 1.0f, -1.0f));
		}
	}
	if (wioslowanie == 2)
	{
		Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, -1.0f));
		Mo = glm::rotate(Mo, 2 * wioslo_height / 3, glm::vec3(0.0f, -1.0f, 0.0f));
		if (stan == 0)
		{
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, -1.0f, 1.0f));
		}
		else if (stan == 1)
		{
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, 1.0f, 1.0f));
		}
		else if (stan == 2)
		{
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, 1.0f, -1.0f));
		}
		else
		{
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, wioslo_height, glm::vec3(0.0f, 1.0f, -1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, -1.0f, -1.0f));
		}
	}

	int vertex_sum = 0;
	GLuint textures[] = { tex_s1,tex_s2,tex_s3,tex_s4,tex_s5,tex_s6,tex_s7,tex_s8,tex_s9,tex_s10,tex_s11,tex_s12,tex_s13,tex_s14 };
	
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms));

	for (int j = 0; j < 14; j++)
	{
		glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices[vertex_sum * 4]); //Specify source of the data for the attribute vertex
		glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals[vertex_sum * 4]); //Specify source of the data for the attribute vertex
		glEnableVertexAttribArray(sp->a("texCoord0"));
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs[vertex_sum * 2]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[j]);
		glUniform1i(sp->u("textureMap0"), 0);

		glDrawArrays(GL_TRIANGLES, 0, number_vertex[j]); //Draw the object
		glDisableVertexAttribArray(sp->a("texCoord0"));
		glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
		glDisableVertexAttribArray(sp->a("normal"));


		vertex_sum = vertex_sum + number_vertex[j];
	}

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mo));

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices[vertex_sum * 4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals[vertex_sum * 4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs[vertex_sum * 2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_s4);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex[14]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));





	glm::mat4 Ml = modelLighthouse;

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ml)); // lighthouse

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &LHvertices[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &LHnormals[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &LHuvs[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texLighthouse);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, LHnumber_vertex[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));




	glm::mat4 Mg = M;
	Mg = glm::translate(Mg, glm::vec3(15.0f, -9.0f, 30.0f));
	Mg = glm::scale(Mg, glm::vec3(8.0f, 8.0f, 8.0f));
	Mg = glm::rotate(Mg, angle_gora, glm::vec3(0.0f, 1.0f, 0.0f));
	Mg = glm::translate(Mg, glm::vec3(-angle_gora / 4, 0.0f, -angle_gora / 2));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mg));

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices_gora[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals_gora[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs_gora[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_gora);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex_gora[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));


	glm::mat4 Mg2 = M;
	Mg2 = glm::translate(Mg2, glm::vec3(40.0f, -9.0f, 50.0f));
	Mg2 = glm::scale(Mg2, glm::vec3(8.0f, 8.0f, 8.0f));
	Mg2 = glm::rotate(Mg2, angle_gora, glm::vec3(0.0f, 1.0f, 0.0f));
	Mg2 = glm::translate(Mg2, glm::vec3(-angle_gora / 4, 0.0f, -angle_gora / 2));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mg2));

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices_gora[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals_gora[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs_gora[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_gora);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex_gora[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));



	glm::mat4 Mch = M;
	Mch = glm::translate(Mch, glm::vec3(50.0f, 30.0f, 140.0f));
	Mch = glm::scale(Mch, glm::vec3(4.0f, 4.0f, 4.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch));

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices_chmura[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals_chmura[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs_chmura[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_chmura);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex_chmura[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));


	glm::mat4 Mch2 = M;
	Mch2 = glm::translate(Mch2, glm::vec3(-50.0f, 30.0f, 140.0f));
	Mch2 = glm::scale(Mch2, glm::vec3(4.0f, 4.0f, 4.0f));
	Mch2 = glm::rotate(Mch2, PI, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mch2));

	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices_chmura[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals_chmura[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs_chmura[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_chmura);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex_chmura[0]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));




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


	float angle_x = 0; //current rotation angle of the object, x axis
	float angle_y = 0; //current rotation angle of the object, y axis
	float angle_k = 0;
	float angle_f = 0;
	float angle_p = 0;
	float time = 0;
	float angle_wioslo = 0;
	float angle_gora = 0;
	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		angle_x = speed_x * glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y = speed_y * glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_k += speed_k * glfwGetTime();
		angle_p = speed_p * glfwGetTime();
		angle_gora += speed_gora * glfwGetTime();
		angle_cx += speed_cx * glfwGetTime();
		angle_cy += speed_cy * glfwGetTime();
		if (angle_cy < -3) angle_cy = -3;
		if (angle_cy > 132) angle_cy = 132;
		pos_angle += angle_x;
		pos_x -= speed_p /50.0 * sin(pos_angle) * glfwGetTime();
		pos_y -= speed_p /50.0 * cos(pos_angle) * glfwGetTime();
		time = glfwGetTime();
		wateroffset += speed_water * time;
		time *= speed_water * 9.5 / gridSize;
		for (float& x : water_textures) x += time;
		while (wateroffset > gridSize / 40.0) {
			wateroffset -= gridSize / 20.0;
			for (float& x : water_textures) x -= 0.504;
		}
		angle_f += speed_f * glfwGetTime();

		if (wioslowanie)
		{
			angle_wioslo += speed_l * glfwGetTime();
			if (angle_wioslo >= wioslo_height) {
				angle_wioslo = 0;
				stan++;
				stan = stan % 4;
			}
		}
		if (!wioslowanie)
		{
			angle_wioslo = 0;
			stan = 0;
		}

		glfwSetTime(0); //Zero the timer
		drawScene(window, angle_x, angle_y, angle_k, sin(angle_f) / 9.0, angle_p, wateroffset,angle_wioslo,angle_gora); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
