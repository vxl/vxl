// This is mul/m23d/tests/test_rotation_matrix.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <m23d/m23d_rotation_matrix.h>


void test_rotation_matrix()
{
  vcl_cout<<"==== test m23d_rotation_matrix ====="<<vcl_endl;

  vnl_matrix<double> I3(3,3);
  I3.set_identity();

  vnl_matrix<double> Rx=m23d_rotation_matrix_x(0.17);
  TEST_NEAR("Rx*Rx'=I",(Rx*Rx.transpose()-I3).rms(),0,1e-6);

  vnl_matrix<double> Ry=m23d_rotation_matrix_y(0.13);
  TEST_NEAR("Ry*Ry'=I",(Ry*Ry.transpose()-I3).rms(),0,1e-6);

  vnl_matrix<double> Rz=m23d_rotation_matrix_z(0.11);
  TEST_NEAR("Rz*Rz'=I",(Rz*Rz.transpose()-I3).rms(),0,1e-6);
}

TESTMAIN(test_rotation_matrix);
