#ifndef clsfy_smo_base_h_
#define clsfy_smo_base_h_

//:
// \file
// \author Ian Scott
// \date 26-Nov-2001
// \brief Sequential Minimum Optimisation algorithm

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <mbl/mbl_data_wrapper.h>

// Edit these definitions to suit your taste, but
// don't commit the changes.

#define CLSFY_SMO_BASE_PRINT_PROGRESS   0

//: A base class for sequential minimal optimisation.
// This finds the optimal point on a quadratic function constrained by
// inequality bounds on each parameter, and a single equality constraint.
// The is the class of problems associated with Support Vector Machines
class clsfy_smo_base
{
 protected:

  //: Error rate on the training data
  double error_;

  //: An iterator on the data;
  mbl_data_wrapper<vnl_vector<double> > *data_;

  //: The parameters to be optimised
  vnl_vector<double> alph_;

  //: Amount by which a sample can violate the KKT conditions
  double tolerance_;

  //: Tolerance on several equalities.
  // Including testing if a Lagrange multiplier is at one of the bounds.
  double eps_;

  //: Bias
  // \invariant a_i unbound and optimal => KKT_i holds
  double b_;

  //: Cache KKT error values for unbound multipliers.
  // \invariant a_i unbound => E_i = f(x_i) - y1 
  vcl_vector<double> error_cache_;

  //: Target values y_i
  vcl_vector<int> target_;

  //: The norm of each training vector is useful to know quickly
  vcl_vector<double> precomputed_self_dot_product_;

  vnl_random rng_;

  //: Attempt to jointly optimise Lagrange multipliers i1, and i2.
  // \param i1 first Lagrange multiplier.
  // \param i2 second Lagrange multiplier.
  // \param E1 The amount by which i1 violates KKT conditions.
  virtual int take_step(int i1, int i2, double E1) =0;

  //: Attempt to optimise sample i1.
  // This attempts to find another value i2,
  // in order to jointly optimise both.
  virtual int examine_example(int i1) =0;

  //: Access the data points
  const vnl_vector<double> & data_point(unsigned long l);

  //: Calculate the kernel for data items i1 and i2;
  virtual double kernel(int i1, int i2) =0;

  //: Calculate the classifier function learnt so far for data item k.
  virtual double learned_func(int k) ;

 public:

  //: Get the optimised parameters
  const vnl_vector<double>& lagrange_mults() const;

  //: Set the initial values of the parameters to be optimised.
  // The caller is responsible for ensuring that the initial values
  // fulfill the constraints;
  void set_lagrange_mults(const vnl_vector<double>& lagrange_mults);

  //: Bias term in function evaluation.
  // For SVMs this would be the value to be subtracted from sum of kernel
  // functions to get 0 as class boundary.
  double bias();

  //: Reseeds the internal random number generator.
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_ctime.h>
  // ..
  // sampler.reseed(vcl_time(0));
  // \endcode
  virtual void reseed(unsigned long seed);

  //: amount by which a sample can violate the KKT conditions
  const double& tolerance() const;

  //: Set the amount by which a sample can violate the KKT conditions.
  // Default value is 0.001
  void set_tolerance(double tolerance);

  //: tolerance on several equalities.
  // Including testing if a Lagrange multiplier is at one of the bounds.
  double eps() const;

  //: Set the tolerance on several equalities.
  // Including testing if a Lagrange multiplier is at one of the bounds.
  // Default value is 0.001
  void set_eps(double eps);

  clsfy_smo_base();
  virtual ~clsfy_smo_base();

  virtual double error_rate();

  //: Run the optimisation
  virtual int calc()=0;

  //: error rate on the training data
  double error();
};

#endif
