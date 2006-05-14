// This is mul/pdf1d/pdf1d_flat.h
#ifndef pdf1d_flat_h
#define pdf1d_flat_h

//:
// \file
// \brief Univariate flat PDF
// \author Tim Cootes

#include <vcl_cmath.h>
#include <vcl_iosfwd.h>
#include <pdf1d/pdf1d_pdf.h>

//: Class for univariate flat distributions: p(x)=1/(hi-lo) in [lo,hi]
//  In range [lo,hi] p(x)=1/(hi-lo)
class pdf1d_flat : public pdf1d_pdf
{
  double lo_,hi_;
  double p_;
  double log_p_;

 public:
  //: Dflt ctor (creates flat distribution in range [0,1])
  pdf1d_flat();

  //: Dflt ctor (creates flat distribution in range [lo,hi])
  pdf1d_flat(double lo, double hi);

  //: Destructor
  virtual ~pdf1d_flat();

  //: Return standard deviation
  double sd() const { return vcl_sqrt(variance()); }

  //: Creates flat distribution in range [lo,hi]
  void set(double lo, double hi);

  //: Lower limit of range
  double lo() const { return lo_; }

  //: Upper limit of range
  double hi() const { return hi_; }


  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  virtual pdf1d_sampler* new_sampler() const;

  //: Probability density at x
  virtual double operator()(double x) const;

  //: Log of probability density at x
  // This value is also the Normalised Mahalanobis distance
  // from the centroid to the given vector.
  virtual double log_p(double x) const;

  //: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
  virtual double cdf(double x) const;

  //: Return true if cdf() uses an analytic implementation
  virtual bool cdf_is_analytic() const;

  //: Gradient of PDF at x
  virtual double gradient(double x, double& p) const;

  //: Compute threshold for PDF to pass a given proportion
  virtual double log_prob_thresh(double pass_proportion) const;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x returned unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  virtual double nearest_plausible(double x, double log_p_min) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_pdf* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;


  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // pdf1d_flat_h
