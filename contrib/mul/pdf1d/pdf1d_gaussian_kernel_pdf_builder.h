// This is mul/pdf1d/pdf1d_gaussian_kernel_pdf_builder.h
#ifndef pdf1d_gaussian_kernel_pdf_builder_h
#define pdf1d_gaussian_kernel_pdf_builder_h

//:
// \file
// \author Tim Cootes
// \brief Builds gaussian kernel pdfs

#include <pdf1d/pdf1d_kernel_pdf_builder.h>
#include <vcl_iosfwd.h>

//=======================================================================

class pdf1d_gaussian_kernel_pdf;

//: Build gaussian kernel pdf objects
//  Build functions are implemented in the pdf1d_kernel_pdf_builder parent
class pdf1d_gaussian_kernel_pdf_builder : public pdf1d_kernel_pdf_builder
{
  pdf1d_gaussian_kernel_pdf& gkpdf(pdf1d_pdf& model) const;
 public:

  //: Dflt ctor
  pdf1d_gaussian_kernel_pdf_builder();

  //: Destructor
  virtual ~pdf1d_gaussian_kernel_pdf_builder();

  //: Create empty model
  virtual pdf1d_pdf* new_model() const;

  //: Name of the model class returned by new_model()
  virtual vcl_string new_model_type() const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;
};

#endif // pdf1d_gaussian_kernel_pdf_builder_h
