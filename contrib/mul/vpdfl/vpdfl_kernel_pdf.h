// This is mul/vpdfl/vpdfl_kernel_pdf.h
#ifndef vpdfl_kernel_pdf_h
#define vpdfl_kernel_pdf_h
//:
// \file
// \brief Multi-variate kernel PDF
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_pdf_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Multi-variate kernel PDF
//  Distribution is the sum of a set of kernel functions placed on the training
//  samples.
//
//  Essentially a special case of a mixture model.  It differs from
//  the former in that a) every component has the same shape, though
//  possibly a different width b) every component has the same weight
//  c) Usually one uses different training algorithms.
class vpdfl_kernel_pdf : public vpdfl_pdf_base
{
 protected:
  //: Position of kernel centres
  std::vector<vnl_vector<double> > x_;

  //: Width of each kernel
  vnl_vector<double> width_;

  //: True if all kernels have the same width
  bool all_same_width_;

  //: Compute mean/variance given current centres and widths
  void calc_mean_var();

 public:

  //: Dflt ctor
  vpdfl_kernel_pdf();

  //: Destructor
  ~vpdfl_kernel_pdf() override;

  //: Initialise so all kernels have the same width.
  //  width is essentially the sd on the kernels
  void set_centres(const vnl_vector<double>* x, int n, double width);

  //: Initialise so all kernels have given width.
  //  width[i] is essentially the sd on kernel i
  void set_centres(const vnl_vector<double>* x, int n,
                   const vnl_vector<double>& width);

  //: Position of kernel centres
  const std::vector<vnl_vector<double> >& centre() const { return x_; }

  //: Width of each kernel
  const vnl_vector<double>& width() const { return width_; }

  //: True if all kernels have the same width
  bool all_same_width() const { return all_same_width_; }

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

#endif // vpdfl_kernel_pdf_h
