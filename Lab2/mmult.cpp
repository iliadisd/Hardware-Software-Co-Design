#include <iostream>
using namespace std;

#define lm  6
#define ln  6
#define lp  6
#define m 1<<lm
#define n 1<<ln
#define p 1<<lp
#define BUFFER_SIZE 16

const unsigned int c_len = m / BUFFER_SIZE;
const unsigned int c_size = BUFFER_SIZE;

extern "C" {
void mult_hw (int in1[m-1][n-1],
int in2[n-1][p-1],
int out[m-1][p-1])
{
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
//#pragma HLS INTERFACE s_axilite port = size bundle = control
//#pragma HLS INTERFACE s_axilite port = dim1 bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

int buffer_in1[BUFFER_SIZE][BUFFER_SIZE];
int buffer_in2[BUFFER_SIZE][BUFFER_SIZE];
int buffer_output[BUFFER_SIZE][BUFFER_SIZE];

/*
 #pragma HLS ARRAY_PARTITION variable=in1 cyclic factor = 128 dim = 2
 #pragma HLS ARRAY_PARTITION variable=in2 cyclic factor = 128 dim = 1

  for (int i = 0 ; i < m-1; i++){
    #pragma HLS loop_tripcount min=255 max=255
    for (int j = 0; j < p-1; j++){
      #pragma HLS loop_tripcount min=255 max=255
	  #pragma HLS PIPELINE II=1
      int result = 0;
      for (int k = 0; k < n-1; k++){
	   // #pragma HLS unroll factor = 256
        result += in1[i][k] * in2[k][j];
      }
      out[i][j] = result;
    }
  }
}

} */

for (int i = 0; i < m; i+= BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = 4 max = 4
  int chunk_size = BUFFER_SIZE;
  // boundary checks
  if ((i + BUFFER_SIZE) > m)
    chunk_size = m - i;

// Transferring data in bursts hides the memory access latency as well as
// improves bandwidth utilization and efficiency of the memory controller.
// It is recommended to infer burst transfers from successive requests of data
// from consecutive address locations.
// A local memory vl_local is used for buffering the data from a single burst.
// The entire input vector is read in multiple bursts.
// The choice of LOCAL_MEM_SIZE depends on the specific applications and
// available on-chip memory on target FPGA.
// burst read of v1 and v2 vector from global memory


    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      for (int k = 0; k < chunk_size; k++){
        #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        #pragma HLS PIPELINE II = 1
    	  buffer_in1[j][k] = in1[i + j][i + k];
    }
  }


  for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
    for (int k = 0; k < chunk_size; k++){
      #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      #pragma HLS PIPELINE II = 1
    	buffer_in2[j][k] = in2[i + j][i + k];
    }
  }

  // PIPELINE pragma reduces the initiation interval for loop by allowing the

    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
    for (int k = 0; k < chunk_size; k++){
    	int result = 0;
  #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      for (int l = 0; l < chunk_size; l++){
        #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
  #pragma HLS PIPELINE II = 1
    	  result+= buffer_in1[j][l] *  buffer_in2[l][k];

    }
      buffer_output[j][k] = result;
    }
}

  // burst write the result

    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
  for (int k = 0; k < chunk_size; k++){
    #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II = 1
      out[i + j][i + k] = buffer_output[j][k];
    }
  }
}
}
}

















