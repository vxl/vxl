//:
// \file
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mbl/mbl_stats_1d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_compare_to_pdf_ks.h>
#include <pdf1d/pdf1d_compare_to_pdf_bhat.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_epanech_kernel_pdf_builder.h>

// Generate samples with one pdf and test if it matches a particular form
// Output sent to B_vs_Nsamples.txt file.

vnl_random mz_random;

//: Generate n samples from pdf and compare with test_pdf
void run_experiment(const pdf1d_pdf& pdf,
                    const pdf1d_builder& test_builder,
                    double& B_mean, double& B_var, int n)
{
  vnl_vector<double> d(n),b;

  // Generate n random samples from the pdf
  pdf.get_samples(d);

  int n_trials = 20;

  // Strictly should use kernel builder, when one is available
  pdf1d_compare_to_pdf_bhat bhat_comparator;
  //  bhat_comparator.set_builder(pdf1d_gaussian_builder());
  pdf1d_epanech_kernel_pdf_builder ek_builder;
  ek_builder.set_use_width_from_separation();
  bhat_comparator.set_builder(ek_builder);

  pdf1d_compare_to_pdf_ks ks_comparator;

  bhat_comparator.bootstrap_compare_form(b,d.data_block(),n,test_builder,n_trials);

  pdf1d_calc_mean_var(B_mean,B_var,b);
#if 0
  vcl_cout<<B_mean<<" sd:"<<vcl_sqrt(B_var)<<vcl_endl;

  // Test if distribution of B is gaussian or log gaussian:
  vcl_cout<<"Compare B pdf to gaussian: "<<pdf1d_compare_to_gaussian_1d(b,n_reps)<<vcl_endl;

  vnl_vector<double> log_b = b;
  for (int i=0;i<b.size();++i) log_b[i]=vcl_log(b[i]);
  vcl_cout<<"Compare log(B) pdf to gaussian: "<<pdf1d_compare_to_gaussian_1d(log_b,n_reps)<<vcl_endl;

  vnl_vector<double> exp_b = b;
  for (int i=0;i<b.size();++i) exp_b[i]=vcl_exp(b[i]);
  vcl_cout<<"Compare exp(B) pdf to gaussian: "<<pdf1d_compare_to_gaussian_1d(exp_b,n_reps)<<vcl_endl;
#endif
}

#if 0
void run_experiment(const pdf1d_pdf& pdf,int n_samples, int n_repeats)
{
  mbl_stats_1d B_mean_stats,B_var_stats;
  for (int i=0;i<n_repeats;++i)
  {
    double B_mean,B_var;
    run_experiment(pdf,B_mean,B_var,n_samples);
    B_mean_stats.obs(B_mean);
    B_var_stats.obs(B_var);
  }

  vcl_cout<<"Overall statistics of B(stochastic):"<<vcl_endl;
  B_mean_stats.print_summary(vcl_cout);
  vcl_cout<<"\nAverage SD: "<<vcl_sqrt(B_var_stats.mean())<<vcl_endl;
}
#endif

void run_multi_experiments(const pdf1d_pdf& pdf,
                           const pdf1d_builder& test_builder,
                           double& B_mean, double& B_var,
                           int n_samples, int n_repeats)
{
  mbl_stats_1d B_mean_stats,B_var_stats;
  for (int i=0;i<n_repeats;++i)
  {
    double m,v;
    run_experiment(pdf,test_builder,m,v,n_samples);
    B_mean_stats.obs(m);
    B_var_stats.obs(v);
  }

  B_mean = B_mean_stats.mean();
  B_var  = B_var_stats.mean();
}


// Generate graph of Mean/SD B for increasing numbers of samples
void graph_results(const pdf1d_pdf& pdf,
                   const pdf1d_builder& test_builder,
                   const vcl_string& path)
{
  vcl_ofstream ofs(path.c_str(),vcl_ios::out);
  double B_mean,B_var;

  int n_repeats = 50;

  for (int i=1;i<20;++i)
  {
    int ns = i*10;
    run_multi_experiments(pdf,test_builder,B_mean,B_var,ns,n_repeats);
    ofs<<ns<<' '<<B_mean<<' '<<vcl_sqrt(B_var)<<vcl_endl;
  }

  ofs.close();

  vcl_cout<<"Results saved to "<<path<<vcl_endl;
}

int main()
{
  pdf1d_gaussian gaussian(0,1);
  pdf1d_gaussian gaussian2(0,2);
  pdf1d_flat flat(0,1);
  pdf1d_gaussian_builder g_builder;

  // Generate samples with one pdf and test if it matches form defined by the builder
  graph_results(flat,g_builder,"B_vs_Nsamples.txt");
#if 0
  vcl_cout<<"Testing distribution of Bhat. overlaps."<<vcl_endl;

  int n_samples = 10;
  int n_repeats = 20;
  run_experiment(n_samples,n_repeats);
#endif
  return 0;
}
