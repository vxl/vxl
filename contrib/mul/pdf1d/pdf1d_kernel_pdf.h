// This is mul/pdf1d/pdf1d_kernel_pdf.h
#ifndef pdf1d_kernel_pdf_h
#define pdf1d_kernel_pdf_h

//:
// \file
// \brief Univariate kernel_pdf PDF
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_pdf.h>
#include <vnl/vnl_vector.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Class for univariate kernel based PDFs.
//  Distribution is the sum of a set of kernel functions placed on the training
//  samples.
//
//  Essentially a special case of a mixture model.  It differs from
//  the former in that a) every component has the same shape, though
//  possibly a different width b) every component has the same weight
//  c) Usually one uses different training algorithms.
//  Mixture models are more general.
class pdf1d_kernel_pdf : public pdf1d_pdf
{
 protected:
  //: Position of kernel centres
  vnl_vector<double> x_;

  //: Width of each kernel
  vnl_vector<double> width_;

  //: Workspace
  // If non-empty x_(index[i]) <= x_(index[i+1])
  mutable std::vector<int> index_;

  //: True if all kernels have the same width
  bool all_same_width_;

 public:
  //: Dflt ctor
  pdf1d_kernel_pdf();

  //: Destructor
  ~pdf1d_kernel_pdf() override;

  //: Initialise so all kernels have the same width
  virtual void set_centres(const vnl_vector<double>& x, double width);

  //: Initialise so all kernels have given width
  virtual void set_centres(const vnl_vector<double>& x,
                           const vnl_vector<double>& width);

  //: Position of kernel centres
  const vnl_vector<double>& centre() const { return x_; }

  //: Width of each kernel
  const vnl_vector<double>& width() const { return width_; }

  //: True if all kernels have the same width
  bool all_same_width() const { return all_same_width_; }

  //: The inverse cdf.
  // The value of x: P(x'<x) = P for x' drawn from distribution pdf.
  // Uses Newton-Raphson.
  double inverse_cdf(double P) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // pdf1d_kernel_pdf_h
