// This is mul/pdf1d/pdf1d_mixture.h
#ifndef pdf1d_mixture_h_
#define pdf1d_mixture_h_
//:
// \file
// \brief Implements a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes and Ian Scott

//=======================================================================

#include <vector>
#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_pdf.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

//: Represents a mixture model (a set of individual pdfs + weights)
class pdf1d_mixture : public pdf1d_pdf
{
  std::vector<pdf1d_pdf*> component_;
  std::vector<double>     weight_;

  void init();
  void delete_stuff();
 public:

  //: Dflt ctor
  pdf1d_mixture();

  //: Copy ctor
  pdf1d_mixture(const pdf1d_mixture&);

  //: Copy operator
  pdf1d_mixture& operator=(const pdf1d_mixture&);

  //: Destructor
  ~pdf1d_mixture() override;

  //: Probability density at x
  double operator()(double x) const override;

  //: Log of probability density at x
  double log_p(double x) const override;

  //: Cumulative Probability (P(x'<x) for x' drawn from the distribution)
  double cdf(double x) const override;

  //: Return true if cdf() uses an analytic implementation
  bool cdf_is_analytic() const override;

  //: Gradient of PDF at x
  double gradient(double x, double& p) const override;

  //: Not Yet Implemented
  // Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  double nearest_plausible(double x, double log_p_min) const override;

  //: Initialise to use n components of type comp_type
  //  Clones taken by comp_type
  void init(const pdf1d_pdf& comp_type, int n);

  //: Return instance object for this PDF
  //  Object is created on heap. Caller responsible for deletion.
  pdf1d_sampler * new_sampler() const override;

  //: Number of components in mixture
  unsigned n_components() const { return component_.size(); }

  //: Get i<I>th</I> weight.
  double weight(unsigned i) const { return weight_[i]; }

  //: Array of weights
  // Use weight(i) where possible
  const std::vector<double>& weights() const { return weight_; }

  //: Array of weights
  // Warning care must be taken to ensure consistency when modifying weights
  // Warning. Use weight(i) where possible
  std::vector<double>& weights() { return weight_; }

  //: Return index of component nearest to x
  unsigned nearest_comp(double x) const;

  //: Add a component to current model
  //  Clone taken of comp
  void add_component(const pdf1d_pdf& comp);

  //: Remove all components cleanly
  void clear();

  //: Get i<I>th</I> component.
  const pdf1d_pdf & component(unsigned i) const { return *component_[i]; }

  //: Access to components - for use by builders
  //  Care must be taken to ensure consistency when modifying
  // Use component(i) where possible
  std::vector<pdf1d_pdf*>& components() { return component_; }

  //: Access to components - for use by builders
  // Use component(i) where possible
  const std::vector<pdf1d_pdf*>& components() const { return component_; }

  //: Set the whole pdf mean and variance values.
  // Components and Weights should already be correct so that
  // the error checking can work.
  // #define NDEBUG to turn off error checking.
  void set_mean_and_variance(double m, double v);

  //: Return true if the object represents a valid PDF.
  // This will return false, if n_dims() is 0, for example just ofter
  // default construction.
  bool is_valid_pdf() const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_pdf* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // pdf1d_mixture_h_
