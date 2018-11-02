// This is mul/clsfy/tests/test_logit_loss_function.cxx
#include <iostream>
#include <string>
#include <testlib/testlib_test.h>
//:
// \file
// \brief Tests the clsfy_logit_loss_function class
// \author TFC

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <clsfy/clsfy_logit_loss_function.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <mbl/mbl_data_array_wrapper.h>

//: Tests the clsfy_logit_loss_function class
void test_logit_loss_function()
{
  std::cout << "**************************************\n"
           << " Testing clsfy_logit_loss_function_builder\n"
           << "**************************************\n";

  // Create data
  unsigned n_egs=20;
  unsigned n_dim=10;
  vnl_random rand1(3857);
  std::vector<vnl_vector<double> > data(n_egs);
  vnl_vector<double> class_id(n_egs);

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
      class_id[i]=1.0;
    }
    else
    {
      sampler2.sample(data[i]);
      class_id[i]=-1.0;
    }
  }

  double min_p=0.001, alpha=1.0;
  mbl_data_array_wrapper<vnl_vector<double> > data1(data);
  clsfy_quad_regulariser quad_reg(alpha);
  clsfy_logit_loss_function fn(data1,class_id,min_p,&quad_reg);

  vnl_vector<double> w(n_dim+1,0.5);
  for (unsigned i=0;i<=n_dim;++i) w[i]=0.1*(i+1);
  std::cout<<"f(w)="<<fn.f(w)<<std::endl;

  // Test the gradient
  double f0 = fn.f(w);
  vnl_vector<double> gradient;
  fn.gradf(w,gradient);
  double d = 1e-6;
  for (unsigned i=0;i<=n_dim;++i)
  {
    w[i]+=d;
    double gi = (fn.f(w)-f0)/d;
    w[i]-=d;

    TEST_NEAR("Gradient",gradient[i],gi,1e-5);
  }

  double f2;
  vnl_vector<double> g2;
  fn.compute(w,&f2,&g2);

  TEST_NEAR("compute f",f0,f2,1e-6);
  TEST_NEAR("compute g",(gradient-g2).rms(),0,1e-5);
}

TESTMAIN(test_logit_loss_function);
