#version 410 core

in vec4 gl_FragCoord;
out vec3 color;

void main() {
	dvec2 a = dvec2(2, 2);
    color = vec3(cos(gl_FragCoord.x * gl_FragCoord.y / 10000),0,0);
}
