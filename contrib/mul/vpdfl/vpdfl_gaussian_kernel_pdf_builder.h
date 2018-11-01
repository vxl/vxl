// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf_builder.h
#ifndef vpdfl_gaussian_kernel_pdf_builder_h
#define vpdfl_gaussian_kernel_pdf_builder_h
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
  ~vpdfl_gaussian_kernel_pdf_builder() override;

  //: Create empty model
  vpdfl_pdf_base* new_model() const override;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_builder_base* clone() const override;
};

#endif // vpdfl_gaussian_kernel_pdf_builder_h
