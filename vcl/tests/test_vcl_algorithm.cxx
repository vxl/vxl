/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_algorithm.h>

int test_vcl_algorithm_main() 
{
  double v[5] = {1,5,2,4,3};
  vcl_sort(v, v+5);

  return 0;
}
