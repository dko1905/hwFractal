#version 330 core

in vec4 gl_FragCoord;
out vec3 color;

uniform float u_scale;
uniform vec2 u_pan;
uniform vec2 u_resolution;
uniform int u_iter_max;
uniform vec2 u_c;

vec2 cmpxmul(in vec2 a, in vec2 b);
vec2 cmpxadd(in vec2 a, in vec2 b);
float cmpxabs(in vec2 a);

vec3 hsv2rgb(in vec3 c);

void main() {
	vec2 coord = ((vec2(gl_FragCoord) - u_resolution * 0.5f) * u_scale + (u_pan * u_resolution));

	int n = 0;

	vec2 c = u_c;
	vec2 z = vec2(coord.x, coord.y);

	while (cmpxabs(z) < 4.0 && n < u_iter_max) {
		// z = (z * z) + c;
		z = cmpxmul(z, z);
		z = cmpxadd(z, c);
		n++;
	}

	if (n >= u_iter_max) {
		color = vec3(0.f, 0.f, 0.f);
	} else {
		color = hsv2rgb(vec3(((n * 10) % 255)/255.f, 1.f, 1.f));
	}
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
