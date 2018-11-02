// This is mul/pdf1d/pdf1d_epanech_kernel_pdf_builder.h
#ifndef pdf1d_epanech_kernel_pdf_builder_h
#define pdf1d_epanech_kernel_pdf_builder_h

//:
// \file
// \author Tim Cootes
// \brief Builds Epanechnikov kernel pdfs

#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_kernel_pdf_builder.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

class pdf1d_epanech_kernel_pdf;

//: Build Epanechnikov kernel pdf objects
//  Build functions are implemented in the pdf1d_kernel_pdf_builder parent
class pdf1d_epanech_kernel_pdf_builder : public pdf1d_kernel_pdf_builder
{
  pdf1d_epanech_kernel_pdf& gkpdf(pdf1d_pdf& model) const;
 public:

  //: Dflt ctor
  pdf1d_epanech_kernel_pdf_builder();

  //: Destructor
  ~pdf1d_epanech_kernel_pdf_builder() override;

  //: Create empty model
  pdf1d_pdf* new_model() const override;

  //: Name of the model class returned by new_model()
  std::string new_model_type() const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;
};

#endif // pdf1d_epanech_kernel_pdf_builder_h
