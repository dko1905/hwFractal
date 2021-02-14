#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define CL_TARGET_OPENCL_VERSION 200
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
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
	 */
	/* 1. Get device */
	ret = choose_device(info, config, &info->device_id);
	if (ret != 0) {
		perr("Failed to get device IDs");
		goto cl_gdids_err;
	}
	pdebug("Got device ID");
	/* 2. Create context */
	info->context = clCreateContext(NULL, 1, &info->device_id, NULL, NULL, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create context");
		goto cl_cc_err;
	}
	pdebug("Created context");
	/* 3. Create commancl_int *errcode_retd queue */
	info->command_queue = clCreateCommandQueueWithProperties(info->context, info->device_id, 0, &ret);
	if (ret != CL_SUCCESS) {
		perr("Failed to create command queue");
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
		perr("Failed to create program");
		goto cl_creatprogram_err;
	}
	pdebug("Created program");
	/* Build program. */
	ret = clBuildProgram(info->program, 0, NULL, NULL, NULL, NULL);
	if (ret != CL_SUCCESS) {
		perr("Failed to build program");
		goto cl_buildprogram_err;
	}
	pdebug("Built program");
	/* Create kernel. */
	info->kernel = clCreateKernel(info->program, "calcSin", &ret);
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
	} else if (config->gpu_choice >= 0 && config->gpu_choice < devices - 1) {
		*device_id = device_ids[config->gpu_choice];
	} else {
		free(device_ids);
		perr("Invalid gpu choice in config.");
		return -1;
	}

	free(device_ids);

	return 0;
}
