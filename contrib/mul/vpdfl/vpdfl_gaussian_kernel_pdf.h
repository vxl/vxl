// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf.h
#ifndef vpdfl_gaussian_kernel_pdf_h
#define vpdfl_gaussian_kernel_pdf_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Multi-variate spherical gaussian kernel PDF.
// \author Tim Cootes

#include <vpdfl/vpdfl_kernel_pdf.h>
#include <vcl_iosfwd.h>

//: Multi-variate spherical gaussian kernel PDF.
class vpdfl_gaussian_kernel_pdf : public vpdfl_kernel_pdf
{
 public:
  //: Dflt ctor
  vpdfl_gaussian_kernel_pdf();

  //: Destructor
  virtual ~vpdfl_gaussian_kernel_pdf();

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  virtual vpdfl_sampler_base* new_sampler() const;

  //: Probability density at x
  virtual double operator()(const vnl_vector<double>& x) const;

  //: Log of probability density at x
  // This value is also the Normalised Mahalanobis distance
  // from the centroid to the given vector.
  virtual double log_p(const vnl_vector<double>& x) const;

  //: Gradient of PDF at x
  virtual void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x,
                        double& p) const;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  virtual void nearest_plausible(vnl_vector<double>& x, double log_p_min) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_pdf_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // vpdfl_gaussian_kernel_pdf_h
