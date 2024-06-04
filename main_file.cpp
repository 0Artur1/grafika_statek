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


const int gridSize = 1000; //Probably optimal amount for 30 fps
const float gridSpacing = 0.8f;
float waveAmplitude = 0.6f;
std::vector<float>water_textures;
std::vector<float>water_vertices;
std::vector<float> heightMap(gridSize* gridSize, 0.0f);
ShaderProgram* sp;


float speed_x = 0; //angular speed in radians
float speed_y = 0; //angular speed in radians
float speed_k = PI * 4;
float speed_f = PI/45;
float speed_p = 0;
float speed_water = 5;
float aspectRatio = 1;
//float angle_p = 0;
//float angle_k = 0;
glm::mat4 modelShip = glm::mat4(1.0f);

std::vector< float > vertices;
std::vector< float > uvs;
std::vector< float > normals; // Won't be used at the moment.
std::vector<int> number_vertex;

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}



void generateWater()
{
	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			// Przyk³adowa funkcja falowa
			heightMap[i * gridSize + j] = (sin(0.1f * i) * cos(0.1f * j));
		}
	}
	water_vertices.clear();
	water_textures.clear();
	for (int i = 0; i < gridSize - 1; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			// Wierzcho³ek (i, j)
			water_vertices.push_back(i * gridSpacing);
			water_vertices.push_back(heightMap[i * gridSize + j] * waveAmplitude);
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
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x -= PI / 8;
		if (key == GLFW_KEY_RIGHT) speed_x += PI / 8;
		if (key == GLFW_KEY_UP) speed_p += PI * 2;
		if (key == GLFW_KEY_DOWN) speed_p -= PI * 2;
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
		if (waveAmplitude < 0.05f)waveAmplitude = 0.1f;
		if (waveAmplitude > 1.95f) waveAmplitude = 2.0f;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x += PI / 8;
		if (key == GLFW_KEY_RIGHT) speed_x -= PI / 8;
		if (key == GLFW_KEY_UP) speed_p -= PI * 2;
		if (key == GLFW_KEY_DOWN) speed_p += PI * 2;
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
	bool res = parse_from_obj("drakkar_ship.obj", vertices, uvs, normals, number_vertex);
	std::cout << "ended\n";
	generateWater();
	modelShip = glm::scale(modelShip, glm::vec3(0.25f, 0.25f, 0.25f));
	modelShip = glm::translate(modelShip, glm::vec3(0.0f, -21.5f, 100.0f));
	modelShip = glm::rotate(modelShip, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
}


//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************
	delete sp;
	glDeleteTextures(1, &texWater);
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
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_k, float angle_f,float angle_p,float wateroffset) {
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
	Ms = glm::rotate(Ms, -speed_f, glm::vec3(1.0f, 0.0f, 0.0f));
	Ms = glm::rotate(Ms, angle_f, glm::vec3(1.0f, 0.0f, 0.0f));
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
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &normals[number_vertex[0]*4]); //Specify source of the data for the attribute vertex
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, &uvs[number_vertex[0] * 2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texShip1);
	glUniform1i(sp->u("textureMap0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, number_vertex[1]);
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
	float wateroffset = 0;
	float jakies_x = 1;
	int jakies_y = 1;
	float time = 0;
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
		if (jakies_x > -1 && jakies_y==1)
		{
			jakies_x -= glfwGetTime();
			angle_f += speed_f * glfwGetTime();
			//std::cout << jakies_x << std::endl;
		}
		else if(jakies_x<=1)
		{
			jakies_y = 0;
			jakies_x += glfwGetTime();
			angle_f -= speed_f * glfwGetTime();
			//std::cout << jakies_x << std::endl;
			if (jakies_x >= 1) {
				jakies_y = 1;
			}
		}
		glfwSetTime(0); //Zero the timer
		drawScene(window, angle_x, angle_y, angle_k, angle_f,angle_p,wateroffset); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}

