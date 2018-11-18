// This is mul/pdf1d/pdf1d_compare_to_pdf.cxx

//:
// \file
// \author Tim Cootes
// \brief Base for classes with test whether data could come from a given pdf.

#include "pdf1d_compare_to_pdf.h"
#include <pdf1d/pdf1d_pdf.h>
#include <pdf1d/pdf1d_resample.h>
#include <vsl/vsl_indent.h>
#include <pdf1d/pdf1d_calc_mean_var.h>

//=======================================================================
// Dflt ctor
//=======================================================================

pdf1d_compare_to_pdf::pdf1d_compare_to_pdf() = default;

//=======================================================================
// Destructor
//=======================================================================

pdf1d_compare_to_pdf::~pdf1d_compare_to_pdf() = default;


//: Test whether data came from the given distribution, using bootstrap
//  Repeatedly resamples n values from data[0..n-1] and compares with
//  the given pdf.  Individual comparisons are returned in B.
//  \return Mean of B
double pdf1d_compare_to_pdf::bootstrap_compare(vnl_vector<double>& B,
                                               const double* data, int n,
                                               const pdf1d_pdf& pdf, int n_trials)
{
  vnl_vector<double> sample(n);

  B.set_size(n_trials);

  double sum = 0;
  for (int i=0;i<n_trials;++i)
  {
    // Check resampled data is not all identical
    double s_mean,s_var=0;
    while (s_var<1e-9)
    {
      pdf1d_resample(sample,data,n);
      pdf1d_calc_mean_var(s_mean,s_var,sample);
    }

    double c = compare(sample.data_block(),n,pdf);
    B[i] = c;
    sum+=c;
  }

  return sum/n_trials;
}

//: Test whether data has form of the given distribution
//  Default behaviour is to build pdf from data and then compare data with pdf
double pdf1d_compare_to_pdf::compare_form(const double* data, int n,
                              const pdf1d_builder& builder)
{
  if (!pdf_.isDefined() || pdf_->is_a()!=builder.new_model_type())
    pdf_ = builder.new_model();

  builder.build_from_array(pdf_,data,n);
  return compare(data,n,*(pdf_.ptr()));
}

//: Test whether data has form of the given distribution
double pdf1d_compare_to_pdf::bootstrap_compare_form(vnl_vector<double>& B,
                              const double* data, int n,
                              const pdf1d_builder& builder, int n_trials)
{
  vnl_vector<double> sample(n);

  if (!pdf_.isDefined() || pdf_->is_a()!=builder.new_model_type())
    pdf_ = builder.new_model();

  B.set_size(n_trials);

  double sum = 0;
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

    builder.build_from_array(pdf_,sample.data_block(),n);

    // Test overlap of pdf with original data
    double c = compare(data,n,*(pdf_.ptr()));
    B[i] = c;
    sum+=c;
  }

  return sum/n_trials;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string pdf1d_compare_to_pdf::is_a() const
{
  return std::string("pdf1d_compare_to_pdf");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool pdf1d_compare_to_pdf::is_class(std::string const& s) const
{
  return s==pdf1d_compare_to_pdf::is_a();
}


void vsl_print_summary(std::ostream& os,const pdf1d_compare_to_pdf& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
}

//=======================================================================

void vsl_print_summary(std::ostream& os,const pdf1d_compare_to_pdf* b)
{
  if (b)
    vsl_print_summary(os, *b);
  else
    os << "No pdf1d_compare_to_pdf defined.";
}
