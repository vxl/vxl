// This is mul/msm/tests/test_param_limiters.cxx
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
// \brief test msm_param_limiter

#include <msm/msm_ellipsoid_limiter.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

void test_ellipsoid_limiter()
{
  std::cout << "***********************\n"
           << " Testing msm_ellipsoid_limiter\n"
           << "***********************\n";

  msm_ellipsoid_limiter limiter;

  unsigned n=5;
  vnl_vector<double> mode_var(n);
  for (unsigned i=0;i<n;++i) mode_var[i]=1.0/(1+i);

  limiter.set_param_var(mode_var);

  double n_sds = 3.0;
  limiter.set_n_sds(n_sds);
  double M = n_sds*n_sds;

  vnl_vector<double> b0,b(n);
  b.fill(0.1);  // Inside
  b0=b;
  limiter.apply_limit(b);
  TEST_NEAR("No change for small b",(b-b0).rms(),0.0,1e-6);

  // Create point outside the ellipsoid
  b.fill(1.0);
  std::cout<<"M="<<limiter.mahalanobis(b)<<std::endl;
  limiter.apply_limit(b);
  TEST_NEAR("Revised point is on surface",
            limiter.mahalanobis(b),M,1e-6);
  std::cout<<b<<std::endl;
  TEST("Point not on line (1,1,1...)",std::fabs(b[0]-b[1])>0.01,true);
}

void test_param_limiters()
{
  test_ellipsoid_limiter();
}

TESTMAIN(test_param_limiters);
