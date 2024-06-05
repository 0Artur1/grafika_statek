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

//#define piesek //Nalezy odkomentowac, aby zmienic statek

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


const int gridSize = 1000; //Probably optimal amount for 30 fps
const float gridSpacing = 0.8f;
float waveAmplitude = 0.4f;
std::vector<float>water_textures;
std::vector<float>water_vertices;
std::vector<float> heightMap(gridSize* gridSize, 0.0f);
ShaderProgram* sp;


float speed_x = 0; //angular speed in radians
float speed_y = 0; //angular speed in radians
float speed_k = PI * 4;
float speed_f = PI / 45;
float speed_p = 0;
float speed_water = 8;
float aspectRatio = 1;
int kolysanie = 1;
#ifdef piesek
float speed_l = PI / 8;
int stan = 0;
int wioslowanie = 0;
#endif
//float angle_p = 0;
//float angle_k = 0;
glm::mat4 modelShip = glm::mat4(1.0f);

std::vector< float > vertices;
std::vector< float > uvs;
std::vector< float > normals; // Won't be used at the moment.
std::vector<int> number_vertex;

#ifdef piesek
	GLuint tex_s1, tex_s2, tex_s3, tex_s4, tex_s5, tex_s6, tex_s7, tex_s8, tex_s9, tex_s10, tex_s11, tex_s12, tex_s13, tex_s14;
#endif
GLuint texWater, texShip1, texShip2; //texture handle
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



void generateWater()
{
	int r;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			// Przyk³adowa funkcja falowa
			heightMap[i * gridSize + j] = sin(0.1f * i) * cos(0.1f * j);
		}
	}
	water_vertices.clear();
	water_textures.clear();
	for (int i = 0; i < gridSize - 2; i += 2) {
		for (int j = 0; j < gridSize; ++j) {
			// Wierzcho³ek (i, j)
			water_vertices.push_back(i * gridSpacing);
			water_vertices.push_back(heightMap[(i)*gridSize + j] * waveAmplitude);
			water_vertices.push_back(j * gridSpacing);
			water_vertices.push_back(1.0f);

			// Wierzcho³ek (i+1, j)
			water_vertices.push_back((i + 1) * gridSpacing);
			water_vertices.push_back(heightMap[(i + 1) * gridSize + j] * waveAmplitude);
			water_vertices.push_back(j * gridSpacing);
			water_vertices.push_back(1.0f);

			// tekstura
			water_textures.push_back(1 / (gridSize / 8.0) * i);
			water_textures.push_back(1 / (gridSize / 8.0) * j);
			water_textures.push_back(1 / (gridSize / 8.0) * (i + 1));
			water_textures.push_back(1 / (gridSize / 8.0) * j);
		}
		//Generowanie w odwrotnej kolejnoœci, aby poprawiæ b³¹d
		for (int j = gridSize; j > 0; --j) {
			// Wierzcho³ek (i, j)
			water_vertices.push_back((i + 1) * gridSpacing);
			water_vertices.push_back(heightMap[(i + 1) * gridSize + j] * waveAmplitude);
			water_vertices.push_back(j * gridSpacing);
			water_vertices.push_back(1.0f);

			// Wierzcho³ek (i+1, j)
			water_vertices.push_back((i + 2) * gridSpacing);
			water_vertices.push_back(heightMap[(i + 2) * gridSize + j] * waveAmplitude);
			water_vertices.push_back(j * gridSpacing);
			water_vertices.push_back(1.0f);

			// tekstura
			water_textures.push_back(1 / (gridSize / 8.0) * (i + 1));
			water_textures.push_back(1 / (gridSize / 8.0) * j);
			water_textures.push_back(1 / (gridSize / 8.0) * (i + 2));
			water_textures.push_back(1 / (gridSize / 8.0) * j);
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x -= PI / 8;
		if (key == GLFW_KEY_RIGHT) speed_x += PI / 8;
		if (key == GLFW_KEY_UP)
		{
			speed_p += PI * 2;
#ifdef piesek
			wioslowanie = 1;
			speed_p += PI * 98;
#endif
		}
		if (key == GLFW_KEY_DOWN)
		{
			speed_p -= PI * 2;
#ifdef piesek
			wioslowanie = 1;
			speed_p -= PI * 98;
#endif
		}
		if (key == GLFW_KEY_W)
		{
			waveAmplitude = waveAmplitude + 0.1f;
			generateWater();
			//speed_f = speed_f + PI / 180;
		}
		if (key == GLFW_KEY_S)
		{
			waveAmplitude = waveAmplitude - 0.1f;
			generateWater();
			//speed_f = speed_f + PI / 180;
		}
		if (key == GLFW_KEY_Z) kolysanie = !kolysanie;

#ifdef piesek
		if (key == GLFW_KEY_X) wioslowanie = !wioslowanie;
#endif
		if (waveAmplitude < 0.05f)waveAmplitude = 0.1f;
		if (waveAmplitude > 1.95f) waveAmplitude = 2.0f;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x += PI / 8;
		if (key == GLFW_KEY_RIGHT) speed_x -= PI / 8;
		if (key == GLFW_KEY_UP)
		{
			speed_p -= PI * 2;
#ifdef piesek
			wioslowanie = 0;
			speed_p -= PI * 98;
#endif
		}
		if (key == GLFW_KEY_DOWN)
		{
			speed_p += PI * 2;
#ifdef piesek
			wioslowanie = 0;
			speed_p += PI * 98;
#endif
		}
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
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	texWater = readTexture("morze9.png");
	texShip1 = readTexture("mat1_c.png");
	texShip2 = readTexture("mat0_c.png");
#ifdef piesek
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
	bool res = another_parse_from_obj("Ship08.obj", vertices, uvs, normals, number_vertex);
	std::cout << "ended\n";
	modelShip = glm::translate(modelShip, glm::vec3(0.0f, -4.3f, 20.0f));
	modelShip = glm::scale(modelShip, glm::vec3(0.005f, 0.005f, 0.005f));
#endif
	generateWater();
#ifndef piesek
	bool res = parse_from_obj("drakkar.obj", vertices, uvs, normals, number_vertex);
	std::cout << "ended\n";
	modelShip = glm::scale(modelShip, glm::vec3(0.25f, 0.25f, 0.25f));
	modelShip = glm::translate(modelShip, glm::vec3(0.0f, -22.0f, 100.0f));
#endif
	modelShip = glm::rotate(modelShip, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
}


//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
	glDeleteTextures(1, &texWater);
	glDeleteTextures(1, &texShip1);
	glDeleteTextures(1, &texShip2);
#ifdef piesek
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
#endif
}

void drawWater() {
	//sp->use();

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, GL_FALSE, 0, &water_vertices[0]);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &water_textures[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWater);

	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, water_vertices.size() / 4);

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("texCoord0"));
}

//Drawing procedure
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_k, float angle_f, float angle_p, float wateroffset, float angle_wioslo) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, 1.0f, -5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)); //compute view matrix
	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 1.0f, 250.0f); //compute projection matrix

	sp->use();//activate shading program
	//Send parameters to graphics card
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 Mw = glm::translate(M, glm::vec3(wateroffset - gridSize * gridSpacing * 0.5, -5.0f, wateroffset - gridSize * gridSpacing * 0.5));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mw));

	drawWater();

	//M = glm::mat4(1.0f);
	glm::mat4 Ms = modelShip;
	Ms = glm::rotate(Ms, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Compute model matrix
	Ms = glm::rotate(Ms, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Compute model matrix
	Ms = glm::translate(Ms, glm::vec3(0.0f, 0.0f, angle_p));
	modelShip = Ms;

	if (kolysanie)
	{
		Ms = glm::rotate(Ms, -speed_f, glm::vec3(1.0f, 0.0f, 0.0f));
		Ms = glm::rotate(Ms, angle_f, glm::vec3(1.0f, 0.0f, 0.0f));
	}

#ifdef piesek
	glm::mat4 Mo = Ms; //Mo=Macierz modelu oars (wiosel statku)
	if (wioslowanie)
	{
		if (stan == 0)
		{
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, 1.0f, 1.0f));
		}
		else if (stan == 1)
		{
			Mo = glm::rotate(Mo, PI / 16, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, -1.0f, 1.0f));
		}
		else if (stan == 2)
		{
			Mo = glm::rotate(Mo, PI / 16, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, PI / 16, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, -1.0f, -1.0f));
		}
		else
		{
			Mo = glm::rotate(Mo, PI / 16, glm::vec3(0.0f, 1.0f, 1.0f));
			Mo = glm::rotate(Mo, PI / 16, glm::vec3(0.0f, -1.0f, 1.0f));
			Mo = glm::rotate(Mo, PI / 16, glm::vec3(0.0f, -1.0f, -1.0f));
			Mo = glm::rotate(Mo, angle_wioslo, glm::vec3(0.0f, 1.0f, -1.0f));
		}
	}
#endif

#ifndef piesek

	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Ms));

	//czêœæ pierwsza statku
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals[0]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texShip2);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex[0]); //Draw the object
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));



	//czêœc druga statku
	glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &vertices[number_vertex[0] * 4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals[number_vertex[0] * 4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs[number_vertex[0] * 2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texShip1);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex[1]);
	glDisableVertexAttribArray(sp->a("texCoord0"));
	glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
	glDisableVertexAttribArray(sp->a("normal"));

#endif


#ifdef piesek
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

#endif

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
	float wateroffset = 0;
	float jakies_x = 1;
	int jakies_y = 1;
	float time = 0;
	float angle_wioslo = 0;
	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		angle_x = speed_x * glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y = speed_y * glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_k += speed_k * glfwGetTime();
		angle_p = speed_p * glfwGetTime();
		time = glfwGetTime();
		wateroffset += speed_water * time;
		time *= speed_water * 10.0 / gridSize;
		for (float& x : water_textures) x += time;
		while (wateroffset > gridSize / 8.0) {
			wateroffset -= gridSize / 4.0;
			for (float& x : water_textures) x -= 0.5;
		}
		if (jakies_x > -1 && jakies_y == 1)
		{
			jakies_x -= glfwGetTime();
			angle_f += speed_f * glfwGetTime();
			//std::cout << jakies_x << std::endl;
		}
		else if (jakies_x <= 1)
		{
			jakies_y = 0;
			jakies_x += glfwGetTime();
			angle_f -= speed_f * glfwGetTime();
			//std::cout << jakies_x << std::endl;
			if (jakies_x >= 1) {
				jakies_y = 1;
			}
		}

#ifdef piesek
		if (speed_p > 0 || wioslowanie)
		{
			if (angle_wioslo >= PI / 16) {
				angle_wioslo = 0;
				stan++;
				stan = stan % 4;
			}
			angle_wioslo += speed_l * glfwGetTime();
		}
		if (speed_p < 0)
		{
			if (angle_wioslo <= PI / 16) {
				angle_wioslo = 0;
				stan++;
				stan = stan % 4;
			}
			angle_wioslo -= speed_l * glfwGetTime();
		}
#endif

		glfwSetTime(0); //Zero the timer
		drawScene(window, angle_x, angle_y, angle_k, angle_f, angle_p, wateroffset,angle_wioslo); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
