#version 330 core

#define ITER_MAX 5000

in vec4 gl_FragCoord;
out vec3 color;

uniform float u_scale;
uniform vec2 u_pan;
uniform vec2 u_resolution;

vec2 cmpxmul(in vec2 a, in vec2 b);
vec2 cmpxadd(in vec2 a, in vec2 b);
float cmpxabs(in vec2 a);

vec3 hsv2rgb(in vec3 c);

void main() {
	vec2 coord = ((vec2(gl_FragCoord) - u_resolution * 0.5f) * u_scale + (u_pan * u_resolution));

	int n = 0;

	vec2 c = vec2(coord.x, coord.y);
	vec2 z = vec2(0.f, 0.f);

	while (cmpxabs(z) < 4.0 && n < ITER_MAX) {
		// z = (z * z) + c;
		z = cmpxmul(z, z);
		z = cmpxadd(z, c);
		n++;
	}

	//color = hsv2rgb(vec3(cos(coord.x * coord.y), 1, 1));
	color = hsv2rgb(vec3(n * 360 / float(ITER_MAX), 1.f, 1.f));
}

vec3 hsv2rgb(in vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 cmpxmul(in vec2 a, in vec2 b){
	return vec2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

float cmpxabs(in vec2 a) {
	return a.x * a.x + a.y * a.y;
}

vec2 cmpxadd(in vec2 a, in vec2 b) {
	return vec2(a.x + b.x, a.y + b.y);
}
