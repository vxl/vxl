// This is mul/vpdfl/vpdfl_mixture.h
// Copyright: (C) 2000 Victoria University of Manchester
#ifndef vpdfl_mixture_h_
#define vpdfl_mixture_h_
//:
// \file
// \brief Implements a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes
// \date 21-July-98
//
// \verbatim
//  Modifications
//   IMS   Converted to VXL 12 May 2000
// \endverbatim

//=======================================================================

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_pdf_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

//: Represents a mixture model (a set of individual pdfs + weights)
class vpdfl_mixture : public vpdfl_pdf_base
{
  std::vector<vpdfl_pdf_base*> component_;
  std::vector<double>          weight_;

  //: Workspace so we don't have to keep creating vectors
  mutable vnl_vector<double> ws_;

  void delete_stuff();
 public:

  //: Dflt ctor
  vpdfl_mixture();

  //: Copy ctor
  vpdfl_mixture(const vpdfl_mixture&);

  //: Copy operator
  vpdfl_mixture& operator=(const vpdfl_mixture&);

  //: Destructor
  ~vpdfl_mixture() override;

  //: Probability density at x
  double operator()(const vnl_vector<double>& x) const override;

  //: Log of probability density at x
  double log_p(const vnl_vector<double>& x) const override;

  //: Gradient of PDF at x
  void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x, double& p) const override;

  //: Not Yet Implemented
  // Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  void nearest_plausible(vnl_vector<double>& x, double log_p_min) const override;

  //: Initialise to use n components of type comp_type
  //  Clones taken by comp_type
  void init(const vpdfl_pdf_base& comp_type, unsigned n);

  //: Return instance object for this PDF
  //  Object is created on heap. Caller responsible for deletion.
  vpdfl_sampler_base * new_sampler() const override;

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
  unsigned nearest_comp(const vnl_vector<double>& x) const;

  //: Set the contents of the mixture model.
  // Clones are taken of all the data, and the class will be responsible for their deletion.
  void set(const std::vector<vpdfl_pdf_base*>& components, const std::vector<double> & weights);

  //: Add a component to current model
  //  Clone taken of comp
  void add_component(const vpdfl_pdf_base& comp);

  //: Remove all components cleanly
  void clear();

  //: Get i<I>th</I> component.
  const vpdfl_pdf_base & component(unsigned i) const { return *component_[i]; }

  //: Access to components - for use by builders
  //  Care must be taken to ensure consistency when modifying
  // Use component(i) where possible
  std::vector<vpdfl_pdf_base*>& components() { return component_; }

  //: Access to components - for use by builders
  // Use component(i) where possible
  const std::vector<vpdfl_pdf_base*>& components() const { return component_; }

  //: Set the whole pdf mean and variance values.
  // Components and Weights should already be correct so that
  // the error checking can work.
  // #define NDEBUG to turn off error checking.
  void set_mean_and_variance(vnl_vector<double>&m, vnl_vector<double>&v);

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
  vpdfl_pdf_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vpdfl_mixture_h_
