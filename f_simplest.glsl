#version 330
uniform sampler2D textureMap0;
uniform int lpow1;
uniform int lpow2;
uniform vec4 ks1;
uniform vec4 ks2;

in vec2 iTexCoord0;
in vec4 n;
in vec4 v;
in vec4 l1;
in vec4 l2;
out vec4 pixelColor;

void main(void) {
	vec4 ml1 = normalize(l1);
	vec4 ml2 = normalize(l2);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	vec4 mr1 = reflect(-l1, n);
	vec4 mr2 = reflect(-l2, n);
	vec4 kd = texture(textureMap0, iTexCoord0);

	float nl1 = clamp(dot(mn, ml1), 0, 1);
	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv1 = pow(clamp(dot(mr1, mv), 0, 1), lpow1);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1), lpow2);
	pixelColor = vec4(kd.rgb * nl1, kd.a) + vec4(kd.rgb * nl2, kd.a) + vec4(ks1.rgb * rv1, 0);+ vec4(ks2.rgb * rv2, 0);
}











/*

#version 330

out vec4 pixelColor; //Output variable. Almost final pixel color.
void main(void) {
	pixelColor= vec4(0.0f, 0.5f, 1.0f,1.0f);
}

#version 330

uniform sampler2D textureMap0;
in vec2 iTexCoord0;

out vec4 pixelColor;
void main(void) {
pixelColor= texture(textureMap0, iTexCoord0);
}
*/

