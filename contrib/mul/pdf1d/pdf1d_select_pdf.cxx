// This is mul/pdf1d/pdf1d_select_pdf.cxx
#include "pdf1d_select_pdf.h"
//:
// \file
// \author Tim Cootes
// \brief Functions to select a pdf or form of pdf for given data.

#include <vcl_cassert.h>
#include <vnl/vnl_vector.h>

//: Use the comparitor to decide which pdf the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf(const double* data, int n, vcl_vector<const pdf1d_pdf*>& pdf,
                     pdf1d_compare_to_pdf& comparitor)
{
  assert(pdf.size()>0);

  int best_i = 0;
  double best_B = comparitor.compare(data,n,*pdf[0]);
  for (unsigned int i=1;i<pdf.size();++i)
  {
    double B = comparitor.compare(data,n,*pdf[i]);
    if (B>best_B)
    {
      best_B=B;
      best_i = i;
    }
  }

  return best_i;
}

//: Use the comparitor to decide which pdf the data is most like
//  Typically matches data with pdf_builder[i] and computes difference
//  between result and pdf[i]
//  Returns index of the chosen pdf.
int pdf1d_select_pdf(const double* data, int n,
                     vcl_vector<const pdf1d_pdf*>& pdf,
                     vcl_vector<pdf1d_builder*>& pdf_builder,
                     pdf1d_compare_to_pdf_bhat& comparitor)
{
  assert(pdf.size()>0);

  int best_i = 0;
  double best_B = 0.0;
  for (unsigned int i=0;i<pdf.size();++i)
  {
    comparitor.set_builder(*pdf_builder[i]);
    double B = comparitor.compare(data,n,*pdf[i]);
    if (i==0 || B>best_B)
    {
      best_B=B;
      best_i = i;
    }
  }

  return best_i;
}

//: Use the comparitor to decide which pdf the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf_using_bootstrap(const double* data, int n, vcl_vector<const pdf1d_pdf*>& pdf,
                     pdf1d_compare_to_pdf& comparitor)
{
  assert(pdf.size()>0);

  int n_trials = 20;
  vnl_vector<double> B(n_trials);

  int best_i = 0;
  double best_B = comparitor.bootstrap_compare(B,data,n,*pdf[0],n_trials);
  for (unsigned int i=1;i<pdf.size();++i)
  {
    double B_mean = comparitor.bootstrap_compare(B,data,n,*pdf[i],n_trials);
    if (B_mean>best_B)
    {
      best_B=B_mean;
      best_i = i;
    }
  }

  return best_i;
}

//: Use the comparitor to decide which pdf form the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf_form(const double* data, int n,
                     vcl_vector<pdf1d_builder*>& pdf_builder,
                     pdf1d_compare_to_pdf& comparitor)
{
  assert(pdf_builder.size()>0);

  int best_i = 0;
  double best_B = comparitor.compare_form(data,n,*pdf_builder[0]);
  for (unsigned int i=1;i<pdf_builder.size();++i)
  {
    double B = comparitor.compare_form(data,n,*pdf_builder[i]);
    if (B>best_B)
    {
      best_B=B;
      best_i = i;
    }
  }

  return best_i;
}
