// This is mul/pdf1d/tools/select_pdf.cxx

//:
// \file
// \author Tim Cootes
// \brief Test how often a particular comparator chooses correct pdf
// Generate samples from one pdf.  Use comparator to decide which
// of several pdfs samples belong to.  Generate graph of
// %correct vs N.samples

#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <pdf1d/pdf1d_compare_to_pdf_ks.h>
#include <pdf1d/pdf1d_compare_to_pdf_bhat.h>
#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_exponential_builder.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_builder.h>
#include <pdf1d/pdf1d_select_pdf.h>


//: Generate samples from pdf[0].  See how often pdf[0] is selected.
double pdf1d_test_pdf_selection(vcl_vector<const pdf1d_pdf*>& pdf,
                                int n_samples,
                                pdf1d_compare_to_pdf& comparator, int n_tests)
{
  int n_correct = 0;
  pdf1d_sampler *sampler = pdf[0]->new_sampler();

  vnl_vector<double> x(n_samples);
  for (int i=0;i<n_tests;++i)
  {
    sampler->get_samples(x);
    // Note: Using bootstrap gives more reliable results with more samples, but is much slower
    if (pdf1d_select_pdf(x.data_block(),n_samples,pdf,comparator)==0)
      n_correct++;
  }

  delete sampler;

  return double(n_correct)/n_tests;
}

//: Generate samples from pdf[0].  See how often pdf[0] is selected.
double pdf1d_test_bhat_pdf_selection(vcl_vector<const pdf1d_pdf*>& pdf,
                          int n_samples,
                          vcl_vector<pdf1d_builder*>& pdf_builder, int n_tests)
{
  pdf1d_compare_to_pdf_bhat comparator;
  int n_correct = 0;
  pdf1d_sampler *sampler = pdf[0]->new_sampler();

  vnl_vector<double> x(n_samples);
  for (int i=0;i<n_tests;++i)
  {
    sampler->get_samples(x);
    // Note: Using bootstrap gives more reliable results with more samples, but is much slower
    if (pdf1d_select_pdf(x.data_block(),n_samples,pdf,pdf_builder,comparator)==0)
      n_correct++;
  }

  delete sampler;

  return double(n_correct)/n_tests;
}

void graph_selection_results(const vcl_string& path,
                             vcl_vector<const pdf1d_pdf*>& pdf,
                             pdf1d_compare_to_pdf& comparator, int n_tests)
{
  vcl_ofstream ofs(path.c_str(),vcl_ios::out);
  for (int n_samples=10;n_samples<=200;n_samples+=10)
  {
    double prop_correct = pdf1d_test_pdf_selection(pdf,n_samples,comparator,n_tests);
    ofs<<n_samples<<' '<<prop_correct<<vcl_endl;
  }

  ofs.close();

  vcl_cout<<"Results saved to "<<path<<vcl_endl;
}

void graph_bhat_selection_results(const vcl_string& path,
                             vcl_vector<const pdf1d_pdf*>& pdf,
                             vcl_vector<pdf1d_builder*>& pdf_builder,  int n_tests)
{
  vcl_ofstream ofs(path.c_str(),vcl_ios::out);
  for (int n_samples=10;n_samples<=200;n_samples+=10)
  {
    double prop_correct = pdf1d_test_bhat_pdf_selection(pdf,n_samples,pdf_builder,n_tests);
    ofs<<n_samples<<' '<<prop_correct<<vcl_endl;
  }

  ofs.close();

  vcl_cout<<"Results saved to "<<path<<vcl_endl;
}

void test_flat_vs_gauss()
{
  pdf1d_flat flat(-1,1);
  pdf1d_gaussian Gaussian(flat.mean(),flat.variance());

  vcl_vector<const pdf1d_pdf*> pdf;
  pdf.push_back(&flat);
  pdf.push_back(&Gaussian);

  pdf1d_compare_to_pdf_bhat bhat_comparator_NN;
  graph_selection_results("N_OK_flat_v_gauss_BNN.txt",
                          pdf,bhat_comparator_NN,100);

  pdf1d_compare_to_pdf_bhat bhat_comparator_fixed;
  bhat_comparator_fixed.set_builder(pdf1d_gaussian_kernel_pdf_builder());
  graph_selection_results("N_OK_flat_v_gauss_Bfixed.txt",
                          pdf,bhat_comparator_fixed,100);

  pdf1d_compare_to_pdf_ks ks_comparator;
  graph_selection_results("N_OK_flat_v_gauss_KS.txt",
                          pdf,ks_comparator,100);
}

// Test Gaussians with different variances
void test_g1_v_g2()
{
  pdf1d_gaussian g1(0,1);
  pdf1d_gaussian g2(0,2);

  vcl_vector<const pdf1d_pdf*> pdf;
  pdf.push_back(&g1);
  pdf.push_back(&g2);

  pdf1d_compare_to_pdf_bhat bhat_comparator_gauss;
  bhat_comparator_gauss.set_builder(pdf1d_gaussian_builder());
  graph_selection_results("N_OK_g1_v_g2_Bgauss.txt",
                          pdf,bhat_comparator_gauss,100);

  pdf1d_compare_to_pdf_bhat bhat_comparator_NN;
  graph_selection_results("N_OK_g1_v_g2_BNN.txt",
                          pdf,bhat_comparator_NN,100);

  pdf1d_compare_to_pdf_bhat bhat_comparator_fixed;
  bhat_comparator_fixed.set_builder(pdf1d_gaussian_kernel_pdf_builder());
  graph_selection_results("N_OK_g1_v_g2_Bfixed.txt",
                          pdf,bhat_comparator_fixed,100);

  pdf1d_compare_to_pdf_ks ks_comparator;
  graph_selection_results("N_OK_g1_v_g2_KS.txt",
                          pdf,ks_comparator,100);
}

// Test Gaussians with different variances
void test_g2_v_g3()
{
  pdf1d_gaussian g1(0,2);
  pdf1d_gaussian g2(0,3);

  vcl_vector<const pdf1d_pdf*> pdf;
  pdf.push_back(&g1);
  pdf.push_back(&g2);

  pdf1d_compare_to_pdf_bhat bhat_comparator_gauss;
  bhat_comparator_gauss.set_builder(pdf1d_gaussian_builder());
  graph_selection_results("N_OK_g2_v_g3_Bgauss.txt",
                          pdf,bhat_comparator_gauss,100);

  pdf1d_compare_to_pdf_bhat bhat_comparator_NN;
  graph_selection_results("N_OK_g2_v_g3_BNN.txt",
                          pdf,bhat_comparator_NN,100);

//   pdf1d_epanech_kernel_pdf_builder ek_builder;
//   ek_builder.set_use_width_from_separation();
//   pdf1d_compare_to_pdf_bhat bhat_comparator_eNN;
//   bhat_comparator_eNN.set_builder(ek_builder);
//   graph_selection_results("N_OK_g2_v_g3_BeNN.txt",
//                           pdf,bhat_comparator_eNN,100);

  pdf1d_compare_to_pdf_bhat bhat_comparator_fixed;
  bhat_comparator_fixed.set_builder(pdf1d_gaussian_kernel_pdf_builder());
  graph_selection_results("N_OK_g2_v_g3_Bfixed.txt",
                          pdf,bhat_comparator_fixed,100);

  pdf1d_compare_to_pdf_ks ks_comparator;
  graph_selection_results("N_OK_g2_v_g3_KS.txt",
                          pdf,ks_comparator,100);
}

// Test exponential vs Gaussian
void test_e_v_g()
{
  pdf1d_exponential e;
  pdf1d_gaussian g(e.mean(),e.variance());

  vcl_vector<const pdf1d_pdf*> pdf;
  pdf.push_back(&e);
  pdf.push_back(&g);

  pdf1d_exponential_builder e_builder;
  pdf1d_gaussian_builder g_builder;
  vcl_vector<pdf1d_builder*> pdf_builder;
  pdf_builder.push_back(&e_builder);
  pdf_builder.push_back(&g_builder);

  graph_bhat_selection_results("N_OK_e_v_g_Beg.txt",
                          pdf,pdf_builder,100);


  pdf1d_compare_to_pdf_bhat bhat_comparator_NN;
  graph_selection_results("N_OK_e_v_g_BNN.txt",
                          pdf,bhat_comparator_NN,100);

  pdf1d_compare_to_pdf_bhat bhat_comparator_fixed;
  bhat_comparator_fixed.set_builder(pdf1d_gaussian_kernel_pdf_builder());
  graph_selection_results("N_OK_e_v_g_Bfixed.txt",
                          pdf,bhat_comparator_fixed,100);

  pdf1d_compare_to_pdf_ks ks_comparator;
  graph_selection_results("N_OK_e_v_g_KS.txt",
                          pdf,ks_comparator,100);
}

int main()
{
//  test_flat_vs_gauss();
  test_g1_v_g2();
//  test_e_v_g();
//  test_g2_v_g3();

  return 0;
}
