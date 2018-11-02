// This is mul/m23d/tests/test_rotation_from_ortho_projection.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <m23d/m23d_rotation_from_ortho_projection.h>
#include <m23d/m23d_rotation_matrix.h>

void test_rotation_from_ortho_projection()
{
  std::cout<<"==== test m23d_rotation_from_ortho_projection ====="<<std::endl;

  vnl_matrix<double> R1 = m23d_rotation_matrix(0.1,0.2,0.3);

  // Generate identity projection
  vnl_matrix<double> PI(2,3),Pzero(2,3,0.0);
  PI(0,0)=1; PI(0,1)=0; PI(0,2)=0;
  PI(1,0)=0; PI(1,1)=1; PI(1,2)=0;

  vnl_matrix<double> P1=PI*R1;

  // Attempt to recover R1 from P1
  vnl_matrix<double> R1a = m23d_rotation_from_ortho_projection(P1);

  TEST_NEAR("Recovered R1",(R1a-R1).rms(),0.0,1e-6);

  std::cout<<"Recover R1 from scaled projection"<<std::endl;
  vnl_matrix<double> R1b = m23d_rotation_from_ortho_projection(0.19*P1);
  TEST_NEAR("Recovered R1",(R1b-R1).rms(),0.0,1e-6);

  std::cout<<"Recover R1 from reflected projection"<<std::endl;
  vnl_matrix<double> R1c = m23d_rotation_from_ortho_projection(-0.19*P1);
  std::cout<<"R1:"<<R1<<std::endl
          <<"R1c:"<<R1c<<std::endl;
  TEST_NEAR("Recovered R1 (1st 2 rows)",(R1c+R1).extract(2,3).rms(),0.0,1e-6);
  vnl_matrix<double> P1R1t=(-0.19*P1)*R1c.transpose();
  std::cout<<"R1R1t:"<<P1R1t<<std::endl;
}

TESTMAIN(test_rotation_from_ortho_projection);
