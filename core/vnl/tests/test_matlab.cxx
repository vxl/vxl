/*
  fsm@robots.ox.ac.uk
*/
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matlab.h>

void test_matlab() {
  vnl_vector<int> v(7);
  for (int i=0;i<v.size();i++)
    v(i)=i;

  cerr << v << endl;
  cerr << matlab(v) << endl;

  vnl_matrix<int> M(6,8);
  for (int i=0;i<M.rows();i++)
    for (int j=0;j<M.columns();j++)
      M(i,j)=i*j;

  cerr << M << endl;
  cerr << matlab(M) << endl;
}

TESTMAIN(test_matlab);
