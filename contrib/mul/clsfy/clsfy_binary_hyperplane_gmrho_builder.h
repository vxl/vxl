#ifndef clsfy_binary_hyperplane_gmrho_builder_h
#define clsfy_binary_hyperplane_gmrho_builder_h

//:
// \file
// \author Martin Roberts
// \brief Builder for linear 2-state classifier, using a sigmoidal Geman-McClure rho function 

#include <vcl_string.h>
#include <vcl_functional.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
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

class clsfy_binary_hyperplane_gmrho_builder  : public clsfy_binary_hyperplane_ls_builder {
private:
    //: The classifier weights (weight N is the constant)
    mutable vnl_vector<double> weights_;

    //: Number of training examples (data.rows())
    mutable unsigned num_examples_;
    //: Number of variables (data.cols())
    mutable unsigned num_vars_;
    //: Tolerance for non-linear optimiser convergence
    mutable double epsilon_;

    //: should sigma be estimate during the build or a pre-defined value used
    bool auto_estimate_sigma_;
    //: use this for sigma if auto_estimate_sigma is true
    double sigma_preset_;

    //: Estimate the scale (sigma) used in the Geman-McClure function
    //This is increased by the mis-classification overlap region if any
    double estimate_sigma(const vnl_matrix<double>& data,
                          const vnl_vector<double >& y) const;
    //: Determine the weights for the hyperplane
    void determine_weights(const vnl_matrix<double>& data,
                           const vnl_vector<double >& y,
                           double sigma) const;
  public:


  // Dflt ctor
  clsfy_binary_hyperplane_gmrho_builder():
      clsfy_binary_hyperplane_ls_builder(),
      num_examples_(0),num_vars_(0),epsilon_(1.0E-8),
      auto_estimate_sigma_(true),sigma_preset_(1.0) {}

  //: Build a linear classifier, with the given data.
  // Return the mean error over the training set.
  double build(clsfy_classifier_base &classifier,
               mbl_data_wrapper<vnl_vector<double> > &inputs,
               const vcl_vector<unsigned> &outputs) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Print class to os
  void print_summary(vcl_ostream& os) const;

  //: Create a deep copy.
  // client is responsible for deleting returned object.
  virtual clsfy_builder_base* clone() const;

  //: should sigma be estimate during the build or a pre-defined value used
  void set_auto_estimate_sigma(bool bAuto) {auto_estimate_sigma_ = bAuto;}
  //: use this for sigma if auto_estimate_sigma is true
  void set_sigma_preset(double sigma_preset) {sigma_preset_ = sigma_preset;}

  virtual void b_write(vsl_b_ostream &) const;
  virtual void b_read(vsl_b_istream &);
};
//: Some helper stuff, like the error function to be minimised
namespace clsfy_binary_hyperplane_gmrho_builder_helpers
{
    //: The cost function, sum Geman-McClure error functions over all training examples
    class gmrho_sum : public vnl_cost_function
    {
        //: Reference to data matrix, one row per training example
        const vnl_matrix<double>& x_;
        const vnl_vector<double>& y_;
        double sigma_;
        double var_;
        unsigned num_examples_;
        unsigned num_vars_;
        double alpha_;
        double beta_;
     public:
        //: construct passing in reference to data matrix
        gmrho_sum(const vnl_matrix<double>& x,
                  const vnl_vector<double>& y,double sigma=1);
        
        //: reset the scaling factor
        void set_sigma(double sigma) {sigma_ = sigma; var_ = sigma*sigma;}

        //:  The main function.  Given the vector of weights parameters vector , compute the value of f(x).
        virtual double f(vnl_vector<double> const& w);

        //:  Calculate the gradient of f at parameter vector x.
        virtual void gradf(vnl_vector<double> const& x, vnl_vector<double>& gradient);
        
    };

    //: functor to accumulate gradient contributions for given training example
    class gm_grad_accum
    {
        const double* px_;
        const double wt_;
      public:
        gm_grad_accum(const double* px,double wt) : px_(px),wt_(wt) {}
        void operator()(double& grad)
        {
            grad += (*px_++) * wt_;
        }
    };

    //: Given the class category variable, return the associated regression value (e.g. 1 for class 1, -1 for class 0)
    class category_value
    {
        const double y0;
        const double y1;
    public:
        category_value(unsigned num_category1,unsigned num_total):
                y0(-1.0*double(num_total-num_category1)/double(num_total)),
                y1(double(num_category1)/double(num_total)) {}
            
        double operator()(const unsigned& classNum)
        {
            //return classNum ? y1 : y0;
            return classNum ? 1.0 : -1.0;
        }
    };
};


#endif // clsfy_binary_hyperplane_gmrho_builder.h_h






