// This is mul/mbl/tests/test_rvm_regression_builder.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_rvm_regression_builder.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_random.h>

void test_rvm_regression_builder()
{
  vcl_cout << "********************\n"
           << " Testing mbl_rvm_regression_builder\n"
           << "********************\n";

  // Generate some sinusoidal data
  unsigned ns = 100;
  vnl_random mz_rand(364982);
  vcl_vector<vnl_vector<double> > samples(ns);
  vnl_vector<double> target(ns);
  for (unsigned i=0;i<ns;++i)
  {
    // (sin(|x|))/|x|)
    double x = 0.001+20.0*(double(i)-(ns/2))/ns;
    samples[i].set_size(1);
    target[i]=vcl_sin(vcl_fabs(x))/vcl_fabs(x) + mz_rand.normal()*0.03;
    samples[i][0]=x;
  }

  mbl_data_array_wrapper<vnl_vector<double> > data(&samples[0],ns);

  mbl_rvm_regression_builder builder;

  vcl_vector<int> index;
  vnl_vector<double> weights;
  double sqr_width;

  double var = 0.2;
  builder.gauss_build(data,var,target,index,weights,sqr_width);

  vcl_cout<<"Number of kernels chosen: "<<index.size()<<" from "<<ns<<vcl_endl;
  vcl_cout<<"Variance: "<<sqr_width<<vcl_endl;
  vcl_cout<<"Centred on:"<<vcl_endl;
  for (unsigned i=0;i<index.size();++i)
    vcl_cout<<samples[index[i]][0]<<" wt: "<<weights[i+1]<<vcl_endl;

  TEST("var=0.2 Select 5 vectors",index.size(),5);

  var=1.0;
  builder.gauss_build(data,var,target,index,weights,sqr_width);
  vcl_cout<<"Number of kernels chosen: "<<index.size()<<" from "<<ns<<vcl_endl;
  vcl_cout<<"Variance: "<<sqr_width<<vcl_endl;
  TEST("var=1.0 Select 10 vectors",index.size(),10);

}

TESTMAIN(test_rvm_regression_builder);
