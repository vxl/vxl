//:
// \file
// \author Tim Cootes
// \brief test pdf1d_compare_to_pdf_ks

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cmath.h> // for vcl_fabs()

#include <pdf1d/pdf1d_compare_to_pdf_ks.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <vsl/vsl_binary_loader.h>
#include <vnl/vnl_test.h>
#include <pdf1d/pdf1d_prob_ks.h>

//=======================================================================


//: Generate lots of samples using pdf, build new pdf with builder and compare the two
void test_compare_to_pdf_ks()
{
  vcl_cout << "\n\n************************" << vcl_endl;
  vcl_cout << " Testing pdf1d_compare_to_pdf_ks " << vcl_endl;
  vcl_cout << "************************" << vcl_endl;


  pdf1d_compare_to_pdf_ks comparitor;
  pdf1d_gaussian gauss(0,1);
  pdf1d_sampler* p_sampler = gauss.new_sampler();
  p_sampler->reseed(147478);

  for (int i=1;i<=3;++i)
  {
    int n_samples = 30*i;


    // Generate lots of samples
    vnl_vector<double> data(n_samples);
    p_sampler->get_samples(data);

    double P = comparitor.compare(data.data_block(),n_samples,gauss);
    vcl_cout<<"Testing "<<n_samples<<" samples drawn from gaussian with unit gaussian"<<vcl_endl;
    vcl_cout<<"Prob = "<<P<<vcl_endl;

    double mean,var;
    pdf1d_calc_mean_var(mean,var,data);
    vcl_cout<<"Sample Mean: "<<mean<<" Variance: "<<var<<vcl_endl;
  }

  TEST("pdf1d_prob_ks(0)==1",vcl_fabs(pdf1d_prob_ks(0)-1.0)<1e-8, true);
  TEST("pdf1d_prob_ks(inf)==0",vcl_fabs(pdf1d_prob_ks(9e9))<1e-8, true);

  delete p_sampler;
}

TESTMAIN(test_compare_to_pdf_ks);
