#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>

#define CL_TARGET_OPENCL_VERSION 120
#ifdef __APPLE__
#  include <OpenCL/opencl.h>
#else
#  include <CL/cl.h>
#endif

#include "clw.h"
#include "../util/printer.h"

struct clw_info {
	cl_device_id device_id;
	cl_context context;
	cl_command_queue command_queue;
	cl_program program;
	cl_kernel kernel;
};

static int choose_device(struct clw_info *info, const struct clw_config *config, cl_device_id *device_id);

int clw_init(struct clw_info **info_p, const struct clw_config *config) {
	struct clw_info *info = NULL;
	char *kernel_str = NULL;
	cl_int ret;

	/* Allocate clw_info input. */
	*info_p = malloc(sizeof(struct clw_info));
	if (*info_p == NULL) {
		perr("Failed to allocate clw_info: %s", strerror(errno));
		goto malloc_info_err;
	}
	info = *info_p;

	/* Steps to setup OpenCL:
	 * 1. Get/select device
	 * 2. Create context
	 * 3. Create command queue
	 * 4. Read, compile & create kernel from source code
	 * 5. Create buffers
	 */
	/* 1. Get device */
	ret = choose_device(info, config, &info->device_id);
	if (ret != 0) {
		perr("Failed to get device IDs: %i", ret);
		goto cl_gdids_err;
	}
	pdebug("Got device ID");
	/* 2. Create context */
	info->context = clCreateContext(NULL, 1, &info->device_id, NULL, NULL, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create context: %i", ret);
		goto cl_cc_err;
	}
	pdebug("Created context");
	/* 3. Create commancl_int *errcode_retd queue */
	/* Yes, we are using a deprecated function here, but we are targeting
	 * OpenCL 1.2, so it's fine.
	 */
	info->command_queue = clCreateCommandQueue(info->context, info->device_id, 0, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create command queue: %i", ret);
		goto cl_createqueue_err;
	}
	pdebug("Created command queue");
	/* 4. *Read, compile & create kernel from source code */
	/* Read source code. */
	if (config->kernel_use_file) {
		FILE* file;
		struct stat fstat;
		if (stat(config->kernel_path, &fstat) != 0) {
			perr("Failed to stat %s: %s", config->kernel_path, strerror(errno));
			goto cl_readkernel_err;
		}
		kernel_str = malloc(fstat.st_size + 1);
		if (kernel_str == NULL) {
			perr("Failed to allocate kernel_str buffer: %s", strerror(errno));
			goto cl_readkernel_err;
		}
		kernel_str[fstat.st_size] = '\0';
		if ((file = fopen(config->kernel_path, "r")) == NULL) {
			perr("Failed to open %s: %s", config->kernel_path, strerror(errno));
			goto cl_readkernel_err;
		}
		if (fread(kernel_str, fstat.st_size, 1, file) != 1) {
			fclose(file);
			perr("Failed to read from %s: %s", config->kernel_path, strerror(errno));
			goto cl_readkernel_err;
		}
		fclose(file);
	} else {
		kernel_str = malloc(strlen(config->kernel_str) + 1);
		if (kernel_str == NULL) {
			perr("Failed to allocate kernel_str buffer: %s", strerror(errno));
			goto cl_readkernel_err;
		}
		memcpy(kernel_str, config->kernel_str, strlen(config->kernel_str) + 1);
	}
	pdebug("Read kernel source code");
	/* Create program. */
	info->program = clCreateProgramWithSource(info->context, 1, (const char **)&kernel_str, NULL, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create program: %i", ret);
		goto cl_creatprogram_err;
	}
	pdebug("Created program");
	/* Build program. */
	ret = clBuildProgram(info->program, 0, NULL, NULL, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to build program: %i", ret);
		goto cl_buildprogram_err;
	}
	pdebug("Built program");
	/* Create kernel. */
	info->kernel = clCreateKernel(info->program, "calcArr", &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create kernel: %i", ret);
		goto cl_creatkernel_err;
	}
	pdebug("Created kernel");
	free(kernel_str);

	return 0;

	clReleaseKernel(info->kernel);
cl_creatkernel_err:
cl_buildprogram_err:
	clReleaseProgram(info->program);
cl_creatprogram_err:
	free(kernel_str);
cl_readkernel_err:
	clReleaseCommandQueue(info->command_queue);
cl_createqueue_err:
	clReleaseContext(info->context);
cl_cc_err:
cl_gdids_err:
	free(*info_p);
	*info_p = NULL;
malloc_info_err:
	return -1;
}

void clw_free(struct clw_info **info_p) {
	struct clw_info *info = *info_p;

	clReleaseKernel(info->kernel);
	clReleaseProgram(info->program);
	clReleaseCommandQueue(info->command_queue);
	clReleaseContext(info->context);

	free(*info_p);
	*info_p = NULL;
}

void clw_test2(struct clw_info *info, size_t arr_size) {
	float *in1 = NULL, *in2 = NULL, *out = NULL;
	cl_mem in1_mem = NULL, in2_mem = NULL, out_mem = NULL;
	cl_int ret = 0;

	/* Create CPU buffers. */
	in1 = malloc(arr_size * sizeof(float));
	if (in1 == NULL) {
		perr("Failed to allocate in1 (CPU): %s", strerror(errno));
		goto in1_err;
	}
	in2 = malloc(arr_size * sizeof(float));
	if (in2 == NULL) {
		perr("Failed to allocate n2 (CPU): %s", strerror(errno));
		goto in2_err;
	}
	out = malloc(arr_size * sizeof(float));
	if (out == NULL) {
		perr("Failed to allocate out (CPU): %s", strerror(errno));
		goto out_err;
	}
	pdebug("Allocated n1, n2 and out (CPU)");
	/* Create GPU buffer. */
	in1_mem = clCreateBuffer(info->context, CL_MEM_READ_ONLY, arr_size * sizeof(float), NULL, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create in1 (GPU): %i", ret);
		goto in1_gpu_err;
	}
	in2_mem = clCreateBuffer(info->context, CL_MEM_READ_ONLY, arr_size * sizeof(float), NULL, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create in2 (GPU): %i", ret);
		goto in2_gpu_err;
	}
	out_mem = clCreateBuffer(info->context, CL_MEM_WRITE_ONLY, arr_size * sizeof(float), NULL, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create out (GPU): %i", ret);
		goto out_gpu_err;
	}
	pdebug("Created in1, in2 and out (GPU)");
	/* Fill CPU buffers. */
	for (size_t n = 0; n < arr_size; ++n) {
		in1[n] = (float)(rand() % 1000) / 10.f;
		in2[n] = (float)(rand() % 1000) / 10.f;
		out[n] = 1.5f;
	}
	/* Enqueue onto GPU. */
	ret = clEnqueueWriteBuffer(info->command_queue, in1_mem, CL_FALSE, 0, arr_size * sizeof(float), in1, 0, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to enqueue in1: %i", ret);
		goto enqueue_err;
	}
	ret = clEnqueueWriteBuffer(info->command_queue, in2_mem, CL_FALSE, 0, arr_size * sizeof(float), in2, 0, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to enqueue in2: %i", ret);
		goto enqueue_err;
	}
	ret = clEnqueueWriteBuffer(info->command_queue, out_mem, CL_FALSE, 0, arr_size * sizeof(float), out, 0, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to enqueue out: %i", ret);
		goto enqueue_err;
	}
	pdebug("Enqueued n1 and n2");
	/* Set kernel arguments. */
 	ret = clSetKernelArg(info->kernel, 0, sizeof(cl_mem), &in1_mem);
 	if (ret != CL_SUCCESS) {
 		perr("Failed to set in1 argument: %i", ret);
 		goto kernel_arg_err;
 	}
 	ret = clSetKernelArg(info->kernel, 1, sizeof(cl_mem), &in2_mem);
 	if (ret != CL_SUCCESS) {
 		perr("Failed to set in2 argument: %i", ret);
 		goto kernel_arg_err;
 	}
 	ret = clSetKernelArg(info->kernel, 2, sizeof(cl_mem), &out_mem);
 	if (ret != CL_SUCCESS) {
 		perr("Failed to set out argument: %i", ret);
 		goto kernel_arg_err;
 	}
	pdebug("Set kernel arguments");
	/* Enqueue starting the kernel. */
	size_t global_dimensions[] = {arr_size, 0, 0};
	ret = clEnqueueNDRangeKernel(info->command_queue, info->kernel, 1, NULL, global_dimensions, NULL, 0, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to enqueue NDRangeKernel: %i", ret);
		goto range_enqueue_err;
	}
	pdebug("Enqueued NDRangeKernel");
	/* Read results. */
	ret = clEnqueueReadBuffer(info->command_queue, out_mem, CL_FALSE, 0, arr_size * sizeof(float), out, 0, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to enqueue ReadBuffer: %i", ret);
		goto read_out_err;
	}
	pdebug("Enqueued ReadBuffer");
	/* Wait until it's done. */
	ret = clFinish(info->command_queue);
	if (ret != CL_SUCCESS) {
		perr("Failed to finish: %i", ret);
		goto finish_err;
	}
	pdebug("Finished", ret);
	/* Print results. */
	pdebug("Output: ");
	for (size_t n = 0; n < arr_size - 1; ++n) {
		printf("%.3f, ", in1[n]);
	}
	printf("%.3f\n", out[arr_size - 1]);
	for (size_t n = 0; n < arr_size - 1; ++n) {
		printf("%.3f, ", in2[n]);
	}
	printf("%.3f\n", out[arr_size - 1]);
	for (size_t n = 0; n < arr_size - 1; ++n) {
		printf("%.3f, ", out[n]);
	}
	printf("%.3f\n", out[arr_size - 1]);

finish_err:
read_out_err:
range_enqueue_err:
kernel_arg_err:
enqueue_err:
	clReleaseMemObject(out_mem);
out_gpu_err:
	clReleaseMemObject(in2_mem);
in2_gpu_err:
	clReleaseMemObject(in1_mem);
in1_gpu_err:
	free(out);
out_err:
	free(in2);
in2_err:
	free(in1);
in1_err:
	return;
}

// void clw_test(struct clw_info *info) {
// 	cl_int ret = 0;

// 	/* Copy data to buffer. */
// 	ret = clEnqueueWriteBuffer(info->command_queue, info->data_gpu, CL_FALSE, 0, sizeof(info->data_cpu), info->data_cpu, 0, NULL, NULL);
// 	if (ret != CL_SUCCESS) {
// 		perr("Failed to enqueue write buffer: %i", ret);
// 		return;
// 	}
// 	pdebug("Enqueued write buffer");

// 	/* Execute kernel. */
// 	ret = clSetKernelArg(info->kernel, 0, sizeof(info->data_gpu), &info->data_gpu);
// 	if (ret != CL_SUCCESS) {
// 		perr("Failed to set kernel arguments: %i", ret);
// 		return;
// 	}
// 	pdebug("Set kernel arguments");
// 	size_t global_dimensions[] = {sizeof(info->data_cpu)/sizeof(info->data_cpu[0]), 0, 0};
// 	ret = clEnqueueNDRangeKernel(info->command_queue, info->kernel, 1, NULL, global_dimensions, NULL, 0, NULL, NULL);
// 	if (ret != CL_SUCCESS) {
// 		perr("Failed to enqueue NDRangeKernel: %i", ret);
// 		return;
// 	}
// 	pdebug("Enqueued NDRangeKernel");

// 	/* Read results. */
// 	ret = clEnqueueReadBuffer(info->command_queue, info->data_gpu, CL_FALSE, 0, sizeof(info->data_cpu), info->data_cpu, 0, NULL, NULL);
// 	if (ret != CL_SUCCESS) {
// 		perr("Failed to enqueue ReadBuffer: %i", ret);
// 		return;
// 	}
// 	pdebug("Enqueued ReadBuffer");

// 	ret = clFinish(info->command_queue);
// 	if (ret != CL_SUCCESS) {
// 		perr("Failed to finish: %i", ret);
// 		return;
// 	}
// 	pdebug("Finished", ret);

// 	pdebug("2:");
// 	for (size_t n = 0; n < sizeof(info->data_cpu)/sizeof(info->data_cpu[0]); ++n) {
// 		pdebug("\t%zu: %f", n, info->data_cpu[n]);
// 	}
// }

static int choose_device(struct clw_info *info, const struct clw_config *config, cl_device_id *device_id) {
	cl_int ret = 0;
	cl_uint devices = 0;
	cl_device_id *device_ids = NULL;

	/* Get number of devices. */
	ret = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 0, NULL, &devices);
	if (ret != CL_SUCCESS) {
		perr("Failed to get device IDs");
		return -1;
	}
	pdebug("Got device IDs, amount of devices: %u", devices);

	/* Allocate space for devices. */
	device_ids = malloc(sizeof(cl_device_id) * devices);
	if (device_ids == NULL) {
		perr("Failed to allocate device_ids: %s", strerror(errno));
		return -1;
	}
	pdebug("Allocated device_ids");

	/* Load info about devices. */
	ret = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, devices, device_ids, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to get device IDs 2");
		free(device_ids);
		return -1;
	}
	pdebug("Got device IDs 2");

	pinfo("Detected %u device(s)", devices);

	for (cl_uint i = 0; i < devices; ++i) {
		uint8_t ret_buffer[50];
		size_t ret_size;

		pinfo("Device %u:", i); /* Print device info. */

		/* Get device info. */
		ret = clGetDeviceInfo(device_ids[i], CL_DEVICE_AVAILABLE, sizeof(ret_buffer), ret_buffer, &ret_size);
		if (ret != CL_SUCCESS) {
			pwarn("Failed to get device status, not quitting: %i", ret);
		}
		pinfo("    Available: %s", ((cl_int)ret_buffer[0]) == CL_TRUE ? "YES" : "NO");
		ret = clGetDeviceInfo(device_ids[i], CL_DEVICE_TYPE, sizeof(ret_buffer), ret_buffer, &ret_size);
		if (ret != CL_SUCCESS) {
			pwarn("Failed to get device type, not quitting: %i", ret);
		}
		switch (*(cl_device_type *)ret_buffer) {
			case CL_DEVICE_TYPE_CPU:
				ret_buffer[0] = 'C';
				ret_buffer[1] = 'P';
				ret_buffer[2] = 'U';
				ret_buffer[3] = '\0';
				break;
			case CL_DEVICE_TYPE_GPU:
				ret_buffer[0] = 'G';
				ret_buffer[1] = 'P';
				ret_buffer[2] = 'U';
				ret_buffer[3] = '\0';
				break;
			case CL_DEVICE_TYPE_ACCELERATOR:
				ret_buffer[0] = 'A';
				ret_buffer[1] = 'C';
				ret_buffer[2] = 'C';
				ret_buffer[3] = 'E';
				ret_buffer[4] = 'L';
				ret_buffer[5] = '\0';
				break;
			default:
				ret_buffer[0] = 'D';
				ret_buffer[1] = 'E';
				ret_buffer[2] = 'F';
				ret_buffer[3] = 'A';
				ret_buffer[4] = 'U';
				ret_buffer[5] = 'L';
				ret_buffer[6] = 'T';
				ret_buffer[7] = '\0';
				break;
		}
		pinfo("    Type: %s", (char *)ret_buffer);
		ret = clGetDeviceInfo(device_ids[i], CL_DEVICE_NAME, sizeof(ret_buffer), ret_buffer, &ret_size);
		if (ret != CL_SUCCESS) {
			pwarn("Failed to get device name, not quitting: %i", ret);
		}
		pinfo("    Device name: %s", (char *)ret_buffer);
		ret = clGetDeviceInfo(device_ids[i], CL_DEVICE_PROFILE, sizeof(ret_buffer), ret_buffer, &ret_size);
		if (ret != CL_SUCCESS) {
			pwarn("Failed to get device profile, not quitting: %i", ret);
		}
		pinfo("    Device profile: %s", (char *)ret_buffer);
	}

	if (config->gpu_choice == -1) {
		unsigned int user_input = -1;
		int scanf_ret = 0;
		do {
			pinfo("Please choose device: ");
			scanf_ret = scanf("%u", &user_input);
		} while (scanf_ret != EOF && (scanf_ret != 1 || user_input > devices - 1 || user_input < 0));
		if (scanf_ret == EOF) {
			free(device_ids);
			perr("Received EOF, quitting.");
			return -1;
		}
		pinfo("You chose %u", user_input);
		*device_id = device_ids[user_input];
	} else if (config->gpu_choice >= 0 && config->gpu_choice < devices) {
		*device_id = device_ids[config->gpu_choice];
	} else {
		free(device_ids);
		perr("Invalid gpu choice in config.");
		return -1;
	}

	free(device_ids);

	return 0;
}
