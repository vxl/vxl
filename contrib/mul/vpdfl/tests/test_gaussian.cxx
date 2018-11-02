// This is mul/vpdfl/tests/test_gaussian.cxx
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Ian Scott
// \brief test vpdfl_gaussian, building, sampling, saving etc.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()

#include <vsl/vsl_binary_loader.h>
#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_sampler_base.h>
#include <vpdfl/vpdfl_add_all_binary_loaders.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <mbl/mbl_test.h>
#include <vnl/io/vnl_io_matrix.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================
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


//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_gaussian()
{
  std::cout << "************************\n"
           << " Testing vpdfl_gaussian\n"
           << "************************\n";

  vsl_add_to_binary_loader(vpdfl_gaussian());
  vsl_add_to_binary_loader(vpdfl_gaussian_builder());

  vpdfl_gaussian_builder builder;

  vpdfl_gaussian pdf;

  int n = 4;
  vnl_vector<double> mean(n),evals(n);
  for (int i=0;i<n;++i)
  {
    mean(i) = i+1;
    evals(i) = 1;
  }

  // Initialise to identity
  vnl_matrix<double> evecs(n,n);
  evecs.set_identity();

  std::cout<<"Setting evecs: "; vsl_print_summary(std::cout, evecs); std::cout<<std::endl;

  pdf.set(mean,evecs,evals);


  // check probability density against independently calculated value
  vnl_vector<double> v0(n, 0.0);
  std::cout<<"Prob at zero: "<<pdf(v0)<<std::endl;

  TEST_NEAR("pdf(0)",pdf(v0),7.748596298e-9,1e-15);

  test_gradient(pdf,v0);
  test_gradient_logp(pdf,v0);


// Test builder =======================================
  int n_samples = 10000;

  vpdfl_sampler_base* p_sampler = pdf.new_sampler();

// Generate lots of samples
  std::vector<vnl_vector<double> > data(n_samples);
  for (int i=0;i<n_samples;++i)
    p_sampler->sample(data[i]);

  vpdfl_pdf_base* p_pdf_built = builder.new_model();

  mbl_data_array_wrapper<vnl_vector<double> > data_array(&data[0],n_samples);

  builder.build(*p_pdf_built,data_array);

  std::cout<<"Original PDF: "; vsl_print_summary(std::cout, pdf);
  std::cout<<"\nRebuilt PDF: "; vsl_print_summary(std::cout, p_pdf_built);
  std::cout<<"\n\nPDF sampler: "; vsl_print_summary(std::cout, p_sampler);
  std::cout<<'\n';

  auto& g_pdf1 = static_cast<vpdfl_gaussian&>(*p_pdf_built);
  vpdfl_gaussian g_pdf2;
  vnl_vector<double> var2(4);
  for (unsigned i=0;i<4;++i) var2[i]=1+i;
  g_pdf2.set(g_pdf1.mean(),g_pdf1.eigenvecs(),var2);
  test_gradient(g_pdf2,data[0]);
  test_gradient_logp(g_pdf2,data[0]);

// Test the IO ================================================

  vpdfl_builder_base* p_builder = & builder;
  vpdfl_pdf_base* p_pdf = & pdf;

  TEST("Mean of built model",vnl_vector_ssd(pdf.mean(), p_pdf_built->mean())<0.1,true);
  TEST("Variances",vnl_vector_ssd(pdf.variance(), p_pdf_built->variance())<0.1,true);
  TEST("Eigenvalues", vnl_vector_ssd(pdf.eigenvals(),  (static_cast<vpdfl_gaussian*>(p_pdf_built))->eigenvals())<0.1,true);

  std::cout<<"\n\n=================Testing I/O:\nSaving data...\n";
  vsl_b_ofstream bfs_out("test_gaussian.bvl.tmp");
  TEST("Created test_gaussian.bvl.tmp for writing", (!bfs_out), false);

  vsl_b_write(bfs_out,pdf);
  vsl_b_write(bfs_out,builder);
  vsl_b_write(bfs_out,p_pdf);
  vsl_b_write(bfs_out,p_builder);
  bfs_out.close();

  vpdfl_gaussian          pdf_in;
  vpdfl_gaussian_builder  builder_in;
  vpdfl_pdf_base*         p_base_pdf_in = nullptr;
  vpdfl_builder_base*     p_base_builder_in = nullptr;

  vsl_b_ifstream bfs_in("test_gaussian.bvl.tmp");
  TEST("Opened test_gaussian.bvl.tmp for reading", (!bfs_in), false);

  vsl_b_read(bfs_in, pdf_in);
  vsl_b_read(bfs_in, builder_in);
  vsl_b_read(bfs_in, p_base_pdf_in);
  vsl_b_read(bfs_in, p_base_builder_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("test_gaussian.bvl.tmp");
#endif

  std::cout<<"Original PDF: "; vsl_print_summary(std::cout, pdf);
  std::cout<<"\nOriginal builder: "; vsl_print_summary(std::cout, builder);
  std::cout<<"\n\nLoaded PDF: "; vsl_print_summary(std::cout, pdf_in);
  std::cout<<"\nLoaded builder: "; vsl_print_summary(std::cout, builder_in);
  std::cout<<"\n\n";

  TEST("Original Model == Loaded model",
       mbl_test_summaries_are_equal(pdf, pdf_in), true);
  TEST("Original Model == model loaded by base ptr",
       mbl_test_summaries_are_equal(p_pdf, p_base_pdf_in), true);
  TEST("Original Builder == Loaded builder",
       mbl_test_summaries_are_equal(builder, builder_in), true);
  TEST("Original Builder == Builder loaded by base ptr",
       mbl_test_summaries_are_equal(p_builder, p_base_builder_in), true);

  std::cout << "\n\n========Testing PDF Thresholds==========\n";
  vpdfl_sampler_base *p_sampler2 = p_pdf_built->new_sampler();
  unsigned pass=0, fail=0;
  vnl_vector<double> x;
  double thresh = p_pdf_built->log_prob_thresh(0.9);
  std::cout << "log density threshold for passing 90%: " << thresh << std::endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (p_pdf_built->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  std::cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);
  pass=0; fail=0;
  thresh = p_pdf_built->log_prob_thresh(0.1);
  std::cout << "\n\nlog density threshold for passing 10%: " << thresh << std::endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (p_pdf_built->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  std::cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("70 < pass < 130", pass > 70 && pass < 130, true);
  delete p_sampler2;


  std::cout << "\n\n========Testing PDF Plausible regions==========\n";
  vnl_vector<double> v2(n);
  vnl_vector<double> mean2(n),evals2(n);
  for (int i=0;i<n;++i)
  {
    v2(i)=n+1-i;
    mean2(i) = i*i+1;
    evals2(i) = i+1;
  }
  v2+=mean2;
  pdf.set(mean2,evecs,evals2);

  double pd = pdf.log_prob_thresh(0.9);
  TEST("v2 is outside Int(Prob)<0.9 region", pdf.log_p(v2) < pd, true);
  vnl_vector<double>v3(v2);

  pdf.nearest_plausible(v3,pd);
  std::cout << "Nearest plausible of v2(" << v2 << ") = v3("
           << v3 << ')' << std::endl;
  TEST_NEAR("v3 is on Int(Prob)=0.9 boundary", pdf.log_p(v3), pd, 1e-5);

  TEST_NEAR("v3 and v2 have identical directions from mean",
            angle(v3-mean2,v2-mean2), 0, 1e-5);

  delete p_pdf_built;
  delete p_sampler;
  delete p_base_pdf_in;
  delete p_base_builder_in;
  vsl_delete_all_loaders();

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    vpdfl_add_all_binary_loaders();
    std::istringstream ss(
          "gaussian\n"
          "{\n"
          "  min_var: 0.1234e-5\n"
          "}\n");

    std::unique_ptr<vpdfl_builder_base>
            builder = vpdfl_builder_base::new_pdf_builder_from_stream(ss);

    TEST("Correct builder",builder->is_a(),"vpdfl_gaussian_builder");
    if (builder->is_a()=="vpdfl_axis_gaussian_builder")
    {
      auto &a_builder = static_cast<vpdfl_gaussian_builder&>(*builder);
      std::cout<<a_builder<<std::endl;
      TEST_NEAR("Min var configured", a_builder.min_var(), 0.1234e-5, 1e-8);
    }
    vsl_delete_all_loaders();
  }
}

TESTMAIN( test_gaussian );
