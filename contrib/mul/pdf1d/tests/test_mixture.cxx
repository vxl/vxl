// This is mul/pdf1d/tests/test_mixture.cxx
//=======================================================================
//
//  Copyright: (C) 2000 Victoria University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test pdf1d_mixture, building, sampling, saving etc.

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cmath.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <pdf1d/pdf1d_mixture.h>
#include <pdf1d/pdf1d_mixture_builder.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vsl/vsl_binary_loader.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_mixture()
{
  vcl_cout << "***********************\n"
           << " Testing pdf1d_mixture\n"
           << "***********************\n";

  vsl_add_to_binary_loader(pdf1d_gaussian());
  vsl_add_to_binary_loader(pdf1d_gaussian_builder());
  vsl_add_to_binary_loader(pdf1d_mixture());
  vsl_add_to_binary_loader(pdf1d_mixture_builder());

  pdf1d_gaussian_builder g_builder;
  pdf1d_gaussian         a_gaussian;

  int n_comp = 2;

  // Initialise builder for 2 gaussian components
  pdf1d_mixture_builder builder;
  builder.init(g_builder,n_comp);

  // Initialise default pdf using 2 5D gaussians
  pdf1d_mixture pdf;
  pdf.init(a_gaussian, n_comp);

  //n_comp = 5;
  vcl_vector<double> mean(n_comp), var(n_comp);

  for (int j=0;j<n_comp;++j)
  {
    mean[j]=2*j;
    var[j] =0.125*(j+1);
    pdf1d_gaussian& gauss_j = static_cast<pdf1d_gaussian&>(*(pdf.components()[j]));
    gauss_j.set(mean[j],var[j]);
  }

  int n_samples = 1000;

  pdf1d_sampler* p_sampler = pdf.new_sampler();
  p_sampler->reseed(9667566ul);

    // Generate lots of samples
  vcl_vector<double> data(n_samples);
  for (int i=0;i<n_samples;++i)
    data[i]= p_sampler->sample();

  pdf1d_pdf* p_pdf = builder.new_model();

  mbl_data_array_wrapper<double> data_array(data);

  builder.build(*p_pdf,data_array);

  vcl_cout<<"Original PDF: "<<pdf<<vcl_endl
          <<"Mean: "<< pdf.mean() <<vcl_endl
          <<"Var:  "<<pdf.variance()<<vcl_endl
          <<"New PDF: "<<p_pdf<<vcl_endl
          <<"Mean: " << p_pdf->mean()<<vcl_endl
          <<"Var:  " << p_pdf->variance()<<vcl_endl;

  pdf1d_mixture & gmm =  static_cast<pdf1d_mixture &>(*p_pdf);

  vcl_vector<double> test_wts(n_comp, 1.0/n_comp);
  TEST_NEAR("Weights are about correct",
            vnl_c_vector<double>::euclid_dist_sq(&gmm.weights()[0], &test_wts[0], n_comp),
            0.0, 0.001);

  if (vcl_fabs(gmm.component(0).mean()-mean[0]) <
      vcl_fabs(gmm.component(0).mean()-mean[1]) )
  {
    TEST_NEAR("Means are about correct", gmm.component(0).mean(), mean[0], 0.05);
    TEST_NEAR("Means are about correct", gmm.component(1).mean(), mean[1], 0.05);
  }
  else
  {
    TEST_NEAR("Means are about correct", gmm.component(0).mean(), mean[1], 0.05);
    TEST_NEAR("Means are about correct", gmm.component(1).mean(), mean[0], 0.05);
  }

  vcl_string test_path = "test_mixture.bvl.tmp";

  vcl_cout<<"\n\n=================Testing I/O:\nSaving data...\n";
  vsl_b_ofstream bfs_out(test_path);
  TEST(("Created "+test_path+" for writing").c_str(), (!bfs_out), false);

  vsl_b_write(bfs_out,builder);
  vsl_b_write(bfs_out,static_cast<pdf1d_builder*>( &builder));
  vsl_b_write(bfs_out,p_pdf);
  vsl_b_write(bfs_out,static_cast<pdf1d_pdf*>(0));
  bfs_out.close();

  vcl_cout<<"Loading data...\n";
  pdf1d_mixture_builder builder2;
  pdf1d_builder*  p_builder2 = 0;
  pdf1d_pdf*         p_pdf2 = 0;
  pdf1d_pdf*         p_pdf3 = 0;

  vsl_b_ifstream bfs_in(test_path);
  TEST(("Opened "+test_path+" for reading").c_str(), (!bfs_in), false);
  vsl_b_read(bfs_in, builder2);
  vsl_b_read(bfs_in, p_builder2);
  vsl_b_read(bfs_in, p_pdf2);
  vsl_b_read(bfs_in, p_pdf3);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink(test_path.c_str());
#endif

  vcl_cout<<"Original builder: "; vsl_print_summary(vcl_cout, builder); vcl_cout<<vcl_endl
          <<"Loaded builder: "; vsl_print_summary(vcl_cout, p_builder2); vcl_cout<<vcl_endl
          <<vcl_endl

          <<"Original PDF: "; vsl_print_summary(vcl_cout, p_pdf); vcl_cout<<vcl_endl
          <<"Mean: " << p_pdf->mean()<<vcl_endl
          <<"Var:  " << p_pdf->variance()<<vcl_endl
          <<"Loaded PDF: "; vsl_print_summary(vcl_cout, p_pdf2); vcl_cout<<vcl_endl
          <<"Mean: " << p_pdf2->mean()<<vcl_endl
          <<"Var:  " << p_pdf2->variance()<<vcl_endl
          <<vcl_endl <<vcl_endl;


  vcl_cout << "=============Testing PDF==========";
  pdf1d_sampler *p_sampler2 = p_pdf->new_sampler();
  unsigned pass=0, fail=0;
  double thresh = p_pdf->log_prob_thresh(0.9);
  vcl_cout << vcl_endl << "log density threshold for passing 90%: " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (p_pdf->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("820 < pass < 980", pass > 820 && pass < 980, true);
  pass=0; fail=0;
  thresh = p_pdf->log_prob_thresh(0.1);
  vcl_cout << vcl_endl << vcl_endl << "log density threshold for passing 10%: " << thresh << vcl_endl;
  for (unsigned i=0; i < 1000; i++)
  {
    double x = p_sampler2->sample();
    if (p_pdf->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  vcl_cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("70 < pass < 130", pass > 70 && pass < 130, true);

  delete p_pdf;
  delete p_sampler;
  delete p_sampler2;
  delete p_builder2;
  delete p_pdf2;
  delete p_pdf3;

  vsl_delete_all_loaders();
}

//==================< end of file: test_mixture.cxx >====================
