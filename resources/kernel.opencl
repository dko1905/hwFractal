kernel void calcArr(global float *in1, global float *in2, global float *out) {
	int id = get_global_id(0);
	out[id] = sqrt(in2[id] * in1[id]);
}

kernel void calcMandelbrotSlice(global float )
