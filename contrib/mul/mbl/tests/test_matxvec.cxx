// This is mul/mbl/tests/test_matxvec.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_matxvec.h>
#include <testlib/testlib_test.h>


void test1()
{
  std::cout << "\ntest1():\n";

  vnl_matrix<double> A(4,5);
  for (unsigned int i=0;i<A.rows();++i)
    for (unsigned int j=0;j<A.cols();++j)
      A(i,j) = double(i+j*j);

  vnl_vector<double> b(3),x;
  for (unsigned int i=0;i<b.size();++i) b(i)=1+i;

  mbl_matxvec_prod_mv(A,b,x);
  TEST("mbl_matxvec_prod_mv size()",x.size(), 4);
  TEST_NEAR("mbl_matxvec_prod_mv x(0)", x(0), 14, 1e-8);
  TEST_NEAR("mbl_matxvec_prod_mv x(1)", x(1), 20, 1e-8);
  TEST_NEAR("mbl_matxvec_prod_mv x(2)", x(2), 26, 1e-8);
  TEST_NEAR("mbl_matxvec_prod_mv x(3)", x(3), 32, 1e-8);

  vnl_vector<double> c(5);
  for (unsigned int i=0;i<c.size();++i) c(i)=1+i;

  mbl_matxvec_prod_vm(c,A.transpose(),x);
  TEST("mbl_matxvec_prod_vm size()",x.size(), 4);
  TEST_NEAR("mbl_matxvec_prod_vm x(0)", x(0), 130, 1e-8);
  TEST_NEAR("mbl_matxvec_prod_vm x(1)", x(1), 145, 1e-8);
  TEST_NEAR("mbl_matxvec_prod_vm x(2)", x(2), 160, 1e-8);
  TEST_NEAR("mbl_matxvec_prod_vm x(3)", x(3), 175, 1e-8);
}


// Check consistency with full vector-matrix multiplication
void test2()
{
  std::cout << "\ntest2():\n";

  vnl_matrix<double> A(2,2);
  A(0,0) = 1;  A(0,1) = 3;
  A(1,0) = 5;  A(1,1) = 7;

  vnl_vector<double> b(2);
  b(0) = 1;
  b(1) = 100;

  vnl_vector<double> x1(2), x2(2);

  mbl_matxvec_prod_vm(b,A,x1);
  //std::cout << "prod_vm: b * A = " << x1 << '\n';
  x2 = b*A;
  //std::cout << "std: b * A = " << x2 << '\n';
  TEST_NEAR("prod_vm same as v*M", (x1-x2).magnitude(), 0.0, 1e-8);

  mbl_matxvec_prod_mv(A,b,x1);
  //std::cout << "prod_mv: A * b = " << x1 << '\n';
  x2 = A*b;
  //std::cout << "std: A * b = " << x2 << '\n';
  TEST_NEAR("prod_mv same as M*v", (x1-x2).magnitude(), 0.0, 1e-8);
}


void test_matxvec()
{
  std::cout << "*********************\n"
           << " Testing mbl_matxvec\n"
           << "*********************\n";

  test1();
  test2();
}

TESTMAIN(test_matxvec);
