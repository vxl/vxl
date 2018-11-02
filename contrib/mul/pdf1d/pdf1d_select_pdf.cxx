// This is mul/pdf1d/pdf1d_select_pdf.cxx
#include "pdf1d_select_pdf.h"
//:
// \file
// \author Tim Cootes
// \brief Functions to select a pdf or form of pdf for given data.

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>

//: Use the comparator to decide which pdf the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf(const double* data, int n, std::vector<const pdf1d_pdf*>& pdf,
                     pdf1d_compare_to_pdf& comparator)
{
  assert(pdf.size()>0);

  int best_i = 0;
  double best_B = comparator.compare(data,n,*pdf[0]);
  for (unsigned int i=1;i<pdf.size();++i)
  {
    double B = comparator.compare(data,n,*pdf[i]);
    if (B>best_B)
    {
      best_B=B;
      best_i = i;
    }
  }

  return best_i;
}

//: Use the comparator to decide which pdf the data is most like
//  Typically matches data with pdf_builder[i] and computes difference
//  between result and pdf[i]
//  Returns index of the chosen pdf.
int pdf1d_select_pdf(const double* data, int n,
                     std::vector<const pdf1d_pdf*>& pdf,
                     std::vector<pdf1d_builder*>& pdf_builder,
                     pdf1d_compare_to_pdf_bhat& comparator)
{
  assert(pdf.size()>0);

  int best_i = 0;
  double best_B = 0.0;
  for (unsigned int i=0;i<pdf.size();++i)
  {
    comparator.set_builder(*pdf_builder[i]);
    double B = comparator.compare(data,n,*pdf[i]);
    if (i==0 || B>best_B)
    {
      best_B=B;
      best_i = i;
    }
  }

  return best_i;
}

//: Use the comparator to decide which pdf the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf_using_bootstrap(const double* data, int n, std::vector<const pdf1d_pdf*>& pdf,
                     pdf1d_compare_to_pdf& comparator)
{
  assert(pdf.size()>0);

  int n_trials = 20;
  vnl_vector<double> B(n_trials);

  int best_i = 0;
  double best_B = comparator.bootstrap_compare(B,data,n,*pdf[0],n_trials);
  for (unsigned int i=1;i<pdf.size();++i)
  {
    double B_mean = comparator.bootstrap_compare(B,data,n,*pdf[i],n_trials);
    if (B_mean>best_B)
    {
      best_B=B_mean;
      best_i = i;
    }
  }

  return best_i;
}

//: Use the comparator to decide which pdf form the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf_form(const double* data, int n,
                     std::vector<pdf1d_builder*>& pdf_builder,
                     pdf1d_compare_to_pdf& comparator)
{
  assert(pdf_builder.size()>0);

  int best_i = 0;
  double best_B = comparator.compare_form(data,n,*pdf_builder[0]);
  for (unsigned int i=1;i<pdf_builder.size();++i)
  {
    double B = comparator.compare_form(data,n,*pdf_builder[i]);
    if (B>best_B)
    {
      best_B=B;
      best_i = i;
    }
  }

  return best_i;
}
