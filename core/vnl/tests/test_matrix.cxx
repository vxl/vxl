/*
  fsm@robots.ox.ac.uk
*/
#include <vnl/vnl_matrix.h>
#include <iostream.h>

int main(int, char **) {
  vnl_matrix<double> A(3,5);

  A.fill(2);
  cerr << "A=" << A << endl;
  
  return 0;
}
