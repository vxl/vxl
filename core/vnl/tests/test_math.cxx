/*
  fsm@robots.ox.ac.uk
*/

#include <vnl/vnl_complex.h>
#include <vnl/vnl_math.h>
#include <vcl/vcl_iostream.h>

int main(int,char**) {
  vnl_double_complex z(1,2);
  
  cerr << "z=" << z << endl;
  cerr << "abs(z)=" << vnl_math::abs(z) << endl;
  
  return 0;
}
