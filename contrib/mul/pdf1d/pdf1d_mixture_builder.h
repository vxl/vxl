#ifndef pdf1d_mixture_builder_h_
#define pdf1d_mixture_builder_h_
//:
// \file
// \brief Implements builder for a mixture model PDF.
// \author Tim Cootes and Ian Scott

//=======================================================================

#include <pdf1d/pdf1d_builder.h>
#include <pdf1d/pdf1d_mixture_builder.h>
#include <pdf1d/pdf1d_mixture.h>

//=======================================================================

class pdf1d_mixture;

//: Uses the EM algorithm to build pdf1d_mixture objects.
class pdf1d_mixture_builder : public pdf1d_builder {
private:
  vcl_vector<pdf1d_builder*> builder_;
  double min_var_;
  int max_its_;

	  //: Whether weights changed during iterations
	bool weights_fixed_;

	void initialise(pdf1d_mixture& model,
                  const double* data,
                  const vcl_vector<double>& wts) const;

  //: The Expectation part of the EM algorithm
  void e_step(pdf1d_mixture& model,
              vcl_vector<vnl_vector<double> >& probs,
              const double* data,
              const vcl_vector<double>& wts) const;

  //: The Maximisation part of the EM algorithm
  double m_step(pdf1d_mixture& model,
                const vcl_vector<vnl_vector<double> >& probs,
                const double* data,
                const vcl_vector<double>& wts) const;


  void init();
  void delete_stuff();
public:

  //: Dflt ctor
  pdf1d_mixture_builder();

  //: Copy ctor
  pdf1d_mixture_builder(const pdf1d_mixture_builder&);

  //: Copy operator
  pdf1d_mixture_builder& operator=(const pdf1d_mixture_builder&);

  //: Destructor
  virtual ~pdf1d_mixture_builder();

  //: Initialise n builders of type builder
  //  Clone taken of builder
  void init(pdf1d_builder& builder, int n);

  //: Define maximum number of EM iterations allowed
  void set_max_iterations(int n);

	  //: Whether weights on components left unchanged during iterations
	bool weights_fixed() const { return weights_fixed_; }

	  //: Whether weights on components left unchanged during iterations
	void set_weights_fixed(bool b);

  //: Create empty model
  virtual pdf1d_pdf* new_model() const;

    //: Name of the model class returned by new_model()
  virtual vcl_string new_model_type() const;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Calculate and set the mixture's mean and variance
  // This should be used after the weights or components of
  // the model have been altered.
  static void calc_mean_and_variance(pdf1d_mixture& model);

  //: Build default model with given mean
  virtual void build(pdf1d_pdf& model, double mean) const;

  //: Build model from data
  //  Use EM algorithm to best fit mixture model to given data
  //  If model does not have the correct number of components
  //  they will be initialised to means scattered through the
  //  data.  If they model has the correct number of components
  //  it is assumed that they have been set to sensible initial
  //  values.  The EM algorithm will be used to optimise their
  //  parameters.
  virtual void build(pdf1d_pdf& model,
                     mbl_data_wrapper<double>& data) const;

  //: Build model from weighted data
  //  Use EM algorithm to best fit mixture model to given data
  //  If model does not have the correct number of components
  //  they will be initialised to means scattered through the
  //  data.  If they model has the correct number of components
  //  it is assumed that they have been set to sensible initial
  //  values.  The EM algorithm will be used to optimise their
  //  parameters.
  virtual void weighted_build(pdf1d_pdf& model,
                              mbl_data_wrapper<double>& data,
                              const vcl_vector<double>& wts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // pdf1d_mixture_builder_h_
