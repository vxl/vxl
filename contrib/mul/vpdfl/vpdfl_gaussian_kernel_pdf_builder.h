// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf_builder.h
#ifndef vpdfl_gaussian_kernel_pdf_builder_h
#define vpdfl_gaussian_kernel_pdf_builder_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \brief Builder for vpdfl_gaussian_kernel_pdf objects

#include <vpdfl/vpdfl_kernel_pdf_builder.h>

//=======================================================================

class vpdfl_gaussian_kernel_pdf;

//: Class to build vpdfl_gaussian_kernel_pdf objects
class vpdfl_gaussian_kernel_pdf_builder : public vpdfl_kernel_pdf_builder
{
 public:

  //: Dflt ctor
  vpdfl_gaussian_kernel_pdf_builder();

  //: Destructor
  virtual ~vpdfl_gaussian_kernel_pdf_builder();

  //: Create empty model
  virtual vpdfl_pdf_base* new_model() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_builder_base* clone() const;
};

#endif // vpdfl_gaussian_kernel_pdf_builder_h
