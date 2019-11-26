// This is mul/msm/tests/test_wt_mat_2d.cxx
//=======================================================================
//
//  Copyright: (C) 2010 The University of Manchester
//
//=======================================================================
#include <iostream>
#include "testlib/testlib_test.h"
//:
// \file
// \author Tim Cootes
// \brief test msm_wt_mat_2d

#include <msm/msm_wt_mat_2d.h>
#include "vnl/vnl_matrix.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

//=======================================================================

void test_wt_mat_2d()
{
  std::cout << "***********************\n"
           << " Testing msm_wt_mat_2d\n"
           << "***********************\n";

  msm_wt_mat_2d I;
  TEST_NEAR("Identity",I.m11(),1.0,1e-6);

  msm_wt_mat_2d W2(1,2,5);
  msm_wt_mat_2d W2_inv = W2.inverse();
  TEST("Inverse 1",W2*W2_inv==I,true);
  TEST("Inverse 2",W2_inv*W2==I,true);

  W2.set_axes(0.3,0.8,  1.23,  4.12);
  double ev1,ev2;
  W2.eigen_values(ev1,ev2);
  TEST_NEAR("Largest eigenvalue",ev1,4.12, 1e-6);
  TEST_NEAR("Smallest eigenvalue",ev2,1.23, 1e-6);

  vnl_matrix<double> R(2,2),M(2,2),RtMR;
  double a=1.3, b=0.4;
  R(0,0)=a; R(0,1)=-b;
  R(1,0)=b; R(1,1)= a;
  M(0,0)=W2.m11(); M(0,1)=W2.m12();
  M(1,0)=W2.m21(); M(1,1)=W2.m22();
  RtMR=R*M*R.transpose()/(a*a+b*b);
  msm_wt_mat_2d W3=W2.transform_by(a,b);
  TEST("Rt*W*R",W3==msm_wt_mat_2d(RtMR(0,0),RtMR(0,1),RtMR(1,1)),true);

  // Rotation by 30 degrees
  double u1=std::cos(3.14152857/6);
  double u2=std::sin(3.14152857/6);

  // Test calculation of eigenvector.
  W3.set_axes(u1,u2,2,1);
  vgl_vector_2d<double> evec1;
  W3.eigen_vector(evec1,ev1,ev2);
  std::cout<<"EVec: "<<evec1<<std::endl;
  std::cout<<"Evals: "<<ev1<<", "<<ev2<<std::endl;

  TEST_NEAR("Eigenvector (x)",evec1.x(),u1, 1e-4);
  TEST_NEAR("Eigenvector (y)",evec1.y(),u2, 1e-4);

  // Check the inverse
  W3.inverse().eigen_vector(evec1,ev1,ev2);
  TEST_NEAR("EVal1 of inverse",ev1,1, 1e-4);
  TEST_NEAR("EVal2 of inverse",ev2,0.5, 1e-4);
  TEST_NEAR("EVec of inverse at right angles",u1*evec1.x()+u2*evec1.y(),0, 1e-4);

  // Test rotation
  W3.set_axes(1,0,2,1);  // Variance 2 along x axis

  std::cout<<"Test rotation by 30 degrees"<<std::endl;
  msm_wt_mat_2d W4=W3.transform_by(u1,u2);

  W4.eigen_vector(evec1,ev1,ev2);
  TEST_NEAR("Eigenvector (x)",evec1.x(),u1, 1e-4);
  TEST_NEAR("Eigenvector (y)",evec1.y(),u2, 1e-4);
  TEST_NEAR("EVal1",ev1,2, 1e-4);
  TEST_NEAR("EVal2",ev2,1, 1e-4);


}

TESTMAIN(test_wt_mat_2d);
