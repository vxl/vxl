// This is mul/vpdfl/vpdfl_mixture_builder.h
// Copyright: (C) 2000 Victoria University of Manchester
#ifndef vpdfl_mixture_builder_h_
#define vpdfl_mixture_builder_h_
//:
// \file
// \brief Implements builder for a mixture model PDF.
// \author Tim Cootes
// \date 21-July-98
//
// \verbatim
//  Modifications
//   IMS   Converted to VXL 14 May 2000, with redesign
// \endverbatim

//=======================================================================

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_builder_base.h>
#include <vpdfl/vpdfl_mixture_builder.h>
#include <vpdfl/vpdfl_mixture.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

class vpdfl_mixture;

//: Uses the EM algorithm to build vpdfl_mixture objects.
class vpdfl_mixture_builder : public vpdfl_builder_base
{
  std::vector<vpdfl_builder_base*> builder_;
  double min_var_;
  int max_its_;
  std::vector<vnl_vector<double> > initial_means_;

  //: Whether weights changed during iterations
  bool weights_fixed_;

    //: Assumes means set up.  Estimates starting components.
    // \param mean_sep : Rough guess of mean separation between points
  void initialise_given_means(vpdfl_mixture& model,
                              const vnl_vector<double>* data,
                              const std::vector<vnl_vector<double> >& mean,
                              const std::vector<double>& wts) const;

    //: Means centred on data[i*f]
  void initialise_to_regular_samples(vpdfl_mixture& model,
                                     const vnl_vector<double>* data,
                                     const std::vector<double>& wts) const;

    //: Select positions along diagonal of bounding box
  void initialise_diagonal(vpdfl_mixture& model,
                           const vnl_vector<double>* data,
                           const std::vector<double>& wts) const;

    //: Select positions and widths for components to start
  void initialise(vpdfl_mixture& model,
                  const vnl_vector<double>* data,
                  const std::vector<double>& wts) const;

  //: The Expectation part of the EM algorithm
  void e_step(vpdfl_mixture& model,
              std::vector<vnl_vector<double> >& probs,
              const vnl_vector<double>* data,
              const std::vector<double>& wts) const;

  //: The Maximisation part of the EM algorithm
  double m_step(vpdfl_mixture& model,
                const std::vector<vnl_vector<double> >& probs,
                const vnl_vector<double>* data,
                const std::vector<double>& wts) const;

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
  ~vpdfl_mixture_builder() override;

  //: Initialise n builders of type builder
  //  Clone taken of builder
  void init(const vpdfl_builder_base& builder, int n);

  //: Whether weights on components left unchanged during iterations
  bool weights_fixed() const { return weights_fixed_; }

  //: Whether weights on components left unchanged during iterations
  void set_weights_fixed(bool b);

  //: Define maximum number of EM iterations allowed
  void set_max_iterations(int n);

  //: Define maximum number of EM iterations allowed
  int max_iterations() const { return max_its_; }

  //: Number of basis builders
  unsigned n_builders() const { return builder_.size(); }

  //: Return i-th builder
  vpdfl_builder_base& builder(unsigned i) { return *builder_[i]; }

  //: Create empty model
  vpdfl_pdf_base* new_model() const override;

  //: Define lower threshold on variance for built models
  void set_min_var(double min_var) override;

  //: Get lower threshold on variance for built models
  double min_var() const override;

  //: Calculate and set the mixture's mean and variance
  // This should be used after the weights or components of
  // the model have been altered.
  static void calc_mean_and_variance(vpdfl_mixture& model);

  //: Build default model with given mean
  void build(vpdfl_pdf_base& model,
                     const vnl_vector<double>& mean) const override;

  //: Build model from data
  //  Use EM algorithm to best fit mixture model to given data
  //  If model does not have the correct number of components
  //  they will be initialised to means scattered through the
  //  data.  If they model has the correct number of components
  //  it is assumed that they have been set to sensible initial
  //  values.  The EM algorithm will be used to optimise their
  //  parameters.
  void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const override;

  //: Build model from weighted data
  //  Use EM algorithm to best fit mixture model to given data
  //  If model does not have the correct number of components
  //  they will be initialised to means scattered through the
  //  data.  If they model has the correct number of components
  //  it is assumed that they have been set to sensible initial
  //  values.  The EM algorithm will be used to optimise their
  //  parameters.
  void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const std::vector<double>& wts) const override;

  //: Preset initial component means
  // When initialise is called these means will be used as the initial guess
  // Note that the vector must  is copied
  void preset_initial_means(const std::vector<vnl_vector<double> >& component_means);


  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_builder_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Read initialisation settings from a stream.
  // Parameters:
  // \verbatim
  // {
  //   min_var: 1.0e-6
  //   // Number of pdf bases to use
  //   n_pdfs: 3
  //   // Type of basis pdf
  //   basis_pdf: axis_gaussian { min_var: 0.0001 }
  //   // Maximum number of iterations
  //   max_its: 10
  //   // When true, fix weights during EM
  //   weights_fixed: false
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  void config_from_stream(std::istream & is) override;
};

#endif // vpdfl_mixture_builder_h_
