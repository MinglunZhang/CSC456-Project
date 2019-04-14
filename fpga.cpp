#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <arm_neon.h>
#include "CL/opencl.h"
#include "AOCL_Utils.h"

using namespace aocl_utils;

// OpenCL runtime configuration
cl_platform_id platform = NULL;
unsigned num_devices = 0;
scoped_array<cl_device_id> device; // num_devices elements
cl_context context = NULL;
scoped_array<cl_command_queue> queue; // num_devices elements
cl_program program = NULL;
scoped_array<cl_kernel> kernel; // num_devices elements
scoped_array<int> n_per_device; // num_devices elements

int N; // problem size;

int main(int argc, char **argv) {
    scanf( "%d", &N);
    int mode=0;
    if(argc == 2)
        mode = atoi(argv[1]);
      switch(mode)
    {
      case 0:
          break;

      case 1:
          break;

      case 2:
          break;

      default:

          break;

    }
    return 0;



}

bool init_opencl() {
    cl_int status;
    printf("Initializing OpenCL\n");
    if(!setCwdToExeDir()) {
        return false;
    }
    // Get the OpenCL platform.
    platform = findPlatform("Altera");
    if(platform == NULL) {
        printf("ERROR: Unable to find Altera OpenCL platform.\n");
        return false;
    }
      // Query the available OpenCL device.
    device.reset(getDevices(platform, CL_DEVICE_TYPE_ALL, &num_devices));
    printf("Platform: %s\n", getPlatformName(platform).c_str());
    printf("Using %d device(s)\n", num_devices);
    for(unsigned i = 0; i < num_devices; ++i) {
        printf("  %s\n", getDeviceName(device[i]).c_str());
    }

      // Create the context.
    context = clCreateContext(NULL, num_devices, device, NULL, NULL, &status);
    checkError(status, "Failed to create context");
      // Create the program for all device. Use the first device as the
    // representative device (assuming all device are of the same type).
    std::string binary_file = getBoardBinaryFile("AES", device[0]);
    printf("Using AOCX: %s\n", binary_file.c_str());
    program = createProgramFromBinary(context, binary_file.c_str(), device, num_devices);

      // Build the program that was just created.
    status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
    checkError(status, "Failed to build program");

      // Create per-device objects.
    queue.reset(num_devices);
    kernel.reset(num_devices);
    for(unsigned i = 0; i < num_devices; ++i) {
    // Command queue.
        queue[i] = clCreateCommandQueue(context, device[i], CL_QUEUE_PROFILING_ENABLE, &status);
        checkError(status, "Failed to create command queue");

        // Kernel.
        const char *kernel_name = "vectorAdd";
        kernel[i] = clCreateKernel(program, kernel_name, &status);
        checkError(status, "Failed to create kernel");

            // Determine the number of elements processed by this device.
        n_per_device[i] = N / num_devices; // number of elements handled by this device

        // Spread out the remainder of the elements over the first
        // N % num_devices.
        if(i < (N % num_devices)) {
          n_per_device[i]++;
        }

        //input buffers

    }

    return true;


}