// This is mul/m23d/tests/test_make_ortho_projection.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <m23d/m23d_make_ortho_projection.h>

void test_make_ortho_projection()
{
  vcl_cout<<"==== test m23d_make_ortho_projection ====="<<vcl_endl;

  vnl_matrix<double> P=m23d_make_ortho_projection(0.17,-0.23,0.1);
  vnl_matrix<double> I2(2,2);
  I2.set_identity();
  TEST_NEAR("P*P'=I",(P*P.transpose()-I2).rms(),0,1e-6);

  vnl_matrix<double> P0=m23d_make_ortho_projection(0.0,0.0,0.0);
  vnl_matrix<double> P0_true(2,3);
  P0_true.fill(0.0);
  P0_true(0,0)=1.0;
  P0_true(1,1)=1.0;

  TEST_NEAR("P0=(I|0)",(P0-P0_true).rms(),0,1e-6);
}

TESTMAIN(test_make_ortho_projection);
