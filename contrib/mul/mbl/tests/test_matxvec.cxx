// This is mul/mbl/tests/test_matxvec.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_matxvec.h>
#include <testlib/testlib_test.h>

void test_matxvec()
{
  vcl_cout << "*********************\n"
           << " Testing mbl_matxvec\n"
           << "*********************\n";

  vnl_matrix<double> A(4,5);
  for (unsigned int i=0;i<A.rows();++i)
     for (unsigned int j=0;j<A.cols();++j)
         A(i,j) = double(i+j*j);

  vnl_vector<double> b(3),x;
  for (unsigned int i=0;i<b.size();++i) b(i)=1+i;

  mbl_matxvec_prod_mv(A,b,x);
  TEST("mbl_matxvec_prod_mv size()",x.size()==4,true);
  TEST("mbl_matxvec_prod_mv x(0)",vcl_fabs(x(0)-14)<1e-8,true);
  TEST("mbl_matxvec_prod_mv x(1)",vcl_fabs(x(1)-20)<1e-8,true);
  TEST("mbl_matxvec_prod_mv x(2)",vcl_fabs(x(2)-26)<1e-8,true);
  TEST("mbl_matxvec_prod_mv x(3)",vcl_fabs(x(3)-32)<1e-8,true);

  vnl_vector<double> c(5);
  for (unsigned int i=0;i<c.size();++i) c(i)=1+i;

  mbl_matxvec_prod_vm(c,A.transpose(),x);
  TEST("mbl_matxvec_prod_vm size()",x.size()==4,true);
  TEST("mbl_matxvec_prod_vm x(0)",vcl_fabs(x(0)-130)<1e-8,true);
  TEST("mbl_matxvec_prod_vm x(1)",vcl_fabs(x(1)-145)<1e-8,true);
  TEST("mbl_matxvec_prod_vm x(2)",vcl_fabs(x(2)-160)<1e-8,true);
  TEST("mbl_matxvec_prod_vm x(3)",vcl_fabs(x(3)-175)<1e-8,true);
}

TESTMAIN(test_matxvec);
