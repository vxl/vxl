#ifndef rrel_estimation_problem_h_
#define rrel_estimation_problem_h_
//:
// \file
// \author Chuck Stewart
// \date March 2001
// \brief Base class for an estimation problem in the robust estimation library.

#include <vcl_vector.h>
#include <vnl/vnl_fwd.h>

class rrel_wls_obj;

//: Base class for (robust) estimation problems.
// A fundamental design decision is that the objective function to be
// minimized is not tied to the class.  This allows different robust
// objective functions to be used with the same data and function
// model.  A disadvantage is that computing the derivative of the
// objective with respect to the data or the model parameters is not
// straightforward.  So far this hasn't proven to be a problem.
//
// A second design decision is that functions are provided for
// indirect access to the data for residual and weight calculation and
// to support random sampling operations. Only a few key functions are
// pure virtual; others will cause an abort (with an error message)
// unless overridden. This allows errors to be detected (albeit at
// run-time) if they are used improperly without requiring subclasses
// to redefine unneeded functions.  For example, if a derived class
// does not implement compute_weights(), then attempting to solve that
// problem with IRLS will cause an abort at runtime.

class rrel_estimation_problem
{
 public:
  //: Type of scale information this problem provides.
  //  NONE: problem does not provide any scale information. SINGLE:
  //  problem provides a single prior scale (homoscedastic
  //  data). MULTIPLE: problem provides a scale for each residual
  //  (heteroscedastic data).
  enum scale_t { NONE = 0, SINGLE = 1, MULTIPLE = 2 };

 public:
  //: Constructor.
  // See the comments for param_dof() and num_samples_to_instantiate()
  // for the meaning of these two parameters.
  rrel_estimation_problem( unsigned int param_dof,
                           unsigned int num_samples_for_fit );

  //: Constructor.
  // Derived classes using this _must_ call set_dof() and set_num_samples_for_fit().
  rrel_estimation_problem( );

  //: Destructor.
  virtual ~rrel_estimation_problem();

  //: The degrees of freedom in the parameter set.
  unsigned int param_dof() const { return dof_; }

  //: Minimum number of samples required to uniquely instantiate a fit.
  //  dof()/num_samples_to_instantiate() gives the number of constraints per sample.
  unsigned int num_samples_to_instantiate( ) const { return num_samples_for_fit_; }

  //: The number of samples.
  virtual unsigned int num_samples( ) const = 0;

  //: The degrees of freedom in the residual.
  //  Most of the time, this would be 1 since the residual comes from
  //  a single random variable. In some problems, however, the error
  //  is the combination of more than one random variable. (For
  //  example, if the residual is a 2d Euclidean distance with
  //  possible error in both coordinates, the degrees of freedom in
  //  the error will be 2.)
  virtual unsigned int residual_dof() const { return 1; }

  //: The number of "unique" samples.
  // In most problems, this equals num_samples. With estimation
  // problems involving non-unique correspondences, however, the total
  // number of possible correspondences (= num_samples()) generally
  // much greater than the number of "unique samples" (which would be
  // the number of points from one image).
  //
  // This is used by random sampling techniques to determine the
  // number of samples required to get a given probability of success.
  virtual unsigned int num_unique_samples( ) const { return num_samples(); }

  //: Generate a parameter vector from a minimal sample set.
  // The \a point_indices vector are indices into the data set, and
  // must be filled in with num_samples_to_instantiate() indices.
  // Returns true if and only if the points resulted in a unique
  // parameter vector.
  virtual bool fit_from_minimal_set( const vcl_vector<int>& /* point_indices */,
                                     vnl_vector<double>& /* params */ ) const = 0;

  //: Compute the residuals relative to the given parameter vector.
  // The number of residuals must be equal to the value returned
  // by num_samples().
  //
  // This is a deterministic procedure, in that multiple calls with a
  // given parameter vector must return the same residuals (in the
  // same order).
  virtual void compute_residuals( const vnl_vector<double>& params,
                                  vcl_vector<double>& residuals ) const = 0;

  //: Compute the weights for the given residuals.
  // The residuals are essentially those returned by
  // compute_residuals(). The default behaviour is to apply obj->wgt()
  // to each residual. Some problems, however, many need to augment
  // the weights. Such problems should override this function (but
  // may want to call this to compute the "basic" weights).
  virtual void compute_weights( const vcl_vector<double>& residuals,
                                const rrel_wls_obj* obj,
                                double scale,
                                vcl_vector<double>& weights ) const;

  //: Type of scale information the problem provides.
  virtual scale_t scale_type() const { return scale_type_; }

  //: The prior scale vector, if available.
  // The call is valid only if scale_type() == MULTIPLE.
  virtual const vcl_vector<double>& prior_multiple_scales() const;

  //: The prior scale, if available.
  // The call is valid only if scale_type() == SINGLE.
  virtual double prior_scale() const;

  //: Sets the scales for heteroscedastic data.
  //  Side effect: set scale_type() = MULTIPLE.
  virtual void set_prior_multiple_scales( const vcl_vector<double>& scales );

  //: Sets the scale for homoscedastic data.
  //  Side effect: set scale_type() = SINGLE.
  virtual void set_prior_scale( double scale );

  //: Removes the scale information for the problem.
  //  Side effect: set scale_type() = NONE.
  virtual void set_no_prior_scale( );

  //: Set similarity weights
  //  Currently it is only used in wgted random sampling search
  void set_similarity_weights( const vcl_vector<double>& wgts )
  { similarity_weights_ = wgts; }
  
  //: Get similarity weights
  const vcl_vector<double>& similarity_weights() const 
  { return similarity_weights_; }

  //: Compute the parameter vector and the normalised covariance matrix.
  //  (Multiplying this matrix by the variance in the measurements
  //  gives the covariance matrix.)
  //
  // If the weights are provided they are used in the process.  Note
  // that if the weights are in fact given, the number of weights
  // (num_wgts) MUST be equal to "num_residuals" returned by the
  // compute_residuals function.  If some of the residuals should be
  // ignored as outliers (e.g. as explicitly identified as such by
  // Least Median of Squares), then their weights should just be set
  // to 0.
  virtual bool weighted_least_squares_fit( vnl_vector<double>& params,
                                           vnl_matrix<double>& norm_covar,
                                           const vcl_vector<double>* weights=0 ) const = 0;

 protected:
  //: Set the degrees of freedom.
  void set_param_dof( unsigned int dof ) { dof_ = dof; }

  //: Set the number of samples needed for a unique fit.
  void set_num_samples_for_fit( unsigned int num_samp ) { num_samples_for_fit_ = num_samp; }

  //: Set the type of prior scale.
  void set_scale_type( scale_t t ) { scale_type_ = t; }

 private:
  unsigned int dof_;
  unsigned int num_samples_for_fit_;
  scale_t scale_type_;
  double single_scale_;
  vcl_vector<double>* multiple_scales_;
  vcl_vector<double>  similarity_weights_;
};

#endif
