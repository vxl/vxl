//:
// \file
// \brief Program to test different kernel approximations
// \author Tim Cootes
//
// We generate N samples from a known distribution
//  eg unit gaussian, flat or exponential.
// We then use a variety of kernel estimation techniques
// (eg fixed width, adaptive kernels etc)
// to generate a kernel pdf from the samples.
// We then estimate the Bhat. overlap with the true pdf.

#include <vcl_iostream.h>
#include <mbl/mbl_stats_1d.h>
#include <vnl/vnl_vector.h>
#include <vcl_cmath.h>
#include <pdf1d/pdf1d_compare_to_pdf_bhat.h>
#include <pdf1d/pdf1d_compare_to_pdf_ks.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_builder.h>

//: Compute how well data in x matches true pdf using n different comparitors
void test_comparison(vcl_vector<mbl_stats_1d>& B_stats,
                     const vnl_vector<double>& x,
                     const pdf1d_pdf& true_pdf,
                     vcl_vector<pdf1d_compare_to_pdf*> comparitor)
{
  int n = comparitor.size();
  B_stats.resize(n);

  for (int i=0;i<n;++i)
  {
    double B = comparitor[i]->compare(x.data_block(),x.size(),true_pdf);
    B_stats[i].obs(B);
  }
}

//: Compute how well data sampled from true_pdf matches true_pdf using n different comparitors
void test_comparison(vcl_vector<mbl_stats_1d>& B_stats,
                     int n_samples, int n_repeats,
                     const pdf1d_pdf& true_pdf,
                     vcl_vector<pdf1d_compare_to_pdf*> comparitor)
{
  vnl_vector<double> x(n_samples);
  pdf1d_sampler *sampler = true_pdf.new_sampler();

  for (int i=0;i<n_repeats;++i)
  {
    sampler->get_samples(x);
    test_comparison(B_stats,x,true_pdf,comparitor);
  }

  delete sampler;
}

void test_comparison(int n_samples, int n_trials,
                     const pdf1d_pdf& true_pdf,
                     vcl_vector<pdf1d_compare_to_pdf*> comparitor,
                     const vcl_vector<vcl_string>& name)
{
  vcl_vector<mbl_stats_1d> B_stats;

  test_comparison(B_stats,n_samples,n_trials,true_pdf,comparitor);

  vcl_cout<<"PDF: "<<true_pdf<<vcl_endl;
  vcl_cout<<"Sampling "<<n_samples;
  vcl_cout<<" values from pdf and computing overlap with kernel estimate."<<vcl_endl;
  vcl_cout<<"Averaging over "<<n_trials<<" trials."<<vcl_endl;
  for (int i=0;i<B_stats.size();++i)
  {
    vcl_cout<<name[i]<<" : "<<vcl_endl;
    vcl_cout<<"Mean: "<<B_stats[i].mean()<<" Std.Err: "<<B_stats[i].stdError()<<vcl_endl;
  }
}

int main()
{
  vcl_vector<pdf1d_compare_to_pdf*> comparitor;
  vcl_vector<vcl_string> name;

#if 0
  // Set up gaussian estimator
  pdf1d_compare_to_pdf_bhat comp1;
  comp1.set_builder(pdf1d_gaussian_builder());
  comparitor.push_back(&comp1);
  name.push_back("Single gaussian estimate");


  // Set up gaussian kernel estimator
  pdf1d_compare_to_pdf_bhat comp2;
  pdf1d_gaussian_kernel_pdf_builder k_builder2;
  k_builder2.set_use_fixed_width(0.1);
  comp2.set_builder(k_builder2);
  comparitor.push_back(&comp2);
  name.push_back("Gaussian kernel estimate, width=0.1");
#endif

  // Set up gaussian kernel estimator
  pdf1d_compare_to_pdf_bhat comp3;
  pdf1d_gaussian_kernel_pdf_builder k_builder3;
  k_builder3.set_use_equal_width();
  comp3.set_builder(k_builder3);
  comparitor.push_back(&comp3);
  name.push_back("Bhat. using Gaussian kernel estimate, width depends on n.samples");

  // Set up gaussian kernel estimator
  pdf1d_compare_to_pdf_bhat comp4;
  pdf1d_gaussian_kernel_pdf_builder k_builder4;
  k_builder4.set_use_width_from_separation();
  comp4.set_builder(k_builder4);
  comparitor.push_back(&comp4);
  name.push_back("Bhat. using Gaussian kernel estimate, width depends on local sample separation");

  // Set up adaptive gaussian kernel estimator
  pdf1d_compare_to_pdf_bhat comp5;
  pdf1d_gaussian_kernel_pdf_builder k_builder5;
  k_builder5.set_use_adaptive();
  comp5.set_builder(k_builder5);
  comparitor.push_back(&comp5);
  name.push_back("Bhat. using Adaptive Gaussian kernel estimate");

  // Try with KS statistic
  pdf1d_compare_to_pdf_ks comp_ks;
  comparitor.push_back(&comp_ks);
  name.push_back("KS Statistic");

  int n_samples = 100;
  int n_trials = 1000;

//  pdf1d_gaussian true_pdf(0,1);
//  pdf1d_flat true_pdf(0,1);
  pdf1d_exponential true_pdf(1.0);
  // Let true pdf be n gaussians at 0,1,2 with sd of 0.25
//  pdf1d_gaussian_kernel_pdf true_pdf(2,1,0.25);

  test_comparison(n_samples,n_trials,true_pdf,comparitor,name);

  return 0;
}
