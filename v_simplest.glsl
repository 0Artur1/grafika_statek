/*#version 330
//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Attributes
in vec4 vertex; 
in vec2 texCoord0; 
in vec4 normal;
out vec2 iTexCoord0;
out vec4 l;
out vec4 n;
out vec4 v;
void main(void) {

 vec4 lp = vec4(0, 0, -6, 1);
l = normalize(V * lp - V * M * vertex); 
v = normalize(vec4(0, 0, 0, 1) - V * M *vertex);
 n = normalize(V * M * normal); 

iTexCoord0 = texCoord0;
gl_Position=P*V*M*vertex;
}
#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Attributes
in vec4 vertex; //Vertex coordinates in model space


void main(void) {
    gl_Position=P*V*M*vertex;
}
*/
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
