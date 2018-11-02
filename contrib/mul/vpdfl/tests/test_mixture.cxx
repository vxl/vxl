// This is mul/vpdfl/tests/test_mixture.cxx
//=======================================================================
//
//  Copyright: (C) 2000 Victoria University of Manchester
//
//=======================================================================
#include <iostream>
#include <sstream>
#include <string>
#include <testlib/testlib_test.h>
//:
// \file
// \author Ian Scott
// \brief test vpdfl_gaussian, building, sampling, saving etc.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // vpl_unlink()
#include <vpdfl/vpdfl_mixture.h>
#include <vpdfl/vpdfl_mixture_builder.h>
#include <vpdfl/vpdfl_axis_gaussian.h>
#include <vpdfl/vpdfl_axis_gaussian_builder.h>
#include <vpdfl/vpdfl_sampler_base.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vsl/vsl_binary_loader.h>
#include <vpdfl/vpdfl_add_all_binary_loaders.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

//=======================================================================

//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_mixture()
{
  std::cout << "***********************\n"
           << " Testing vpdfl_mixture\n"
           << "***********************\n";

  vsl_add_to_binary_loader(vpdfl_axis_gaussian());
  vsl_add_to_binary_loader(vpdfl_axis_gaussian_builder());
  vsl_add_to_binary_loader(vpdfl_mixture());
  vsl_add_to_binary_loader(vpdfl_mixture_builder());

  vpdfl_axis_gaussian_builder g_builder;
  vpdfl_axis_gaussian         a_gaussian;

  int n_comp = 2;

  // Initialise builder for 2 gaussian components
  vpdfl_mixture_builder builder;
  builder.init(g_builder,n_comp);
  builder.set_weights_fixed(false);

  // Initialise default pdf using 2 5D gaussians
  vpdfl_mixture pdf;
  pdf.init(a_gaussian, n_comp);

  int n = 5;
  std::vector<vnl_vector<double> > mean(n_comp), var(n_comp);

  for (int j=0;j<n_comp;++j)
  {
    mean[j].set_size(n);
    var[j].set_size(n);
    for (int i=0;i<n;++i)
    {
      mean[j](i) = i*j;
      var[j](i) = i;
    }

    auto& gauss_j = static_cast<vpdfl_axis_gaussian&>(*(pdf.components()[j]));
    gauss_j.set(mean[j],var[j]);
  }

  int n_samples = 1000;

  vpdfl_sampler_base* p_sampler = pdf.new_sampler();
  p_sampler->reseed(9667566ul);

    // Generate lots of samples
  std::vector<vnl_vector<double> > data(n_samples);
  for (int i=0;i<n_samples;++i)
    p_sampler->sample(data[i]);

  vpdfl_pdf_base* p_pdf = builder.new_model();

  mbl_data_array_wrapper<vnl_vector<double> > data_array(data);

  builder.build(*p_pdf,data_array);

  std::cout<<"Original PDF: "<<pdf<<std::endl
          <<"Mean: "<< pdf.mean() <<std::endl
          <<"Var:  "<<pdf.variance()<<std::endl
          <<"New PDF: "<<p_pdf<<std::endl
          <<"Mean: " << p_pdf->mean()<<std::endl
          <<"Var:  " << p_pdf->variance()<<std::endl;

  vpdfl_pdf_base* p_pdf_MI = builder.new_model();
  builder.preset_initial_means(mean);
  std::cout<<"Now building with preset initial means"<<std::endl;
  builder.build(*p_pdf_MI,data_array);

  std::cout<<"Original PDF: "<<pdf<<std::endl
          <<"Mean: "<< pdf.mean() <<std::endl
          <<"Var:  "<<pdf.variance()<<std::endl
          <<"New PDF: "<<p_pdf_MI<<std::endl
          <<"Mean: " << p_pdf_MI->mean()<<std::endl
          <<"Var:  " << p_pdf_MI->variance()<<std::endl;

  auto & gmm =  static_cast<vpdfl_mixture &>(*p_pdf);

  std::vector<double> test_wts(n_comp, 1.0/n_comp);
  TEST_NEAR("Weights are about correct",
            vnl_c_vector<double>:: euclid_dist_sq(&gmm.weights()[0], &test_wts[0], n_comp),
            0.0, 0.01);

  if (vnl_vector_ssd(gmm.component(0).mean(), mean[0]) <
      vnl_vector_ssd(gmm.component(0).mean(), mean[1]) )
  {
    TEST("Means are about correct",
         (vnl_vector_ssd(gmm.component(0).mean(), mean[0]) < 0.05 &&
          vnl_vector_ssd(gmm.component(1).mean(), mean[1]) < 0.05),
         true);
  }
  else
  {
    TEST("Means are about correct",
         (vnl_vector_ssd(gmm.component(0).mean(), mean[1]) < 0.05 &&
          vnl_vector_ssd(gmm.component(1).mean(), mean[0]) < 0.05),
         true);
  }

  auto & gmm_MI =  static_cast<vpdfl_mixture &>(*p_pdf);

  TEST_NEAR("Weights are about correct",
            vnl_c_vector<double>:: euclid_dist_sq(&gmm_MI.weights()[0], &test_wts[0], n_comp),
            0.0, 0.01);

  if (vnl_vector_ssd(gmm_MI.component(0).mean(), mean[0]) <
      vnl_vector_ssd(gmm_MI.component(0).mean(), mean[1]) )
  {
    TEST("Means are about correct",
         (vnl_vector_ssd(gmm_MI.component(0).mean(), mean[0]) < 0.05 &&
          vnl_vector_ssd(gmm_MI.component(1).mean(), mean[1]) < 0.05),
         true);
  }
  else
  {
    TEST("Means are about correct",
         (vnl_vector_ssd(gmm_MI.component(0).mean(), mean[1]) < 0.05 &&
          vnl_vector_ssd(gmm_MI.component(1).mean(), mean[0]) < 0.05),
         true);
  }

  std::string test_path = "test_mixture.bvl.tmp";

  std::cout<<"\n\n=================Testing I/O:\nSaving data...\n";
  vsl_b_ofstream bfs_out(test_path);
  TEST(("Created "+test_path+" for writing").c_str(), (!bfs_out), false);

  vsl_b_write(bfs_out,builder);
  vsl_b_write(bfs_out,static_cast<vpdfl_builder_base*>(&builder));
  vsl_b_write(bfs_out,p_pdf);
  vsl_b_write(bfs_out,(vpdfl_pdf_base*) nullptr);
  bfs_out.close();

  std::cout<<"Loading data...\n";
  vpdfl_mixture_builder builder2;
  vpdfl_builder_base*  p_builder2 = nullptr;
  vpdfl_pdf_base*         p_pdf2 = nullptr;
  vpdfl_pdf_base*         p_pdf3 = nullptr;

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

  std::cout<<"Original builder: "; vsl_print_summary(std::cout, builder); std::cout << std::endl
          <<"\nLoaded builder: "; vsl_print_summary(std::cout, p_builder2); std::cout << std::endl

          <<"\n\nOriginal PDF: "; vsl_print_summary(std::cout, p_pdf); std::cout << std::endl
          <<"\nMean: " << p_pdf->mean()
          <<"\nVar:  " << p_pdf->variance()
          <<"\nLoaded PDF: "; vsl_print_summary(std::cout, p_pdf2); std::cout << std::endl
          <<"\nMean: " << p_pdf2->mean()
          <<"\nVar:  " << p_pdf2->variance()
          <<"\n\n\n";

  std::cout << "=============Testing PDF==========";
  vpdfl_sampler_base *p_sampler2 = p_pdf->new_sampler();
  unsigned pass=0, fail=0;
  vnl_vector<double> x;
  double thresh = p_pdf->log_prob_thresh(0.9);
  std::cout << std::endl << "log density threshold for passing 90%: " << thresh << std::endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (p_pdf->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  std::cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("880 < pass < 920", pass > 880 && pass < 920, true);
  pass=0; fail=0;
  thresh = p_pdf->log_prob_thresh(0.1);
  std::cout << std::endl << std::endl << "log density threshold for passing 10%: " << thresh << std::endl;
  for (unsigned i=0; i < 1000; i++)
  {
    p_sampler2->sample(x);
    if (p_pdf->log_p(x) > thresh)
      pass ++;
    else
      fail ++;
  }
  std::cout << "In a sample of 1000 vectors " << pass << " passed and " << fail <<  " failed.\n";
  TEST("70 <= pass <= 130", pass >= 70 && pass <= 130, true);

  vsl_delete_all_loaders();
  delete p_pdf;
  delete p_sampler;
  delete p_sampler2;
  delete p_builder2;
  delete p_pdf2;
  delete p_pdf3;
  delete p_pdf_MI;

  // -------------------------------------------
  //  Test configuring from stream
  // -------------------------------------------
  {
    vpdfl_add_all_binary_loaders();
    std::istringstream ss(
          "mixture\n"
          "{\n"
          "  min_var: 0.1234e-5\n"
          "  n_pdfs: 3\n"
          "  basis_pdf: axis_gaussian { min_var: 0.0001 }\n"
          "  weights_fixed: true\n"
          "  max_its: 7\n"
          "}\n");

    std::unique_ptr<vpdfl_builder_base>
            builder = vpdfl_builder_base::new_pdf_builder_from_stream(ss);

    TEST("Correct builder",builder->is_a(),"vpdfl_mixture_builder");
    if (builder->is_a()=="vpdfl_mixture_builder")
    {
      auto &a_builder = static_cast<vpdfl_mixture_builder&>(*builder);
      std::cout<<a_builder<<std::endl;
      TEST_NEAR("Min var configured",
                a_builder.min_var(),0.1234e-5,1e-8);
      TEST("Max its configured",
           a_builder.max_iterations(),7);
      TEST("weights_fixed configured",
           a_builder.weights_fixed(),true);
      TEST("N.builders configured",
           a_builder.n_builders(),3);
      TEST("Builder type configured",
           a_builder.builder(0).is_a(),"vpdfl_axis_gaussian_builder");
    }
    vsl_delete_all_loaders();
  }
}

TESTMAIN(test_mixture);

