/*
  fsm@robots.ox.ac.uk
*/
#include <vnl/vnl_vector.h>
#include <iostream.h>

int main(int, char **) {
  vnl_vector<double> a;
  vnl_vector<double> b(3);

  b.fill(0);
  cerr << "b=" << b << endl;

  b.fill(14);
  cerr << "b=" << b << endl;
  
  return 0;
}
