// This is mul/msm/tests/test_wt_mat_2d.cxx
//=======================================================================
//
//  Copyright: (C) 2010 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test msm_wt_mat_2d

#include <msm/msm_wt_mat_2d.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
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
  RtMR=R.transpose()*M*R;
  msm_wt_mat_2d W3=W2.transform_by(a,b);
  TEST("Rt*W*R",W3==msm_wt_mat_2d(RtMR(0,0),RtMR(0,1),RtMR(1,1)),true);

}

TESTMAIN(test_wt_mat_2d);
