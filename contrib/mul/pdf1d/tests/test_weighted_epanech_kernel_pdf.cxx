// This is mul/pdf1d/tests/test_weighted_epanech_kernel_pdf.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Ian Scott
// \brief test weighted_epanech_kernel_pdf, building, saving etc.

#include <pdf1d/pdf1d_weighted_epanech_kernel_pdf.h>
#include <pdf1d/pdf1d_epanech_kernel_pdf_builder.h>
#include <pdf1d/pdf1d_weighted_epanech_kernel_sampler.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_math.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_gaussian.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================


//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_weighted_epanech_kernel_pdf()
{
  vcl_cout << "*******************************************\n"
           << " Testing pdf1d_weighted_epanech_kernel_pdf\n"
           << "*******************************************\n";


  // Test Single kernel distribution
  {
    pdf1d_weighted_epanech_kernel_pdf p1(2,1.0,1.0);
    double d1 [] = {0.0, 1.0};
    p1.set_weight(vnl_vector<double>(2, 2, d1));
    pdf1d_weighted_epanech_kernel_sampler s1;
    s1.set_model(p1);
    const unsigned ns = 10000;
    vnl_vector<double> x(ns), p(ns), ks1(ns);
    s1.regular_samples(x);
    for (unsigned i=0; i<ns; ++i)
    {
      p(i) = p1.cdf(x(i));
      ks1(i) = ((double)i + 0.5)/ns;
    }
    vcl_sort(p.begin(), p.end());
    ks1 -= p;
    ks1.apply(&vnl_math_abs);

    vcl_cout << "Single kernel KS1 deviation is " << vnl_c_vector<double>::max_value(ks1.data_block(), ns) << vcl_endl;
    TEST("KS1 test < 0.003", vnl_c_vector<double>::max_value(ks1.data_block(), ns) < 0.003, true);
  }

  {
    pdf1d_weighted_epanech_kernel_pdf p1(4,1.0,1.0);
    double d1 [] = {0.01, 0.1, 0.1, 1.0};
    p1.set_weight(vnl_vector<double>(4, 4, d1));
    pdf1d_weighted_epanech_kernel_sampler s1;
    s1.set_model(p1);
    const unsigned ns = 10000;
    vnl_vector<double> x(ns), p(ns), ks1(ns);
    s1.get_samples(x);
    for (unsigned i=0; i<ns; ++i)
    {
      p(i) = p1.cdf(x(i));
      ks1(i) = ((double)i + 0.5)/ns;
    }
    vcl_sort(p.begin(), p.end());
    ks1 -= p;
    ks1.apply(&vnl_math_abs);

    vcl_cout << "4 weighted kernels kernel KS1 deviation is " << vnl_c_vector<double>::max_value(ks1.data_block(), ns) << vcl_endl;
    TEST("KS1 test < 0.01", vnl_c_vector<double>::max_value(ks1.data_block(), ns) < 0.01, true);
  }


  vsl_add_to_binary_loader(pdf1d_weighted_epanech_kernel_pdf());
  vsl_add_to_binary_loader(pdf1d_epanech_kernel_pdf_builder());

  pdf1d_epanech_kernel_pdf_builder builder;

  pdf1d_gaussian datagen(1,2);

// Test builder =======================================
  int n_samples = 10000;

  pdf1d_sampler* datagen_samp = datagen.new_sampler();

// Generate lots of samples
  vcl_vector<double> data(n_samples);
  for (int i=0;i<n_samples;++i)
    data[i] = datagen_samp->sample();

  delete datagen_samp;

  pdf1d_pdf* p_pdf_built = new pdf1d_weighted_epanech_kernel_pdf();

  mbl_data_array_wrapper<double> data_array(data);

  builder.build(*p_pdf_built,data_array);

  vcl_cout<<"Original PDF: "; vsl_print_summary(vcl_cout, datagen);
  vcl_cout<<"\nRebuilt PDF: "; vsl_print_summary(vcl_cout, p_pdf_built);
  vcl_cout<<"\n\n";

// Test the IO ================================================

  TEST_NEAR("Mean of built model", datagen.mean(), p_pdf_built->mean(), 0.1);
  TEST_NEAR("Variances", datagen.variance(), p_pdf_built->variance(), 0.1);

  vcl_cout<<"\n\n=================Testing I/O:\nSaving data...\n";
  vsl_b_ofstream bfs_out("test_gaussian_kernel_pdf.bvl.tmp");
  TEST("Created test_gaussian_kernel_pdf.bvl.tmp for writing", (!bfs_out), false);

  vsl_b_write(bfs_out,p_pdf_built);
  bfs_out.close();

  pdf1d_pdf*         p_pdf_in = NULL;
  pdf1d_builder*     p_builder_in = NULL;

  vsl_b_ifstream bfs_in("test_gaussian_kernel_pdf.bvl.tmp");
  TEST("Opened test_gaussian_kernel_pdf.bvl.tmp for reading", (!bfs_in), false);

  vsl_b_read(bfs_in, p_pdf_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("test_gaussian_kernel_pdf.bvl.tmp");
#endif

  vcl_cout<<"Original PDF: "; vsl_print_summary(vcl_cout, p_pdf_built);
  vcl_cout<<"\n\nLoaded PDF: "; vsl_print_summary(vcl_cout, p_pdf_in);
  vcl_cout<<"\n\n";

  TEST("Original Model == model loaded by base ptr",
       p_pdf_built->mean()==p_pdf_in->mean() &&
       p_pdf_built->variance()==p_pdf_in->variance() &&
       p_pdf_built->is_a()==p_pdf_in->is_a() &&
       p_pdf_built->is_class(p_pdf_in->is_a()),
       true);

  vcl_cout << "\n\n========Testing PDF Thresholds==========\n";
  pdf1d_sampler *p_sampler2 = p_pdf_built->new_sampler();
  unsigned pass, fail;
  double thresh;


  pass=0; fail=0;
  thresh = p_pdf_built->inverse_cdf(0.9);
  vcl_cout << "\nSample value threshold for passing 90%:                   " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (x < thresh) pass ++;
    else fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed using normal method.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);

  pass=0; fail=0;
  thresh = p_pdf_built->inverse_cdf(0.1);
  vcl_cout << "\nSample value threshold for passing 10%:                   " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (x < thresh) pass ++;
    else fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed using normal method.\n";
  TEST("80 < pass < 120", pass > 80 && pass < 120, true);

  delete p_sampler2;
  delete p_pdf_built;
  delete p_pdf_in;
  delete p_builder_in;

  vsl_delete_all_loaders();
}

TESTLIB_DEFINE_MAIN(test_gaussian_kernel_pdf);
