// This is mul/mbl/tests/test_rbf_network.cxx
#include <iostream>
#include <cmath>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vnl/vnl_random.h>
#include <mbl/mbl_rbf_network.h>
#include <vnl/vnl_vector.h>

#include <testlib/testlib_test.h>

void test_rbf_network()
{
  std::cout << "\n*************************\n"
           <<   " Testing mbl_rbf_network\n"
           <<   "*************************\n";

  vnl_random rng;
  rng.reseed(123456);

//const unsigned nCentres = 64;
  constexpr unsigned nSamples = 100;
  constexpr unsigned nDims = 3;
  constexpr unsigned nTests = 3;


  std::vector<vnl_vector<double> >
    data(nSamples, vnl_vector<double>(nDims, 0.0));

  // sample data uniformly from unit cube
  for (unsigned int i=0; i<nSamples; ++i)
    for (unsigned int j=0; j<nDims; ++j) {
      data[i](j) = rng.drand32(0.0, 1.0);
      assert ( data[i](j) >= 0 && data[i](j) < 1);
    }

  mbl_rbf_network rbf;
  rbf.build(data);
  vnl_vector<double > w;

  for (unsigned int i=0; i<nTests ; ++i)
  {
    std::cout<<"\nWe pick sample "<<i+1<<" from training.\n"
            <<"The input data: "<<data[i]<<std::endl;
    rbf.calcWts(w,data[i]);//"It fails here"
    TEST_NEAR("Target weight near unity",w[i],1.0,1e-4);
    TEST_NEAR("Sum of weights near unity",w.sum(),1.0,1e-4);
    double wts=-w[i]; for (unsigned int j=0; j<nSamples; ++j) wts += std::fabs(w[j]);
    TEST_NEAR("All other weights near zero",wts,0.0,1e-4);
    std::cout<<"First 10 weights:";
    for (unsigned j=0;j<10;++j) std::cout<<' '<<w[j];
    std::cout<<std::endl;
  }
}

TESTMAIN(test_rbf_network);
