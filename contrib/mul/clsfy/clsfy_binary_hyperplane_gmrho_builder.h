#ifndef clsfy_binary_hyperplane_gmrho_builder_h
#define clsfy_binary_hyperplane_gmrho_builder_h
//:
// \file
// \author Martin Roberts
// \brief Builder for linear 2-state classifier, using a sigmoidal Geman-McClure rho function

#include <string>
#include <iosfwd>
#include <iostream>
#include <functional>
// not used? #include <vcl_compiler.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <clsfy/clsfy_binary_hyperplane_ls_builder.h>

//=======================================================================


//: Builder for linear 2-state classifier
//  Uses a Geman-McClure robust function, rather than a least squares fit, on points that
//  are not mis-classified
//  This increases the weighting given to points near the boundary in determining its fit
//  A conventional least squares fit is perform first to determine a starting solution,
//  and also the sigma scaling factor used in the GM function.
//  Several iterations are performed during which sigma is reduced
// (i.e. deterministic annealing), to try and avoid local minima

class clsfy_binary_hyperplane_gmrho_builder  : public clsfy_binary_hyperplane_ls_builder
{
 private:
  //: The classifier weights (weight N is the constant)
  mutable vnl_vector<double> weights_;

  //: Number of training examples (data.rows())
  mutable unsigned num_examples_{0};
  //: Number of variables (data.cols())
  mutable unsigned num_vars_{0};
  //: Tolerance for non-linear optimiser convergence
  mutable double epsilon_{1.0E-8};

  //: should sigma be estimated during the build or a pre-defined value used
  bool auto_estimate_sigma_{true};
  //: use this for sigma if auto_estimate_sigma is true
  double sigma_preset_{1.0};

  //: Estimate the scale (sigma) used in the Geman-McClure function
  //This is increased by the mis-classification overlap region if any
  double estimate_sigma(const vnl_matrix<double>& data,
                        const vnl_vector<double>& y) const;
  //: Determine the weights for the hyperplane
  void determine_weights(const vnl_matrix<double>& data,
                         const vnl_vector<double>& y,
                         double sigma) const;
 public:

  // Dflt ctor
   clsfy_binary_hyperplane_gmrho_builder()
       : clsfy_binary_hyperplane_ls_builder() {}

   //: Build a linear classifier, with the given data.
   // Return the mean error over the training set.
   double build(clsfy_classifier_base &classifier,
                mbl_data_wrapper<vnl_vector<double>> &inputs,
                const std::vector<unsigned> &outputs) const;

   //: Build model from data
   // Return the mean error over the training set.
   // For this classifiers, you must nClasses==1 to
   // indicate a binary classifier
   double build(clsfy_classifier_base &model,
                mbl_data_wrapper<vnl_vector<double>> &inputs, unsigned nClasses,
                const std::vector<unsigned> &outputs) const override;

   //: Version number for I/O
   short version_no() const;

   //: Name of the class
   std::string is_a() const override;

   //: Name of the class
   bool is_class(std::string const &s) const override;

   //: Print class to os
   void print_summary(std::ostream &os) const override;

   //: Create a deep copy.
   // client is responsible for deleting returned object.
   clsfy_builder_base *clone() const override;

   //: should sigma be estimate during the build or a pre-defined value used
   void set_auto_estimate_sigma(bool bAuto) { auto_estimate_sigma_ = bAuto; }
   //: use this for sigma if auto_estimate_sigma is true
   void set_sigma_preset(double sigma_preset) { sigma_preset_ = sigma_preset; }

   void b_write(vsl_b_ostream &) const override;
   void b_read(vsl_b_istream &) override;
};

#endif // clsfy_binary_hyperplane_gmrho_builder_h
