#version 120

varying vec3 n;

void main() {
	gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor = gl_Color;
    n = gl_NormalMatrix * gl_Normal;
}
