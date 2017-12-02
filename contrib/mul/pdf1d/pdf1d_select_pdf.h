#ifndef pdf1d_select_pdf_h
#define pdf1d_select_pdf_h
//:
// \file
// \author Tim Cootes
// \brief Functions to select a pdf or form of pdf for given data.

#include <pdf1d/pdf1d_compare_to_pdf.h>
#include <pdf1d/pdf1d_compare_to_pdf_bhat.h>


//: Use the comparator to decide which pdf the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf(const double* data, int n,
                     std::vector<const pdf1d_pdf*>& pdf,
                     pdf1d_compare_to_pdf& comparator);

//: Use the comparator to decide which pdf the data is most like
//  Typically matches data with pdf_builder[i] and computes difference
//  between result and pdf[i]
//  Returns index of the chosen pdf.
int pdf1d_select_pdf(const double* data, int n,
                     std::vector<const pdf1d_pdf*>& pdf,
                     std::vector<pdf1d_builder*>& pdf_builder,
                     pdf1d_compare_to_pdf_bhat& comparator);

//: Use the comparator to decide which pdf the data is most like
//  Returns index of the chosen pdf.
//  Uses bootstrap estimation of overlap
int pdf1d_select_pdf_using_bootstrap(const double* data, int n,
                     std::vector<const pdf1d_pdf*>& pdf,
                     pdf1d_compare_to_pdf& comparator);

//: Use the comparator to decide which pdf form the data is most like
//  Returns index of the chosen pdf.
int pdf1d_select_pdf_form(const double* data, int n,
                     std::vector<pdf1d_builder*>& pdf_builder,
                     pdf1d_compare_to_pdf& comparator);


#endif // pdf1d_select_pdf_h
