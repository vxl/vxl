// This is mul/vpdfl/vpdfl_kernel_pdf_builder.h
#ifndef vpdfl_kernel_pdf_builder_h
#define vpdfl_kernel_pdf_builder_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \brief Initialises kernel pdfs

#include <vpdfl/vpdfl_builder_base.h>
#include <vcl_iosfwd.h>

//=======================================================================

class vpdfl_kernel_pdf;

//: Build kernel pdf objects.
//  Contains algorithms for selecting kernel widths.
//
//  Simplest is to use equal widths (set_use_equal_width()).
//
//  A fixed width can be supplied (set_use_fixed_width(w))
//
//  The widths can be estimated from the proximity of neighbours
//  set_use_width_from_separation()
//
//  More interesting is an adaptive kernel estimate (set_use_adaptive()).
//  This tends to get results comparable with the equal width method for
//  simple cases, but can match to more complex distributions more easily.
//  In particular, it tends to approximate the tails more accurately.
//
//  See book on Density Estimation by B.W.Silverman (Pub. Chapman and Hall, 1986)
//  for details.
class vpdfl_kernel_pdf_builder : public vpdfl_builder_base
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

  vpdfl_kernel_pdf& kernel_pdf(vpdfl_pdf_base& model) const;
 public:

  //: Dflt ctor
  vpdfl_kernel_pdf_builder();

  //: Destructor
  virtual ~vpdfl_kernel_pdf_builder();

  //: Use fixed width kernels of given width when building.
  void set_use_fixed_width(double width);

  //: Use equal width kernels of width depending on number of samples.
  void set_use_equal_width();

  //: Kernel width proportional to distance to nearby samples.
  void set_use_width_from_separation();

  //: Build adaptive kernel estimate.
  void set_use_adaptive();

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Build default model with given mean
  virtual void build(vpdfl_pdf_base& model, const vnl_vector<double>& mean) const;

  //: Build model from data
  virtual void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const;

  //: Build kernel_pdf from n elements in data[i]
  void build_from_array(vpdfl_pdf_base& model,
                        const vnl_vector<double>* data, int n) const;

  //: Build model from weighted data
  virtual void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const vcl_vector<double>& wts) const;

  //: Build from n elements in data[i].  Fixed kernel width.
  void build_fixed_width(vpdfl_kernel_pdf& kpdf,
                         const vnl_vector<double>* data, int n, double width) const;

  //: Build from n elements in data[i].  Chooses width.
  //  Same width selected for all points, using
  //  $w=(4/(2n+d.n)^{1/(d+4)}\sigma$, as suggested by Silverman
  //  Note: This value only suitable for gaussian kernels!
  void build_select_equal_width(vpdfl_kernel_pdf& kpdf,
                                 const vnl_vector<double>* data, int n) const;

  //: Kernel width proportional to distance to nearby samples.
  void build_width_from_separation(vpdfl_kernel_pdf& kpdf,
                                   const vnl_vector<double>* data, int n) const;

  //: Build adaptive kernel estimate.
  //  Use equal widths to create a pilot estimate, then use the prob at each
  //  data point to modify the widths
  void build_adaptive(vpdfl_kernel_pdf& kpdf,
                      const vnl_vector<double>* data, int n) const;

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

  //: Read initialisation settings from a stream.
  // Parameters:
  // \verbatim
  // {
  //   min_var: 1.0e-6
  //   // kernel_widths can be fixed_width,select_equal,width_from_sep,adaptive
  //   kernel_widths: fixed_width
  //   // Width to be used when it is fixed_width
  //   fixed_width: 1.0
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  virtual void config_from_stream(vcl_istream & is);

};

#endif // vpdfl_kernel_pdf_builder_h
