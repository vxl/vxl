// This is mul/pdf1d/pdf1d_kernel_pdf_builder.h
#ifndef pdf1d_kernel_pdf_builder_h
#define pdf1d_kernel_pdf_builder_h

//:
// \file
// \author Tim Cootes
// \brief Initialises kernel pdfs

#include <pdf1d/pdf1d_builder.h>
#include <vcl_iosfwd.h>

//=======================================================================

class pdf1d_kernel_pdf;

//: Build kernel pdf objects.
//  Contains algorithms for selecting kernel widths.
//
//  Simplest is to use equal widths (set_use_equal_width()).
//
//  More interesting is an adaptive kernel estimate (set_use_adaptive()).
//  This tends to get results comparable with the equal width method for
//  simple cases, but can match to more complex distributions more easily.
//  In particular, it tends to approximate the tails more accurately.
//
//  See book on Density Estimation by B.W.Silverman (Pub. Chapman and Hall, 1986)
//  for details.
class pdf1d_kernel_pdf_builder : public pdf1d_builder
{
 public:
  enum build_type { fixed_width, select_equal, width_from_sep, adaptive };
 private:
  //: Minimum variance of whole model
  double min_var_;

  //: Type of building to be performed
  build_type build_type_;

  //: Width set if fixed_width option on build used
  double fixed_width_;

  pdf1d_kernel_pdf& kernel_pdf(pdf1d_pdf& model) const;
 public:

  //: Dflt ctor
  pdf1d_kernel_pdf_builder();

  //: Destructor
  virtual ~pdf1d_kernel_pdf_builder();

  //: Use fixed width kernels of given width when building.
  void set_use_fixed_width(double width);

  //: Use equal width kernels of width depending on number of samples.
  // This method appears to give a lower density near the tails
  void set_use_equal_width();

  //: Kernel width proportional to distance to nearby samples.
  void set_use_width_from_separation();

  //: Build adaptive kernel estimate.
  void set_use_adaptive();

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Build from n elements in data[i]
  virtual void build_from_array(pdf1d_pdf& model, const double* data, int n) const;

  //: Build default model with given mean
  virtual void build(pdf1d_pdf& model, double mean) const;

  //: Build model from data
  // The kernel centres in the pdf will have same value and order as
  // the training data
  virtual void build(pdf1d_pdf& model,
                     mbl_data_wrapper<double>& data) const;

  //: Build model from weighted data
  virtual void weighted_build(pdf1d_pdf& model,
                              mbl_data_wrapper<double>& data,
                              const vcl_vector<double>& wts) const;

  //: Build from n elements in data[i].  Fixed kernel width.
  // The kernel centres in the pdf will have same value and order as
  // the training data
  void build_fixed_width(pdf1d_kernel_pdf& kpdf,
                         const double* data, int n, double width) const;

  //: Build from n elements in data[i].  Chooses width.
  //  Same width selected for all points, using
  //  $w=(3n/4)^{-0.2}\sigma$, as suggested by Silverman
  //
  // The kernel centres in the pdf will have same value and order as
  // the training data
  void build_select_equal_width(pdf1d_kernel_pdf& kpdf,
                                const double* data, int n) const;

  //: Kernel width proportional to distance to nearby samples.
  // The kernel centres in the pdf will have same value and order as
  // the training data
  void build_width_from_separation(pdf1d_kernel_pdf& kpdf,
                                   const double* data, int n) const;

  //: Build adaptive kernel estimate.
  //  Use equal widths to create a pilot estimate, then use the prob at each
  //  data point to modify the widths.
  // Uses Silverman's equation 5.8 with alpha = 0.5 as suggested, and
  // based on a pilot estimate as calculated by build_select_equal_width().
  // The kernel centres in the pdf will have same value and order as
  // the training data.
  //
  // This method gives a significantly higher density near the edges
  // of the distribution than suggested by the cumulative histogram of
  // the training data,
  // pushing the cdf estimate closer to 0.5 at the edges. If you want to
  // approximate the cumulative histogram more closely, then
  // build_select_equal_width() may be more effective.
  void build_adaptive(pdf1d_kernel_pdf& kpdf,
                      const double* data, int n) const;

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

#endif // pdf1d_kernel_pdf_builder_h
