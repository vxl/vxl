#ifndef vpdfl_mixture_h_
#define vpdfl_mixture_h_
//=======================================================================
//
//  Copyright: (C) 2000 Victoria University of Manchester
//
//=======================================================================

//:
// \file
// \brief Implements a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes
// \date 21-July-98
//
// Modifications
// \verbatim
//    IMS   Converted to VXL 12 May 2000
// \endverbatim

//=======================================================================

#include <vpdfl/vpdfl_pdf_base.h>
#include <vcl_vector.h>

//=======================================================================

//: Represents a mixture model (a set of individual pdfs + weights)
class vpdfl_mixture : public vpdfl_pdf_base
{
  vcl_vector<vpdfl_pdf_base*> component_;
  vcl_vector<double>          weight_;

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
  virtual ~vpdfl_mixture();

  //: Probability density at x
  virtual double operator()(const vnl_vector<double>& x) const;

  //: Log of probability density at x
  virtual double log_p(const vnl_vector<double>& x) const;

  //: Gradient of PDF at x
  virtual void gradient(vnl_vector<double>& g,
                        const vnl_vector<double>& x, double& p) const;

  //: Not Yet Implemented
  // Compute nearest point to x which has a density above a threshold
  //  If log_p(x)>log_p_min then x unchanged.  Otherwise x is moved
  //  (typically up the gradient) until log_p(x)>=log_p_min.
  // \param x This may be modified to the nearest plausible position.
  // \param log_p_min lower threshold for log_p(x)
  virtual void nearest_plausible(vnl_vector<double>& x, double log_p_min) const;

  //: Initialise to use n components of type comp_type
  //  Clones taken by comp_type
  void init(const vpdfl_pdf_base& comp_type, unsigned n);

  //: Return instance object for this PDF
  //  Object is created on heap. Caller responsible for deletion.
  virtual vpdfl_sampler_base * new_sampler() const;

  //: Number of components in mixture
  unsigned n_components() const { return component_.size(); }

  //: Get i<I>th</I> weight.
  double weight(unsigned i) { return weight_[i]; }

  //: Array of weights
  // Use weight(i) where possible
  const vcl_vector<double>& weights() const { return weight_; }

  //: Array of weights
  // Warning care must be taken to ensure consistency when modifying weights
  // Warning. Use weight(i) where possible
  vcl_vector<double>& weights() { return weight_; }

  //: Return index of component nearest to x
  unsigned nearest_comp(const vnl_vector<double>& x) const;

  //: Set the contents of the mixture model.
  // Clones are taken of all the data, and the class will be responsible for their deletion.
  void set(const vcl_vector<vpdfl_pdf_base*> components, const vcl_vector<double> & weights);

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
  vcl_vector<vpdfl_pdf_base*>& components() { return component_; }

  //: Access to components - for use by builders
  // Use component(i) where possible
  const vcl_vector<vpdfl_pdf_base*>& components() const { return component_; }

  //: Set the whole pdf mean and variance values.
  // Components and Weights should already be correct so that
  // the error checking can work.
  // #define NDEBUG to turn off error checking.
  void set_mean_and_variance(vnl_vector<double>&m, vnl_vector<double>&v);

  //: Return true if the object represents a valid PDF.
  // This will return false, if n_dims() is 0, for example just ofter
  // default construction.
  virtual bool is_valid_pdf() const;

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

#endif // vpdfl_mixture_h_
