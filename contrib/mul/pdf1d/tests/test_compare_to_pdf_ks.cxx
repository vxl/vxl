//:
// \file
// \author Tim Cootes
// \brief test pdf1d_compare_to_pdf_ks

#include <iostream>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pdf1d/pdf1d_compare_to_pdf_ks.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <testlib/testlib_test.h>
#include <pdf1d/pdf1d_prob_ks.h>

//=======================================================================


//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_compare_to_pdf_ks()
{
  std::cout << "*********************************\n"
           << " Testing pdf1d_compare_to_pdf_ks\n"
           << "*********************************\n";


  pdf1d_compare_to_pdf_ks comparator;
  pdf1d_gaussian gauss(0,1);
  pdf1d_sampler* p_sampler = gauss.new_sampler();
  p_sampler->reseed(147478);

  for (int i=1;i<=3;++i)
  {
    int n_samples = 30*i;


    // Generate lots of samples
    vnl_vector<double> data(n_samples);
    p_sampler->get_samples(data);

    double P = comparator.compare(data.data_block(),n_samples,gauss);
    std::cout<<"Testing "<<n_samples<<" samples drawn from Gaussian with unit Gaussian\n"
            <<"Prob = "<<P<<std::endl;

    double mean,var;
    pdf1d_calc_mean_var(mean,var,data);
    std::cout<<"Sample Mean: "<<mean<<" Variance: "<<var<<std::endl;
  }

  TEST_NEAR("pdf1d_prob_ks(0)",  pdf1d_prob_ks(0),   1.0, 1e-8);
  TEST_NEAR("pdf1d_prob_ks(inf)",pdf1d_prob_ks(9e9), 0.0, 1e-8);

  delete p_sampler;
}

TESTLIB_DEFINE_MAIN(test_compare_to_pdf_ks);
