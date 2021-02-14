kernel void calcSin(global float *data) {
	int id = get_global_id(0);
	data[id] = sin(data[id]);
}
