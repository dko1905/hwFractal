#version 410 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform vec2 u_resolution;
uniform double u_zoom;
uniform vec2 u_offset;
uniform uint u_maxiter;

void main(){
	gl_Position.xyz = vertexPosition_modelspace;
	gl_Position.w = 1.0;
}