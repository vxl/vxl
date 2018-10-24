#ifndef clsfy_smo_1_h_
#define clsfy_smo_1_h_

//:
// \file
// \author Ian Scott
// \date 26-Nov-2001
// \brief Sequential Minimum Optimisation algorithm

#include <clsfy/clsfy_smo_base.h>

//: A sequential minimal optimisation for certain restricted quadratic problems.
// This finds the optimal point on a quadratic function constrained by
// inequality bounds on each parameter, and a single equality constraint.
// The is the class of problems associated with Support Vector Machines
// Uses linear kernel operator.
class clsfy_smo_1_lin: public clsfy_smo_base
{
 protected:

  //: upper bound on Lagrange multipliers
  double C_;

  //: Attempt to jointly optimise Lagrange multipliers i1, and i2.
  // \param i1 first Lagrange multiplier.
  // \param i2 second Lagrange multiplier.
  // \param E1 The amount by which i1 violates KKT conditions.
  int take_step(int i1, int i2, double E1) override;

  //: Attempt to optimise sample i1.
  // This attempts to find another value i2,
  // in order to jointly optimise both.
  int examine_example(int i1) override;

  //: Calculate the kernel for data items i1 and i2.
  double kernel(int i1, int i2) override;

 public:

  //: Takes a copy of the data wrapper, but not the data.
  // Be careful not to destroy the underlying data while using this object.
  void set_data(const mbl_data_wrapper<vnl_vector<double> >& data, const std::vector<int>& targets);

  // Upper bounds on each parameter.
  double C() const;

  //: Set the upper bound on the Lagrange multipliers
  void set_C( double C);

  clsfy_smo_1_lin();

  //: Run the optimisation
  int calc() override;
};

//: A sequential minimal optimisation for certain restricted quadratic problems.
// This finds the optimal point on a quadratic function constrained by
// inequality bounds on each parameter, and a single equality constraint.
// The is the class of problems associated with Support Vector Machines.
// Uses RBF kernel operator.
class clsfy_smo_1_rbf: public clsfy_smo_1_lin
{
 protected:

  //: -0.5 sigma^-2, where sigma is the width of the Gaussian kernel
  double gamma_;

  //: Calculate the kernel for data items i1 and i2
  double kernel(int i1, int i2) override;

 public:

  //: -0.5 sigma^-2, where sigma is the width of the Gaussian kernel
  double gamma() const;

  //: Control sigma, the width of the Gaussian kernel.
  // gamma = -0.5 sigma^-2
  void set_gamma(double gamma);

  clsfy_smo_1_rbf();

  //: Run the optimisation
  int calc() override;
};

#endif // clsfy_smo_1_h_
