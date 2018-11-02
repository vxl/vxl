// This is mul/vpdfl/tests/test_axis_gaussian.cxx
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test vpdfl_axis_gaussian, building, sampling, saving, etc.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vpdfl/vpdfl_axis_gaussian_sampler.h>
#include <vpdfl/vpdfl_add_all_binary_loaders.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vsl/vsl_binary_loader.h>
#include <vpl/vpl.h> // vpl_unlink()

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

static void test_gradient(vpdfl_pdf_base& pdf, const vnl_vector<double>& x0)
{
  double p0 = pdf(x0);
  vnl_vector<double> g;
  double p;
  pdf.gradient(g,x0,p);
  TEST_NEAR("gradient produces correct probability",p,p0,1e-6);

  vnl_vector<double> x=x0;
  double d = 1e-6;
  for (unsigned i=0;i<x0.size();++i)
  {
    x=x0;
    x[i]+=d;
    double gi = (pdf(x)-p0)/d;
    TEST_NEAR("Gradient correct",gi,g[i],1e-6);
  }
}

static void test_gradient_logp(vpdfl_pdf_base& pdf, const vnl_vector<double>& x0)
{
  double p0 = pdf.log_p(x0);
  vnl_vector<double> g;
  pdf.gradient_logp(g,x0);

  vnl_vector<double> x=x0;
  double d = 1e-6;
  for (unsigned i=0;i<x0.size();++i)
  {
    x=x0;
    x[i]+=d;
    double gi = (pdf.log_p(x)-p0)/d;
    TEST_NEAR("gradient_logp correct",gi,g[i],1e-6);
  }
}

void test_axis_gaussian()
{
  std::cout << "*****************************\n"
           << " Testing vpdfl_axis_gaussian\n"
           << "*****************************\n";

  int n_dims = 3;
  vnl_vector<double> mean0(n_dims),var0(n_dims),v0(n_dims);
  for (int i=0;i<n_dims;++i)
  {
    mean0(i)=i;
    var0(i) =n_dims-i;
  }

  v0.fill(0);

  vpdfl_axis_gaussian gauss0;
  gauss0.set(mean0,var0);

  test_gradient(gauss0,var0);
  test_gradient_logp(gauss0,var0);

  std::cout<<"Prob at zero: "<<gauss0(v0)<<std::endl;

  TEST_NEAR("gauss0(0)",gauss0(v0),0.00273207,1e-8);

  // Generate lots of samples
  int n = 5000;
  std::vector<vnl_vector<double> > samples(n);

  vpdfl_axis_gaussian_sampler sampler;
  sampler.set_model(gauss0);
  for (int i=0;i<n;++i)
    sampler.sample(samples[i]);

  mbl_data_array_wrapper<vnl_vector<double> > sample_wrapper(&samples[0],n);

  vpdfl_axis_gaussian_builder builder0;

  vpdfl_axis_gaussian gauss1;

  builder0.build(gauss1,sample_wrapper);

  std::cout<<"Original Model: "; vsl_print_summary(std::cout, gauss0); std::cout<<std::endl;
  std::cout<<"Rebuild  Model: "; vsl_print_summary(std::cout, gauss1); std::cout<<std::endl;

  TEST("mean of built model",(gauss0.mean()-gauss1.mean()).squared_magnitude()<0.1,true);
  TEST("var. of built model",(gauss0.variance()-gauss1.variance()).squared_magnitude()<0.1,true);

  // Test the IO
  vsl_add_to_binary_loader(vpdfl_axis_gaussian());
  vsl_add_to_binary_loader(vpdfl_axis_gaussian_builder());
  vpdfl_pdf_base            *base_pdf_ptr_out = &gauss0;
  vpdfl_builder_base *base_builder_ptr_out = &builder0;

  vsl_b_ofstream bfs_out("test_axis_gaussian.bvl.tmp");
  TEST("Created test_axis_gaussian.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, gauss0);
  vsl_b_write(bfs_out, builder0);
  vsl_b_write(bfs_out, base_pdf_ptr_out);
  vsl_b_write(bfs_out, base_builder_ptr_out);
  bfs_out.close();

  vpdfl_axis_gaussian_builder builder0_in;
  vpdfl_axis_gaussian gauss0_in;
  vpdfl_pdf_base            *base_pdf_ptr_in  = nullptr;
  vpdfl_builder_base *base_builder_ptr_in  = nullptr;

  vsl_b_ifstream bfs_in("test_axis_gaussian.bvl.tmp");
  TEST("Opened test_axis_gaussian.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, gauss0_in);
  vsl_b_read(bfs_in, builder0_in);
  vsl_b_read(bfs_in, base_pdf_ptr_in);
  vsl_b_read(bfs_in, base_builder_ptr_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("test_axis_gaussian.bvl.tmp");
#endif

  std::cout<<"Loaded:\n";
  std::cout<<"Model: ";                 vsl_print_summary(std::cout, gauss0_in); std::cout<<std::endl;
  std::cout<<"Builder: ";               vsl_print_summary(std::cout, builder0_in); std::cout<<std::endl;
  std::cout<<"Model   (by base ptr): "; vsl_print_summary(std::cout, base_pdf_ptr_in); std::cout<<std::endl;
  std::cout<<"Builder (by base ptr): "; vsl_print_summary(std::cout, base_builder_ptr_in); std::cout<<std::endl;

  TEST("mean of loaded model",
       (gauss0.mean()-gauss0_in.mean()).squared_magnitude()<1e-8,true);
  TEST("var. of loaded model",
       (gauss0.variance()-gauss0_in.variance()).squared_magnitude()<1e-8,true);
  TEST("Load model by base ptr",base_pdf_ptr_in->is_a()==gauss0.is_a(),true);
  TEST("Load builder by base ptr",base_builder_ptr_in->is_a()==builder0.is_a(),true);


  std::cout << "========Testing PDF Thresholds==========";
  vpdfl_sampler_base *p_sampler2 = gauss1.new_sampler();
  unsigned pass=0, fail=0;
  vnl_vector<double> x;
  double thresh = gauss1.log_prob_thresh(0.9);
  std::cout << std::endl << "log density threshold for passing 90%: " << thresh << std::endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (gauss1.log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  std::cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);
  pass=0; fail=0;
  thresh = gauss1.log_prob_thresh(0.1);
  std::cout << std::endl << std::endl << "log density threshold for passing 10%: " << thresh << std::endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (gauss1.log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  std::cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("70 < pass < 130", pass > 70 && pass < 130, true);

  vsl_delete_all_loaders();
  delete p_sampler2;
  delete base_pdf_ptr_in;
  delete base_builder_ptr_in;

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    vpdfl_add_all_binary_loaders();
    std::istringstream ss(
          "axis_gaussian\n"
          "{\n"
          "  min_var: 0.1234e-5\n"
          "}\n");

    std::unique_ptr<vpdfl_builder_base>
            builder = vpdfl_builder_base::new_pdf_builder_from_stream(ss);

    TEST("Correct builder",builder->is_a(),"vpdfl_axis_gaussian_builder");
    if (builder->is_a()=="vpdfl_axis_gaussian_builder")
    {
      auto &a_builder = static_cast<vpdfl_axis_gaussian_builder&>(*builder);
      std::cout<<a_builder<<std::endl;
      TEST_NEAR("Min var configured", a_builder.min_var(), 0.1234e-5, 1e-8);
    }
    vsl_delete_all_loaders();
  }
}

TESTMAIN(test_axis_gaussian);
