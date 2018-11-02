// This is mul/clsfy/tests/test_binary_hyperplane_logit.cxx
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_binary_hyperplane_logit_builder class
// \author TFC

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <clsfy/clsfy_binary_hyperplane_logit_builder.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <mbl/mbl_data_array_wrapper.h>

//: Tests the clsfy_binary_hyperplane_logit_builder class
void test_binary_hyperplane_logit()
{
  std::cout << "**************************************\n"
           << " Testing clsfy_binary_hyperplane_logit_builder_builder\n"
           << "**************************************\n";

  // Create data
  unsigned n_egs=50;
  unsigned n_dim=10;
  vnl_random rand1(3857);
  std::vector<vnl_vector<double> > data(n_egs);
  std::vector<unsigned> outputs(n_egs);

  vnl_vector<double> mean1(n_dim,0.0),mean2(n_dim,1.1),var(n_dim,n_dim);

  vpdfl_axis_gaussian pdf1,pdf2;
  pdf1.set(mean1,var);
  pdf2.set(mean2,var);
  vpdfl_axis_gaussian_sampler sampler1,sampler2;
  sampler1.set_model(pdf1);
  sampler2.set_model(pdf2);

  for (unsigned i=0;i<n_egs;++i)
  {
    if (i%2==0)
    {
      sampler1.sample(data[i]);
      outputs[i]=1;
    }
    else
    {
      sampler2.sample(data[i]);
      outputs[i]=0;
    }
  }

  mbl_data_array_wrapper<vnl_vector<double> > data1(data);

  clsfy_binary_hyperplane_logit_builder builder;
  builder.set_alpha(0.1);
  clsfy_binary_hyperplane classifier;
  double mean_error = builder.build(classifier,data1,outputs);
  std::cout<<"Mean error: "<<mean_error<<std::endl;
}

TESTMAIN(test_binary_hyperplane_logit);
