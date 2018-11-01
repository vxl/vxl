// This is mul/vpdfl/vpdfl_gaussian_kernel_pdf.h
#ifndef vpdfl_gaussian_kernel_pdf_h
#define vpdfl_gaussian_kernel_pdf_h
//:
// \file
// \brief Multi-variate spherical gaussian kernel PDF.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_kernel_pdf.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Multi-variate spherical gaussian kernel PDF.
class vpdfl_gaussian_kernel_pdf : public vpdfl_kernel_pdf
{
 public:
  //: Dflt ctor
  vpdfl_gaussian_kernel_pdf();

  //: Destructor
  ~vpdfl_gaussian_kernel_pdf() override;

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  vpdfl_sampler_base* new_sampler() const override;

  //: Probability density at x
  double operator()(const vnl_vector<double>& x) const override;

  //: Log of probability density at x
  // This value is also the Normalised Mahalanobis distance
  // from the centroid to the given vector.
  double log_p(const vnl_vector<double>& x) const override;

  //: Gradient of PDF at x
  void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x,
                        double& p) const override;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  void nearest_plausible(vnl_vector<double>& x, double log_p_min) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_pdf_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vpdfl_gaussian_kernel_pdf_h
