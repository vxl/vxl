#ifndef vpdfl_mixture_builder_h_
#define vpdfl_mixture_builder_h_
//=======================================================================
//
//      Copyright: (C) 2000 Victoria University of Manchester
//
//=======================================================================

//:
// \file
// \author Tim Cootes
// \date 21-July-98
// \brief Implements builder for a mixture model PDF.
// \verbatim
//    IMS   Converted to VXL 14 May 2000, with redesign
// \endverbatim

//=======================================================================

#include <vpdfl/vpdfl_builder_base.h>
#include <vpdfl/vpdfl_mixture_builder.h>
#include <vpdfl/vpdfl_mixture.h>

//=======================================================================

class vpdfl_mixture;

//: Base for classes to build vpdfl_mixture objects.
class vpdfl_mixture_builder : public vpdfl_builder_base {
private:
  vcl_vector<vpdfl_builder_base*> builder_;
  double min_var_;
  int max_its_;

  void initialise(vpdfl_mixture& model,
                  const vnl_vector<double>* data,
                  const vcl_vector<double>& wts) const;

  void e_step(vpdfl_mixture& model,
              vcl_vector<vnl_vector<double> >& probs,
              const vnl_vector<double>* data,
              const vcl_vector<double>& wts) const;

  double m_step(vpdfl_mixture& model,
                const vcl_vector<vnl_vector<double> >& probs,
                const vnl_vector<double>* data,
                const vcl_vector<double>& wts) const;

  void init();
  void delete_stuff();
public:

  //: Dflt ctor
  vpdfl_mixture_builder();

  //: Copy ctor
  vpdfl_mixture_builder(const vpdfl_mixture_builder&);

  //: Copy operator
  vpdfl_mixture_builder& operator=(const vpdfl_mixture_builder&);

  //: Destructor
  virtual ~vpdfl_mixture_builder();

  //: Initialise n builders of type builder
  //  Clone taken of builder
  void init(vpdfl_builder_base& builder, int n);

  //: Define maximum number of EM iterations allowed
  void set_max_iterations(int n);

  //: Create empty model
  virtual vpdfl_pdf_base* new_model() const;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Build default model with given mean
  virtual void build(vpdfl_pdf_base& model,
                     const vnl_vector<double>& mean) const;

  //: Build model from data
  //  Use EM algorithm to best fit mixture model to given data
  //  If model does not have the correct number of components
  //  they will be initialised to means scattered through the
  //  data.  If they model has the correct number of components
  //  it is assumed that they have been set to sensible initial
  //  values.  The EM algorithm will be used to optimise their
  //  parameters.
  virtual void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const;

  //: Build model from weighted data
  //  Use EM algorithm to best fit mixture model to given data
  //  If model does not have the correct number of components
  //  they will be initialised to means scattered through the
  //  data.  If they model has the correct number of components
  //  it is assumed that they have been set to sensible initial
  //  values.  The EM algorithm will be used to optimise their
  //  parameters.
  virtual void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const vcl_vector<double>& wts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_builder_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  //!in: bfs: Target binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  //!out: bfs: Target binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // vpdfl_mixture_builder_h_
