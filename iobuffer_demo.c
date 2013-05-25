//
// host buffer I/O demo
//
#include "opencl_helper.h"
#include <stdlib.h>

#define MAX_XOR_DATACOUNT (1024)

OpenCL_Config gstConfig;

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define M_ASSERT_CLSUCCESS(iRet, func_name) if (iRet != CL_SUCCESS) { \
    printf("%s error, iRet=%d: %s\n", func_name, iRet, GetCLErrString(iRet)); \
    return -__LINE__; \
}

static void PrintArrayBuffer(cl_uint *buffer, cl_int num)
{
    cl_int i;

    printf("%.4X: ", 0);
    for (i = 0; i < num; i++)
    {
        if ((i != 0) && (i % 4 == 0))
            printf("\n%.4X: ", i*4);
        printf("%.8x ", buffer[i]);
    }
    printf("\n");
}

int main(int argc, char ** argv)
{
    OpenCL_Config *pstConfig = &gstConfig;

	cl_int i, iRet;
    cl_kernel kernel;

    cl_uint value = 0;
	cl_uint *input = NULL;				// host input buffer ptr
	cl_uint *output = NULL;				// host output buffer ptr
	cl_mem inputBuffer;
	cl_mem outputBuffer;

    size_t work_size = 0;
    cl_event kernel_ready;
    cl_event buffer_ready;

	const char *kernel_name = "kernel.cl";

    // Init OpenCL with default GPU device
	iRet = clInitOpenCLByDefault(pstConfig, CL_DEVICE_TYPE_GPU);
    M_ASSERT_CLSUCCESS(iRet, "clInitOpenCLByDefault()");

	iRet = clBuildProgramFromSource(pstConfig, kernel_name);
    M_ASSERT_CLSUCCESS(iRet, "clBuildProgramFromSource()");

    // load kernel procedure
	kernel = clCreateKernel( pstConfig->program, "xor", NULL );

	///
	///  malloc i/o buffer
	///
	// 1. malloc host buffer
	input = (cl_uint*) malloc( MAX_XOR_DATACOUNT*sizeof(cl_uint) );
	output = (cl_uint*) malloc( MAX_XOR_DATACOUNT*sizeof(cl_uint) );
	if ((input == NULL) or (output == NULL))
	{
		printf("malloc() host i/o buffer error, input=%p output=%p\n", input, output);
		return -11;
	}

    // Init input buffer
    for (i = 0; i < MAX_XOR_DATACOUNT; i++)
    {
        input[i] = (cl_uint)(rand()<<16 ^ rand());
    }

    printf("--- INPUT ---\n");
    PrintArrayBuffer(input, 32);
    
	// 2. alloc OpenCL buffer
    inputBuffer = clCreateBuffer(pstConfig->context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, MAX_XOR_DATACOUNT*sizeof(cl_uint), input, &iRet);
    M_ASSERT_CLSUCCESS(iRet, "clCreateBuffer(inputBuffer)");

    outputBuffer = clCreateBuffer(pstConfig->context, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, MAX_XOR_DATACOUNT*sizeof(cl_uint), output, &iRet);
    M_ASSERT_CLSUCCESS(iRet, "clCreateBuffer(outputBuffer)");

    // 3. set kernel args
    // i/o buffer
    iRet = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
    M_ASSERT_CLSUCCESS(iRet, "clSetKernelArg(inputBuffer)");
    iRet = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
    M_ASSERT_CLSUCCESS(iRet, "clSetKernelArg(outputBuffer)");
    
    // arg - value
    value = 0xF173371F;
    iRet = clSetKernelArg(kernel, 2, sizeof(cl_uint), (void *)&value);
    M_ASSERT_CLSUCCESS(iRet, "clSetKernelArg(outputBuffer)");
    

    // 4. run kernel and wait event
    work_size = MAX_XOR_DATACOUNT;
    iRet = clEnqueueNDRangeKernel(pstConfig->queue, kernel, 1, NULL, &work_size, NULL, 0, NULL, &kernel_ready);
    M_ASSERT_CLSUCCESS(iRet, "clEnqueueNDRangeKernel()");

    // wait for the kernel call to finish execution
    iRet = clWaitAndReleaseOneEvent(kernel_ready);
    M_ASSERT_CLSUCCESS(iRet, "clWaitAndReleaseOneEvent()");


    // 5. read output to host
    iRet = clEnqueueReadBuffer(pstConfig->queue, outputBuffer, CL_TRUE, 0, MAX_XOR_DATACOUNT*sizeof(cl_uint), output, 0, NULL, &buffer_ready);
    M_ASSERT_CLSUCCESS(iRet, "clEnqueueReadBuffer()");
    
    // Wait for the read buffer to finish execution
    iRet = clWaitAndReleaseOneEvent(buffer_ready);
    M_ASSERT_CLSUCCESS(iRet, "clWaitAndReleaseOneEvent()");

    printf("\n--- OUTPUT ---\n");
    PrintArrayBuffer(output, 32);

	return clFreeOpenCLResources(pstConfig);
}
