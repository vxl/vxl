// This is mul/pdf1d/tests/test_gaussian_kernel_pdf.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Ian Scott
// \brief test pdf1d_gaussian_kernel_pdf, building, sampling, saving etc.

#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_builder.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_sampler.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_gaussian_sampler.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================

// Fiddle things to force use of sampling version of pdf1d_pdf::inverse_cdf()
class pdf1d_test_gaussian_kernel_pdf_test_sample_inverse_cdf : public pdf1d_gaussian_kernel_pdf
{
  virtual bool cdf_is_analytic() const {return false;}
};


//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_gaussian_kernel_pdf()
{
  vcl_cout << "***********************************\n"
           << " Testing pdf1d_gaussian_kernel_pdf\n"
           << "***********************************\n";

  vsl_add_to_binary_loader(pdf1d_gaussian_kernel_pdf());
  vsl_add_to_binary_loader(pdf1d_gaussian_kernel_pdf_builder());

  pdf1d_gaussian_kernel_pdf_builder builder;

  pdf1d_gaussian datagen(1,2);

// Test builder =======================================
  int n_samples = 10000;

  pdf1d_sampler* datagen_samp = datagen.new_sampler();

// Generate lots of samples
  vcl_vector<double> data(n_samples);
  for (int i=0;i<n_samples;++i)
    data[i] = datagen_samp->sample();

  delete datagen_samp;

  pdf1d_pdf* p_pdf_built = builder.new_model();

  mbl_data_array_wrapper<double> data_array(data);

  builder.build(*p_pdf_built,data_array);

  pdf1d_sampler *p_sampler = p_pdf_built->new_sampler();

  vcl_cout<<"Original PDF: "; vsl_print_summary(vcl_cout, datagen);
  vcl_cout<<"\nRebuilt PDF: "; vsl_print_summary(vcl_cout, p_pdf_built);
  vcl_cout<<"\n\nPDF sampler: "; vsl_print_summary(vcl_cout, p_sampler);
  vcl_cout<<'\n';

// Test the IO ================================================

  pdf1d_builder* p_builder = & builder;

  TEST_NEAR("Mean of built model",datagen.mean(),p_pdf_built->mean(),0.1);
  TEST_NEAR("Variances",datagen.variance(),p_pdf_built->variance(),0.1);

  vcl_cout<<"\n\n=================Testing I/O:\nSaving data...\n";
  vsl_b_ofstream bfs_out("test_gaussian_kernel_pdf.bvl.tmp");
  TEST("Created test_gaussian_kernel_pdf.bvl.tmp for writing", (!bfs_out), false);

  vsl_b_write(bfs_out,builder);
  vsl_b_write(bfs_out,p_pdf_built);
  vsl_b_write(bfs_out,p_builder);
  bfs_out.close();

  pdf1d_gaussian_kernel_pdf_builder  builder_in;
  pdf1d_pdf*         p_pdf_in = NULL;
  pdf1d_builder*     p_builder_in = NULL;

  vsl_b_ifstream bfs_in("test_gaussian_kernel_pdf.bvl.tmp");
  TEST("Opened test_gaussian_kernel_pdf.bvl.tmp for reading", (!bfs_in), false);

  vsl_b_read(bfs_in, builder_in);
  vsl_b_read(bfs_in, p_pdf_in);
  vsl_b_read(bfs_in, p_builder_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("test_gaussian_kernel_pdf.bvl.tmp");
#endif

  vcl_cout<<"Original PDF: "; vsl_print_summary(vcl_cout, p_pdf_built);
  vcl_cout<<"\nOriginal builder: "; vsl_print_summary(vcl_cout, builder);
  vcl_cout<<"\n\nLoaded PDF: "; vsl_print_summary(vcl_cout, p_pdf_in);
  vcl_cout<<"\nLoaded builder: "; vsl_print_summary(vcl_cout, builder_in);
  vcl_cout<<"\n\n";

  TEST("Original Model == model loaded by base ptr",
       p_pdf_built->mean()==p_pdf_in->mean() &&
       p_pdf_built->variance()==p_pdf_in->variance() &&
       p_pdf_built->is_a()==p_pdf_in->is_a() &&
       p_pdf_built->is_class(p_pdf_in->is_a()),
       true);
  TEST("Original Builder == Loaded builder",
       builder.min_var()==builder_in.min_var(),
       true);
  TEST("Original Builder == Builder loaded by base ptr",
       builder.min_var()==p_builder_in->min_var() &&
       builder.is_a()==p_builder_in->is_a() &&
       builder.is_class(p_builder_in->is_a()),
       true);

  vcl_cout << "\n\n========Testing PDF Thresholds==========\n";
  pdf1d_sampler *p_sampler2 = p_pdf_built->new_sampler();
  unsigned pass, fail, sample_pass, sample_fail;
  double thresh, sample_thresh;

  pdf1d_test_gaussian_kernel_pdf_test_sample_inverse_cdf test_sample_inverse_cdf;


  pdf1d_gaussian_kernel_pdf & kde = (*static_cast<pdf1d_gaussian_kernel_pdf*>(p_pdf_built));
  test_sample_inverse_cdf.set_centres(kde.centre(), kde.width());

  pass=0; fail=0; sample_pass=0; sample_fail=0;
  thresh = p_pdf_built->inverse_cdf(0.9);
  vcl_cout << "\nSample value threshold for passing 90%:                   " << thresh;
  sample_thresh  = test_sample_inverse_cdf.pdf1d_pdf::inverse_cdf(0.9);
  vcl_cout << "\nThreshold calculated using sample method for passing 90%: " << sample_thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (x < thresh) pass ++;
    else fail ++;
    if (x < sample_thresh) sample_pass ++;
    else sample_fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and "
           << fail <<  " failed using normal method.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);
  vcl_cout << "In a sample of 1000 vectors " << sample_pass << " passed and "
           << sample_fail <<  " failed using sample method.\n";
  TEST("860 < pass < 930", sample_pass > 880 && sample_pass < 920, true);


  pass=0; fail=0; sample_pass=0; sample_fail=0;
  thresh = p_pdf_built->inverse_cdf(0.1);
  vcl_cout << "\nSample value threshold for passing 10%:                   " << thresh;
  sample_thresh  = test_sample_inverse_cdf.pdf1d_pdf::inverse_cdf(0.1);
  vcl_cout << "\nThreshold calculated using sample method for passing 10%: " << sample_thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (x < thresh) pass ++;
    else fail ++;
    if (x < sample_thresh) sample_pass ++;
    else sample_fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and "
           << fail <<  " failed using normal method.\n";
  TEST("80 < pass < 120", pass > 80 && pass < 120, true);
  vcl_cout << "In a sample of 1000 vectors " << sample_pass << " passed and "
           << sample_fail <<  " failed using sample method.\n";
  TEST("80 < pass < 120", sample_pass > 60 && sample_pass < 140, true);


  pass=0, fail=0;
  thresh = p_pdf_built->log_prob_thresh(0.9);
  vcl_cout << "\nlog density threshold for passing 90%: " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (p_pdf_built->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);

  pass=0; fail=0;
  thresh = p_pdf_built->log_prob_thresh(0.1);
  vcl_cout <<  "\nlog density threshold for passing 10%: " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
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
  delete p_pdf_in;
  delete p_builder_in;

  vsl_delete_all_loaders();
}

TESTLIB_DEFINE_MAIN(test_gaussian_kernel_pdf);
