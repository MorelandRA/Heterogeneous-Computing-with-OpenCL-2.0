//The code of this program is taken fully from section 3.6.1, with some added or modified comments for clarity based
//on the notes provided in 3.6, as well as some minor refactors for my own readability. It implements vector addition

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

//Kernel for element-wise addition. Logically equivalent to section 3.4's vector add
const char* programSource =
"__kernel \n"
"void vecadd(__global int *A,\n"
"            __global int *B,\n" //The newlines aren't really needed for compilation, but if the compiler fails,
"            __global int *C)\n" //it will include a line number, so breaking it out helps.
"{\n"
"int idx = get_global_id(0);\n"
" C[idx] = A[idx] + B[idx];\n"
"}"
;

int main()
{
    //Number of elements in each array
    const int elements = 2048;

    //Size of the data
    size_t datasize = sizeof(int)*elements;

    //Space for input/output data
    int *A = (int*)malloc(datasize); //Input 1
    int *B = (int*)malloc(datasize); //Input 2
    int *C = (int*)malloc(datasize); //Output

    //Initialize the input data
    for (int i = 0; i < elements; i++)
    {
        A[i] = i;
        B[i] = i;
    } //Since this is just an example, we'll do it like this, but in the real world, if A[i] = i, then we could just
    //use the index instead of the value, have no inputs, and output C[i] = 2*i.

    //Used to check the output of API calls
    cl_int status;

    //Get the first platform
    cl_platform_id platform;
    status = clGetPlatformIDs(1, &platform, NULL);
    //Apparently we're storing the status despite not checking it

    //Get the first device on the first platform;
    cl_device_id device;
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
    //Note that since we're using CL_DEVICE_TYPE_ALL, this could also run on the CPU

    //Create a context and associate it with the device
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);

    //Create a command-queue and associate it with the device
    cl_command_queue cmdQueue = clCreateCommandQueue(context, device, 0, &status);

    //Allocate buffers
    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);

    //Write data from input arrays to buffers
    status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL);
    status = clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, datasize, B, 0, NULL, NULL);

    //Create a program from the kernel source code
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&programSource, NULL, &status);

    //Build the program for the device
    status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    //Create the kernel
    cl_kernel kernel = clCreateKernel(program, "vecadd", &status);

    //Set the kernel arguments
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

    //Define the index space of work-items (Optional)
    size_t indexSpaceSize[1], workGroupSize[1];

    //There are "elements"-number of work items
    indexSpaceSize[0] = elements;
    workGroupSize[0] = 256;

    //Execute the kernel
    status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, indexSpaceSize, workGroupSize, 0, NULL, NULL);

    //Read the device output buffer back to the host output array
    status = clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL);

    //Log the output
    for (int i = 0; i < elements; i++)
    {
        printf("%d " , C[i]);
    }

    //Free OpenCL resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseContext(context);

    //Free host resources
    free(A);
    free(B);
    free(C);

    return 0;
}
