/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_new.h>

struct X_s
{
  double *p;
  X_s() { p = new double[37]; }
  ~X_s() { delete [] p; }
};

int test_vcl_new_main()
{
  X_s my_x;

  vcl_destroy(&my_x);
  new (&my_x) X_s; // vcl_construct(&my_x);

  return 0;
}
