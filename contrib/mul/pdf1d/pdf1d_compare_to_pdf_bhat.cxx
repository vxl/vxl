// This is mul/pdf1d/pdf1d_compare_to_pdf_bhat.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes
// \brief Test if data from a given distribution using Bhattacharyya overlap

#include "pdf1d_compare_to_pdf_bhat.h"

#include <vcl_string.h>

#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_gaussian_kernel_pdf_builder.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <pdf1d/pdf1d_bhat_overlap.h>
#include <pdf1d/pdf1d_sampler.h>
#include <pdf1d/pdf1d_resample.h>

//:
//  When true, if model_pdf is gaussian, use integration over the gaussian ([-3,3]sd)
//  to estimate overlap,  rather than resampling from data distribution
bool use_integration_for_gaussian=true;

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_compare_to_pdf_bhat::pdf1d_compare_to_pdf_bhat()
  : n_per_point_(3)
{
  pdf1d_gaussian_kernel_pdf_builder *gk_builder = new pdf1d_gaussian_kernel_pdf_builder;
  gk_builder->set_use_width_from_separation();
  builder_ = gk_builder;
}

//: Construct and define method of building pdf from data
pdf1d_compare_to_pdf_bhat::pdf1d_compare_to_pdf_bhat(const pdf1d_builder& builder,
                                                     int n_per_point)
  : n_per_point_(n_per_point)
{
  builder_ = builder;
}


//=======================================================================
// Destructor
//=======================================================================

pdf1d_compare_to_pdf_bhat::~pdf1d_compare_to_pdf_bhat()
{
}

//: Define method of building pdf from data
void pdf1d_compare_to_pdf_bhat::set_builder(const pdf1d_builder& b)
{
  builder_ = b;
}

//: Number of samples per data-point used in estimating overlap
void pdf1d_compare_to_pdf_bhat::set_n_per_point(int n)
{
  n_per_point_ = n;
}

//=======================================================================

//: Test whether data came from the given distribution
double pdf1d_compare_to_pdf_bhat::compare(const double* data, int n,
                                          const pdf1d_pdf& model_pdf)
{
  if (!pdf_.isDefined() || pdf_->is_a()!=builder().new_model_type())
    pdf_ = builder().new_model();

  builder().build_from_array(pdf_,data,n);

   // For clarity and to avoid compiler warnings
  const pdf1d_pdf* built_pdf = pdf_.ptr();

  // Use integral overlap
  if (use_integration_for_gaussian && model_pdf.is_class("pdf1d_gaussian"))
    return pdf1d_bhat_overlap_gaussian_with_pdf(model_pdf,*built_pdf);

  return pdf1d_bhat_overlap(*built_pdf,model_pdf,n_per_point_*n);
}

//=======================================================================

//: Test whether data has form of the given distribution
double pdf1d_compare_to_pdf_bhat::bootstrap_compare_form(vnl_vector<double>& B,
                                                         const double* data, int n,
                                                         const pdf1d_builder& builder2, int n_trials)
{
  // Fit a general PDF (typically a kernel pdf) to the data and
  // sample from it to get samples x and associated prob.s p
  if (!pdf_.isDefined() || pdf_->is_a()!=builder().new_model_type())
    pdf_ = builder().new_model();
  builder().build_from_array(pdf_,data,n);

  bool use_gauss_integration = (use_integration_for_gaussian
                                && builder2.new_model_type()=="pdf1d_gaussian");

   // For clarity and to avoid compiler warnings
  const pdf1d_pdf* data_pdf = pdf_.ptr();

  int n_samples = n*n_per_point_;
  vnl_vector<double> x(n_samples),p(n_samples);

  if (!use_gauss_integration)
  {
    pdf1d_sampler* sampler = data_pdf->new_sampler();
    sampler->regular_samples_and_prob(x,p);
    delete sampler;
  }

  vnl_vector<double> sample(n);

  pdf1d_pdf* pdf = builder2.new_model();

  B.set_size(n_trials);

  double sum = 0;
  double b;
  for (int i=0;i<n_trials;++i)
  {
    // Build pdf from resampled data.
    // Check resampled data is not all identical
    double s_mean,s_var=0;
    while (s_var<1e-9)
    {
      pdf1d_resample(sample,data,n);
      pdf1d_calc_mean_var(s_mean,s_var,sample);
    }

    builder2.build_from_array(*pdf,sample.data_block(),n);

    // Test overlap of pdf with pdf around original data
    if (use_gauss_integration)
      b = pdf1d_bhat_overlap_gaussian_with_pdf(*pdf,*data_pdf);
    else
      b = pdf1d_bhat_overlap(*pdf,x.data_block(),p.data_block(),n_samples);

    B[i] = b;
    sum+=b;
  }

  delete pdf;

  return sum/n_trials;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string pdf1d_compare_to_pdf_bhat::is_a() const
{
  return vcl_string("pdf1d_compare_to_pdf_bhat");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_compare_to_pdf_bhat::is_class(vcl_string const& s) const
{
  return pdf1d_compare_to_pdf::is_class(s) || s==pdf1d_compare_to_pdf_bhat::is_a();
}

//=======================================================================
// Method: version_no
//=======================================================================

short pdf1d_compare_to_pdf_bhat::version_no() const
{
  return 1;
}

//=======================================================================
// Method: clone
//=======================================================================

pdf1d_compare_to_pdf* pdf1d_compare_to_pdf_bhat::clone() const
{
  return new pdf1d_compare_to_pdf_bhat(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void pdf1d_compare_to_pdf_bhat::print_summary(vcl_ostream& os) const
{
  os << "Builder: "<<builder_<<vcl_endl;
}

//=======================================================================
// Method: save
//=======================================================================

void pdf1d_compare_to_pdf_bhat::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,builder_);
  vsl_b_write(bfs,n_per_point_);
}

//=======================================================================
// Method: load
//=======================================================================

void pdf1d_compare_to_pdf_bhat::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,builder_);
      vsl_b_read(bfs,n_per_point_);
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, pdf1d_compare_to_pdf_bhat &)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

