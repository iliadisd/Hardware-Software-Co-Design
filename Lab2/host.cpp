#include "xcl2.hpp"
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

#define lm  6
#define ln  6
#define lp  6
#define m 1<<lm
#define n 1<<ln
#define p 1<<lp
void mult_hw (int in1[m-1][n-1], int in2[n-1][p-1], int out[m-1][p-1]);

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string binaryFile = argv[1];

  cl_int err;
  cl::Context context;
  cl::Kernel krnl_mult_hw;
  cl::CommandQueue q;
  // Allocate Memory in Host Memory
  // When creating a buffer with user pointer (CL_MEM_USE_HOST_PTR), under the
  // hood user ptr
  // is used if it is properly aligned. when not aligned, runtime had no choice
  // but to create
  // its own host side buffer. So it is recommended to use this allocator if
  // user wish to
  // create buffer using CL_MEM_USE_HOST_PTR to align user buffer to page
  // boundary. It will
  // ensure that user buffer is used when user create Buffer/Mem object with
  // CL_MEM_USE_HOST_PTR

  // Create the test data
  int in1[m-1][n-1];
  int in2[n-1][p-1];
  int out[m-1][p-1];
  int sw_result [m-1][p-1];
  //int hw_result [m-1][p-1];

  for(int i = 0; i < m-1; i++)
  {
    for(int j = 0; j < n-1; j++)
    {
      in1[i][j] = rand() % 255;
    }
  }

  for(int i = 0; i < n-1; i++)
  {
    for(int j = 0; j < p-1; j++)
    {
      in2[i][j] = rand() % 255;
    }
  }

  for(int i = 0; i < m-1; i++)
  {
    for(int j = 0; j < p-1; j++)
    {
      sw_result[i][j]= 0;
     // hw_result[i][j]= 0;
      out[i][j] = 0;

    }
  }

  // OPENCL HOST CODE AREA START
  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();
  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE, &err));
    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      OCL_CHECK(err, krnl_mult_hw = cl::Kernel(program, "mult_hw", &err));
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err, cl::Buffer buffer_in1(
           	   	   	 context, CL_MEM_READ_ONLY,
			   	   	 m*m*sizeof(int), NULL, &err));
  OCL_CHECK(err, cl::Buffer buffer_in2(
                     context, CL_MEM_READ_ONLY,
					 m*m*sizeof(int), NULL, &err));
  OCL_CHECK(err, cl::Buffer buffer_output(
                     context, CL_MEM_WRITE_ONLY,
					 m*m*sizeof(int), NULL, &err));

 // size_t size = m-1;
  OCL_CHECK(err, err = krnl_mult_hw.setArg(0, buffer_in1));
  OCL_CHECK(err, err = krnl_mult_hw.setArg(1, buffer_in2));
  OCL_CHECK(err, err = krnl_mult_hw.setArg(2, buffer_output));
  //OCL_CHECK(err, err = krnl_mult_hw.setArg(3, size));

  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},
                                                  0 /* 0 means from host*/));

  // Launch the Kernel
  // For HLS kernels global and local size is always (1,1,1). So, it is
  // recommended
  // to always use enqueueTask() for invoking HLS kernel
  OCL_CHECK(err, err = q.enqueueTask(krnl_mult_hw));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  // OPENCL HOST CODE AREA END

  // Compare the results of the Device to the simulation
  bool match = true;
  for (int i = 0; i < m-1; i++){
    for (int j = 0; j< p-1; j++){
      if (sw_result[i][j] != out[i][j]){
  /*      std::cout << "Mismatch on " << "Row:" << i << "Col:" << j;
        std::cout << "Software output : " << sw_result[i][j] <<
        "\t Hardware output; " << out[i][j] << std::endl;
        */
        match = false;
        break;
      } else {
       /*   std::cout << "CPU output : " << sw_result[i][j] <<
          "\t Hardware output; " << out[i][j] << std::endl;
          */
          break;
      }
    }
  }
std::cout << " TEST " << (match? "PASSED" : "FAILED") << std::endl;
return(match? EXIT_SUCCESS : EXIT_FAILURE);
}

