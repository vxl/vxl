// This is mul/pdf1d/tools/plot_pdf_estimate.cxx

//:
// \file
// \brief Plot kernel estimate to sampled data
// Sample from one distribution, generate kernel estimate, generate text file for plotting.
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_exponential.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_epanech_kernel_pdf_builder.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_builder.h>

vnl_random mz_random;

//: Generate n samples from pdf, build a pdf and generate text file of resulting pdf
void plot_estimate(const vcl_string& true_pdf_file, const vcl_string& new_pdf_file,
                   const pdf1d_pdf& pdf,
                   const pdf1d_builder& builder,
                   int n_samples, double min_x, double max_x, int nx)
{
  vnl_vector<double> d(n_samples),b;

  // Generate n random samples from the pdf
  pdf.get_samples(d);

  vcl_cout<<"Range of values: ["<<d.min_value()<<','<<d.max_value()<<']'<<vcl_endl;

  pdf1d_pdf *new_pdf = builder.new_model();

  builder.build_from_array(*new_pdf,d.data_block(),n_samples);

  if (pdf.write_plot_file(true_pdf_file,min_x,max_x,nx))
    vcl_cout<<"Wrote original distribution points to "<<true_pdf_file<<vcl_endl;
  else
    vcl_cout<<"Failed to write distribution points to "<<true_pdf_file<<vcl_endl;

  if (new_pdf->write_plot_file(new_pdf_file,min_x,max_x,nx))
    vcl_cout<<"Wrote distribution points to "<<new_pdf_file<<vcl_endl;
  else
    vcl_cout<<"Failed to write distribution points to "<<new_pdf_file<<vcl_endl;

  delete new_pdf;
}

int main()
{
  pdf1d_exponential exp_pdf(1);
  pdf1d_gaussian gaussian(0,1);
  pdf1d_gaussian gaussian2(0,2);
  pdf1d_flat flat(0,1);
  pdf1d_gaussian_builder g_builder;
  pdf1d_gaussian_kernel_pdf_builder gk_builder;
  gk_builder.set_use_width_from_separation();
  pdf1d_epanech_kernel_pdf_builder ek_builder;
//  ek_builder.set_use_width_from_separation();
  ek_builder.set_use_adaptive();

  int n_samples = 100;

  plot_estimate("true_pdf.txt","kernel.txt",flat,ek_builder,n_samples,-3,3,200);

  return 0;
}
