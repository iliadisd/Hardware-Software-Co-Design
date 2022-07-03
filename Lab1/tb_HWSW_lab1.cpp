#include <iostream>
using namespace std;

typedef int input_type;
#define lm  8
#define ln  8
#define lp  8
#define m 1<<lm
#define n 1<<ln
#define p 1<<lp

void mult_sw (input_type in1 [m-1][n-1], input_type in2 [n-1][p-1], int out [m-1][p-1]);
void mult_hw (input_type in1[m-1][n-1], input_type in2[n-1][p-1], int out[m-1][p-1]);
int main (int argc, char** argv)
{
  input_type in1[m-1][n-1];
  input_type in2[n-1][p-1];
  input_type out[m-1][p-1];
  input_type sw_result [m-1][p-1];
  input_type hw_result [m-1][p-1];

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
      hw_result[i][j]= 0;
      out[i][j] = 0;

    }
  }

  //SW
  mult_sw(in1, in2, sw_result);

  //HW
  mult_hw(in1, in2, hw_result);
  //SW/HW
  bool match = true;
  for (int i = 0; i < m-1; i++){
    for (int j = 0; j< p-1; j++){
      if (sw_result[i][j] != hw_result[i][j]){
        std::cout << "Mismatch on " << "Row:" << i << "Col:" << j;
        std::cout << "Software output : " << sw_result[i][j] <<
        "\t Hardware output; " << hw_result[i][j] << std::endl;
        match = false;
        break;
      } else {
          std::cout << "CPU output : " << sw_result[i][j] <<
          "\t Hardware output; " << hw_result[i][j] << std::endl;
          break;
      }
    }
  }
std::cout << " TEST " << (match? "PASSED" : "FAILED") << std::endl;
return(match? EXIT_SUCCESS : EXIT_FAILURE);
}
