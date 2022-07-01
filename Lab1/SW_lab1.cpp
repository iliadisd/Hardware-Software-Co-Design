#include <iostream>
using namespace std;

typedef int input_type;

#define lm  8
#define ln  8
#define lp  8
#define m 1<<lm
#define n 1<<ln
#define p 1<<lp

void mult_sw (input_type in1 [m-1][n-1],
input_type in2 [n-1][p-1],
int out [m-1][p-1])
{
  for (int i = 0 ; i < m-1; i++){
    for (int j = 0; j < p-1; j++){
      int result = 0;
      for (int k = 0; k < n-1; k++){
        result += in1[i][k] * in2[k][j];
      }
      out[i][j] = result;
    }
  }
}
