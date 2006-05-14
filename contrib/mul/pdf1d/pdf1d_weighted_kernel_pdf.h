// This is mul/pdf1d/pdf1d_weighted_kernel_pdf.h
#ifndef pdf1d_weighted_kernel_pdf_h
#define pdf1d_weighted_kernel_pdf_h

//:
// \file
// \brief Univariate weighted kernel_pdf PDF
// \author Ian Scott

#include <pdf1d/pdf1d_kernel_pdf.h>
#include <vcl_iosfwd.h>

//: Class for univariate kernel based PDFs.
//  Distribution is the sum of a set of kernel functions placed on the training
//  samples.
//
//  Essentially a special case of a mixture model.  It differs from
//  the former in that a) every component has the same shape, though
//  possibly a different width
//  b) Usually one uses different training algorithms.
//  Mixture models are more general.
//
//  Use a normal kernel pdf builder to build this, and then
//  set the weights separately.
class pdf1d_weighted_kernel_pdf : public pdf1d_kernel_pdf
{
 protected:
  //: Weight of each kernel
  vnl_vector<double> weight_;
  double sum_weights_;
 public:

  //:calc the weighted mean and var of kernels.
  // w is expected to sum to n.
  static void pdf1d_weighted_kernel_mean_var(double& mean, double& var,
                                             const vnl_vector<double>& centres,
                                             const vnl_vector<double>& widths,
                                             const vnl_vector<double>& weights);

  //: Set the weights.
  // The weights will be scaled to sum to 1.0
  void set_weight(const vnl_vector<double>& weights);

  //: Swap in the weights values.
  // This function is fast when you are changing the weights regularly
  // The weights will be scaled to sum to 1.0
  void swap_weight(vnl_vector<double>& weights);

  //: Weight of each kernel
  const vnl_vector<double>& weight() const { return weight_; }

  //: Initialise so all kernels have the same width
  // This function resets the weights.
  virtual void set_centres(const vnl_vector<double>& x, double width);

  //: Initialise so all kernels have given width
  // This function resets the weights.
  virtual void set_centres(const vnl_vector<double>& x,
                           const vnl_vector<double>& width);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // pdf1d_weighted_kernel_pdf_h
