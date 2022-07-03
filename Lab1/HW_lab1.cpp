#include <iostream>
using namespace std;

typedef int input_type;
#define lm  8
#define ln  8
#define lp  8
#define m 1<<lm
#define n 1<<ln
#define p 1<<lp


void mult_hw (input_type in1[m-1][n-1],
input_type in2[n-1][p-1],
int out[m-1][p-1])
{
 //#pragma HLS ARRAY_PARTITION variable=in1 cyclic factor = 64 dim = 2
 //#pragma HLS ARRAY_PARTITION variable=in2 cyclic factor = 64 dim = 1

  for (int i = 0 ; i < m-1; i++){
    #pragma HLS loop_tripcount min=255 max=255
    for (int j = 0; j < p-1; j++){
      //#pragma HLS loop_tripcount min=255 max=255
	  //#pragma HLS PIPELINE II=1
      int result = 0;
      for (int k = 0; k < n-1; k++){
	   // #pragma HLS unroll factor = 256
        result += in1[i][k] * in2[k][j];
      }
      out[i][j] = result;
    }
  }
}
