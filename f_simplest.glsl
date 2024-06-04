/*#version 330
uniform sampler2D textureMap0;

in vec2 iTexCoord0;
in vec4 n;
in vec4 l;
in vec4 v;
out vec4 pixelColor;

void main(void) {
vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	vec4 mr = reflect(-l, n);
vec4 kd = texture(textureMap0, iTexCoord0);
	vec4 ks = vec4(1, 1, 1, 1);

float nl = clamp(dot(mn, ml), 0, 1);
float rv = pow(clamp(dot(mr, mv), 0, 1), 50);
pixelColor = vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb * rv, 0);
}

#version 330

out vec4 pixelColor; //Output variable. Almost final pixel color.
void main(void) {
	pixelColor= vec4(0.0f, 0.5f, 1.0f,1.0f);
}
*/

#version 330

uniform sampler2D textureMap0;
in vec2 iTexCoord0;

out vec4 pixelColor;
void main(void) {
pixelColor= texture(textureMap0, iTexCoord0);
}

