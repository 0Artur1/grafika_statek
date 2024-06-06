#version 330
//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp1; // light sources
uniform vec4 lp2;

//Attributes
in vec4 vertex; 
in vec2 texCoord0; 
in vec4 normal;
out vec2 iTexCoord0;
out vec4 n;
out vec4 v;
out vec4 l1;
out vec4 l2;
void main(void) {

	l1 = normalize(V * lp1 - V * M * (vertex));
	l2 = normalize(V * lp2 - V * M * (vertex));  
	v = normalize(vec4(0, 0, 0, 1) - V * M *vertex);
	n = normalize(V * M * normal); 

	iTexCoord0 = texCoord0;
	gl_Position=P*V*M*vertex;
}










/*
#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Attributes
in vec4 vertex; // Koordynaty wierzcholka w przestrzeni modelu
in vec2 texCoord0; // Koordynaty teksturowania
out vec2 iTexCoord0;


void main(void) {
	iTexCoord0 = texCoord0;
	gl_Position=P*V*M*vertex;
}
*/