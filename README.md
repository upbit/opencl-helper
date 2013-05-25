opencl-helper
=============

A simple OpenCL example written in C

usage:
~~~~~ c
    OpenCL_Config gstConfig;
    OpenCL_Config *pstConfig = &gstConfig;

    // Init OpenCL with default GPU device
    clInitOpenCLByDefault(pstConfig, CL_DEVICE_TYPE_GPU);
    clBuildProgramFromSource(pstConfig, "kernel.cl");

    // clEnqueueNDRangeKernel() ..

    clFreeOpenCLResources(pstConfig);
~~~~~
