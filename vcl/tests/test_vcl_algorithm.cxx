/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_new.h>

struct X 
{
  double *p;
  X() { p = new double[37]; }
  ~X() { delete [] p; }
};

int main(int, char **) 
{
  X my_x;
  
  double v[5] = {1,5,2,4,3};
  vcl_sort(v, v+5);

  vcl_destroy(&my_x);
  new (&my_x) X;

  return 0;
}

#if defined(VCL_GCC_27)
VCL_INSTANTIATE_INLINE(void destroy(X *));
#endif
