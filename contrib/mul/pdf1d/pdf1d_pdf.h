// This is mul/pdf1d/pdf1d_pdf.h
#ifndef pdf1d_pdf_h
#define pdf1d_pdf_h

//:
// \file
// \brief Base class for Univariate Probability Density Function classes.
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <vnl/vnl_vector.h> // HACK: IMS: don't replace this with vnl_fwd.h

//=======================================================================

class pdf1d_sampler;

//: Base class for Univariate Probability Density Function classes.
// Functions are available to test the plausibility of a vector or
// set of parameters, to modify a set of parameters so it is plausible
// and to choose a threshold of plausibility.  Also, for cases where
// the distributions of parameters are multi-modal, the number and
// centres of each peak can be recorded.
// This is particularly useful for non-linear and mixture model
// representations of the parameter distributions.
class pdf1d_pdf
{
  double mean_;
  double var_;
 protected:
  void set_mean(double m) { mean_ = m; }
  void set_variance(double v) { var_ = v; }
 public:

  //: Dflt ctor
  pdf1d_pdf();

  //: Destructor
  virtual ~pdf1d_pdf();

  //: Mean of distribution
  double mean() const { return mean_; }

  //: Variance of each dimension
  double variance() const { return var_; }

  //: Number of peaks of distribution
  virtual int n_peaks() const { return 1; }

  //: Position of the i'th peak
  virtual double peak(int) const { return mean_; }

  //: Log of probability density at x
  virtual double log_p(double x) const =0;

  //: Probability density at x
  virtual double operator()(double x) const;

  //: Cumulative Probability (P(x'<x) for x' drawn from the distribution.
  //  By default this can be calculated by drawing random samples from
  //  the distribution and computing the number less than x.
  virtual double cdf(double x) const;

  //: Return true if cdf() uses an analytic implementation.
  //  Default is false, as the base implementation is to draw samples
  //  from the distribution randomly to estimate cdf(x)
  virtual bool cdf_is_analytic() const;

  //: The inverse cdf.
  // The value of x: P(x'<x) = P for x' drawn from distribution pdf.
  // The default version of this algorithm uses sampling if !cdf_is_analytic(),
  // and Newton-Raphson root finding otherwise.
  virtual double inverse_cdf(double P) const;

  //: Gradient and value of PDF at x
  //  Computes gradient of PDF at x, and returns the prob at x in p
  virtual double gradient(double x, double& p) const =0;

  //: Create a sampler object on the heap
  // Caller is responsible for deletion.
  virtual pdf1d_sampler* new_sampler()const=0 ;

  //: Compute threshold for PDF to pass a given proportion
  virtual double log_prob_thresh(double pass_proportion) const;

  //: Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x returned unchanged.  Otherwise move
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  virtual double nearest_plausible(double x, double log_p_min)const =0;

  //: Return true if the object represents a valid PDF.
  // This will return false, if n_dims() is 0, for example just ofter
  // default construction.
  virtual bool is_valid_pdf() const;

  //: Fill x with samples drawn from distribution
  //  Utility function. This calls new_sampler() to
  //  do the work, then deletes the sampler again.
  //  If you intend calling this repeatedly, create
  //  a sampler yourself.
  void get_samples(vnl_vector<double>& x) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_pdf* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;

  //: Write values (x,p(x)) to text file suitable for plotting
  //  Evaluate pdf at n points in range [min_x,max_x] and write a text file,
  //  each line of which is {x p(x)}, suitable for plotting with many graph packages
  bool write_plot_file(const vcl_string& plot_file, double min_x, double max_x, int n) const;
};

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "bfs>>base_ptr;".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const pdf1d_pdf& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const pdf1d_pdf& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, pdf1d_pdf& b);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const pdf1d_pdf& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const pdf1d_pdf* b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_pdf& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_pdf* b);

#endif // pdf1d_pdf_h
