// This is mul/vpdfl/tests/test_gaussian.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Ian Scott
// \brief test vpdfl_gaussian, building, sampling, saving etc.

#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <vpdfl/vpdfl_gaussian_sampler.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vnl/io/vnl_io_matrix.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================

//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_gaussian()
{
  vcl_cout << "************************\n"
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

  vcl_cout<<"Setting evecs: "; vsl_print_summary(vcl_cout, evecs); vcl_cout<<vcl_endl;

  pdf.set(mean,evecs,evals);


  // check probability density against independently calculated value
  vnl_vector<double> v0(n, 0.0);
  vcl_cout<<"Prob at zero: "<<pdf(v0)<<vcl_endl;

  TEST_NEAR("pdf(0)",pdf(v0),7.748596298e-9,1e-15);

// Test builder =======================================
  int n_samples = 10000;

  vpdfl_sampler_base* p_sampler = pdf.new_sampler();

// Generate lots of samples
  vcl_vector<vnl_vector<double> > data(n_samples);
  for (int i=0;i<n_samples;++i)
    p_sampler->sample(data[i]);

  vpdfl_pdf_base* p_pdf_built = builder.new_model();

  mbl_data_array_wrapper<vnl_vector<double> > data_array(&data[0],n_samples);

  builder.build(*p_pdf_built,data_array);

  vcl_cout<<"Original PDF: "; vsl_print_summary(vcl_cout, pdf); vcl_cout<<vcl_endl;
  vcl_cout<<"Rebuilt PDF: "; vsl_print_summary(vcl_cout, p_pdf_built); vcl_cout<<vcl_endl;
  vcl_cout<<vcl_endl;
  vcl_cout<<"PDF sampler: "; vsl_print_summary(vcl_cout, p_sampler); vcl_cout<<vcl_endl;

// Test the IO ================================================

  vpdfl_builder_base* p_builder = & builder;
  vpdfl_pdf_base* p_pdf = & pdf;

  TEST("Mean of built model",vnl_vector_ssd(pdf.mean(), p_pdf_built->mean())<0.1,true);
  TEST("Variances",vnl_vector_ssd(pdf.variance(), p_pdf_built->variance())<0.1,true);
  TEST("Eigenvalues", vnl_vector_ssd(pdf.eigenvals(),  ((vpdfl_gaussian*) p_pdf_built)->eigenvals())<0.1,true);

  vcl_cout<<"\n\n=================Testing I/O:\nSaving data...\n";
  vsl_b_ofstream bfs_out("test_gaussian.bvl.tmp");
  TEST("Created test_gaussian.bvl.tmp for writing", (!bfs_out), false);

  vsl_b_write(bfs_out,pdf);
  vsl_b_write(bfs_out,builder);
  vsl_b_write(bfs_out,p_pdf);
  vsl_b_write(bfs_out,p_builder);
  bfs_out.close();

  vpdfl_gaussian          pdf_in;
  vpdfl_gaussian_builder  builder_in;
  vpdfl_pdf_base*         p_base_pdf_in = NULL;
  vpdfl_builder_base*     p_base_builder_in = NULL;

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

  vcl_cout<<"Original PDF: "; vsl_print_summary(vcl_cout, pdf); vcl_cout<<vcl_endl;
  vcl_cout<<"Original builder: "; vsl_print_summary(vcl_cout, builder); vcl_cout<<vcl_endl;
  vcl_cout<<vcl_endl;

  vcl_cout<<"Loaded PDF: "; vsl_print_summary(vcl_cout, pdf_in); vcl_cout<<vcl_endl;
  vcl_cout<<"Loaded builder: "; vsl_print_summary(vcl_cout, builder_in); vcl_cout<<vcl_endl;
  vcl_cout<<vcl_endl;

  TEST("Original Model == Loaded model",
       pdf.mean()==pdf_in.mean() &&
       pdf.variance()==pdf_in.variance() &&
       pdf.eigenvals() == pdf_in.eigenvals() &&
       pdf.eigenvecs() == pdf_in.eigenvecs() &&
       pdf.log_k() == pdf_in.log_k(),
       true);
  TEST("Original Model == model loaded by base ptr",
       pdf.mean()==p_base_pdf_in->mean() &&
       pdf.variance()==p_base_pdf_in->variance() &&
       pdf.is_a()==p_base_pdf_in->is_a(),
       true);
  TEST("Original Builder == Loaded builder",
       builder.min_var()==builder_in.min_var(),
       true);
  TEST("Original Builder == Builder loaded by base ptr",
       builder.min_var()==p_base_builder_in->min_var() &&
       builder.is_a()==p_base_builder_in->is_a(),
       true);

  vcl_cout << "========Testing PDF Thresholds==========";
  vpdfl_sampler_base *p_sampler2 = p_pdf_built->new_sampler();
  unsigned pass=0, fail=0;
  vnl_vector<double> x;
  double thresh = p_pdf_built->log_prob_thresh(0.9);
  vcl_cout << vcl_endl << "log density threshold for passing 90%: " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (p_pdf_built->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);
  pass=0; fail=0;
  thresh = p_pdf_built->log_prob_thresh(0.1);
  vcl_cout << vcl_endl << vcl_endl << "log density threshold for passing 10%: " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (p_pdf_built->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("70 < pass < 130", pass > 70 && pass < 130, true);
  delete p_sampler2;


  delete p_pdf_built;
  delete p_sampler;
  delete p_base_pdf_in;
  delete p_base_builder_in;
}

TESTLIB_DEFINE_MAIN(test_gaussian);
