// This is mul/mbl/tests/test_rbf_network.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for fabs()
#include <vcl_cassert.h>
#include <vcl_vector.h>

#include <vnl/vnl_random.h>
#include <mbl/mbl_rbf_network.h>
#include <vnl/vnl_vector.h>

#include <testlib/testlib_test.h>

void test_rbf_network()
{
  vcl_cout << "\n*************************\n"
           <<   " Testing mbl_rbf_network\n"
           <<   "*************************\n";

  vnl_random rng;
  rng.reseed(123456);

//const unsigned nCentres = 64;
  const unsigned nSamples = 100;
  const unsigned nDims = 3;
  const unsigned nTests = 3;


  vcl_vector<vnl_vector<double> >
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
    vcl_cout<<"\nWe pick sample "<<i+1<<" from training.\n"
            <<"The input data: "<<data[i]<<vcl_endl;
    rbf.calcWts(w,data[i]);//"It fails here"
    TEST_NEAR("Target weight near unity",w[i],1.0,1e-4);
    TEST_NEAR("Sum of weights near unity",w.sum(),1.0,1e-4);
    double wts=-w[i]; for (unsigned int j=0; j<nSamples; ++j) wts += vcl_fabs(w[j]);
    TEST_NEAR("All other weights near zero",wts,0.0,1e-4);
    vcl_cout<<"First 10 weights:";
    for (unsigned j=0;j<10;++j) vcl_cout<<' '<<w[j];
    vcl_cout<<vcl_endl;
  }
}

TESTMAIN(test_rbf_network);
