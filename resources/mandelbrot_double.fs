#version 410 core

in vec4 gl_FragCoord;
out vec3 color;

uniform double u_scale;
uniform dvec2 u_pan;
uniform vec2 u_resolution;
uniform int u_iter_max;

dvec2 cmpxmul(in dvec2 a, in dvec2 b);
dvec2 cmpxadd(in dvec2 a, in dvec2 b);
double cmpxabs(in dvec2 a);

vec3 hsv2rgb(in vec3 c);

void main() {
	dvec2 coord = ((dvec2(gl_FragCoord) - dvec2(u_resolution) * 0.5) * u_scale + (u_pan * dvec2(u_resolution)));

	int n = 0;

	dvec2 c = dvec2(coord.x, coord.y);
	dvec2 z = dvec2(0., 0.);

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

dvec2 cmpxmul(in dvec2 a, in dvec2 b){
	return dvec2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

double cmpxabs(in dvec2 a) {
	return a.x * a.x + a.y * a.y;
}

dvec2 cmpxadd(in dvec2 a, in dvec2 b) {
	return dvec2(a.x + b.x, a.y + b.y);
}
