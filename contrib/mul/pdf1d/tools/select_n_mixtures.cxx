//:
// \file
// \brief Program to explore selecting optimal number of mixture components
// \author Tim Cootes
//
// We generate N samples from a known mixture model.
// We then try to fit mixture models with different numbers
// of components to the data, and select the one that
// gives the largest mean overlap.

#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vcl_cmath.h>
#include <pdf1d/pdf1d_compare_to_pdf_bhat.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_mixture.h>
#include <pdf1d/pdf1d_mixture_builder.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_builder.h>

vcl_ofstream ofs;

//: Compute how well different forms of pdf match to data x.
//  pdf[i] is the model built by builder[i]
void test_form(vcl_vector<double>& B,
               const vnl_vector<double>& x,
               vcl_vector<pdf1d_builder*>& builder,
               pdf1d_compare_to_pdf_bhat& comparitor)
{
  // This is inefficient
  unsigned int n = builder.size();
  if (B.size()!=n) B.resize(n);
  vnl_vector<double> b;
  for (unsigned int i=0;i<n;++i)
  {
//    B[i] = comparitor.compare_form(x.data_block(),x.size(),*builder[i]);
    B[i] = comparitor.bootstrap_compare_form(b,x.data_block(),x.size(),*builder[i],10);
    vcl_cout<<i+1<<") B: "<<B[i]<<vcl_endl;
    ofs<<i+1<<" "<<B[i]<<vcl_endl;
    for (unsigned int j=0;j<b.size();++j)
      ofs<<" "<<j+1<<" "<<b[j]<<vcl_endl;
  }
  vcl_cout<<"------------------------"<<vcl_endl;
}

void select_form(vnl_vector<int>& histo,
                 int n_samples, int n_trials,
                 const pdf1d_pdf& true_pdf,
                 vcl_vector<pdf1d_builder*>& builder,
                 pdf1d_compare_to_pdf_bhat& comparitor)
{
  vnl_vector<double> x(n_samples);
  pdf1d_sampler *sampler = true_pdf.new_sampler();

  unsigned int n = builder.size();
  if (histo.size()!=n)
  {
    histo.resize(n);
    histo.fill(0);
  }

  vcl_vector<double> B(n);

  for (int i=0;i<n_trials;++i)
  {
    sampler->get_samples(x);
    test_form(B,x,builder,comparitor);

    int best_j=0;
    double best_B = B[0];
    for (unsigned int j=1;j<n;++j)
      if (B[j]>best_B)
      {
        best_B=B[j];
        best_j=j;
      }

    histo[best_j]+=1;
  }

  delete sampler;
}

void select_form(int n_samples, int n_trials, int max_comp,
                 const pdf1d_pdf& true_pdf,
                 pdf1d_compare_to_pdf_bhat& comparitor)
{
  vnl_vector<int> histo;
  pdf1d_gaussian_builder gauss_builder;

  vcl_vector<pdf1d_builder*> builder(max_comp);
  for (int i=0;i<max_comp;++i)
  {
    pdf1d_mixture_builder *b = new pdf1d_mixture_builder;
    b->init(gauss_builder,i+1);
    builder[i] = b;
  }

  select_form(histo,n_samples,n_trials,true_pdf,builder,comparitor);

  vcl_cout<<"PDF: "<<true_pdf<<vcl_endl;
  vcl_cout<<"Sampling "<<n_samples;
  vcl_cout<<" values from pdf and computing overlap with kernel estimate."<<vcl_endl;
  vcl_cout<<"Averaging over "<<n_trials<<" trials."<<vcl_endl;
  vcl_cout<<"Number of times each number of components preferred: "<<vcl_endl;
  for (int i=0;i<max_comp;++i)
  {
    vcl_cout<<i+1<<" components: "<<histo[i]<<vcl_endl;
  }

    // Tidy up
  for (int i=0;i<max_comp;++i)
    delete builder[i];
}

int main()
{
  // Set up gaussian kernel estimator
  pdf1d_compare_to_pdf_bhat comparitor;
  pdf1d_gaussian_kernel_pdf_builder k_builder;
//  k_builder.set_use_equal_width();
  k_builder.set_use_width_from_separation();
  comparitor.set_builder(k_builder);

  int n_samples = 100;
  int n_trials = 1;
  int max_comp = 15;
  int n_mix = 3;

  // Let true pdf be n gaussians at 0,1,2 with sd of 0.25
  pdf1d_gaussian_kernel_pdf true_pdf(n_mix,1,0.25);

  ofs.open("B_vs_N_mix.txt",vcl_ios::out);
  select_form(n_samples,n_trials,max_comp,true_pdf,comparitor);
  ofs.close();

  vcl_cout<<"See B_vs_N_mix.txt"<<vcl_endl;

  return 0;
}
