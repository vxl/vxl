/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_resize.h>

void test_resize() {
  {
    vnl_vector<double> X(3);
    
    X.fill(2);
    cerr << "X = " << X << endl;
    
    vnl_resize(X,5);
    cerr << "X = " << X << endl;
  }
  
  {
    vnl_matrix<double> M(3,4);
    
    M.fill(2);
    cerr << "M = " << endl << M << endl;
    
    vnl_resize(M,5,7);
    cerr << "M = " << endl << M << endl;
  }
}

TESTMAIN(test_resize);
