#ifndef clsfy_binary_hyperplane_logit_builder_h
#define clsfy_binary_hyperplane_logit_builder_h
//:
// \file
// \brief Linear classifier builder using a logit loss function
// \author Tim Cootes
// \date 18 Jul 2009

#include <clsfy/clsfy_binary_hyperplane_ls_builder.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/vnl_vector.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h> // for std::ostream

//=======================================================================


//: Linear classifier builder using a logit loss function
//  Finds hyperplane and bias to minimise a cost function which minimises
//  the sum of log(pr(error)), ie
//
//  alpha*|w|^2 - sum log[(1-minp)logit(c_i * (b+w.x_i)) + minp]
//
//  Where c_i is +/-1, x_i is the training vector and minp is a lower bound on
//  output probability.
//  This focuses attention on points near boundary.
class clsfy_binary_hyperplane_logit_builder  : public clsfy_binary_hyperplane_ls_builder
{
 private:
  //: Weighting on regularisation term
  double alpha_;

  //: Min prob to be returned by classifier
  double min_p_;
 public:

  // Dflt ctor
  clsfy_binary_hyperplane_logit_builder();

  //: Weighting on regularisation term
  void set_alpha(double a);

  //: Min prob to be returned by classifier
  void set_min_p(double p);

  //: Build a linear classifier, with the given data.
  // Return the mean error over the training set.
  // Builds by minimising clsfy_logit_loss_function() with given inputs,
  // and a regularisation term using clsfy_quad_regulariser
  double build(clsfy_classifier_base &classifier,
               mbl_data_wrapper<vnl_vector<double> > &inputs,
               const vcl_vector<unsigned> &outputs) const;

  //: Build model from data
  // Return the mean error over the training set.
  // For this classifiers, you must nClasses==1 to indicate a binary classifier
  virtual double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
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

  virtual void b_write(vsl_b_ostream &) const;
  virtual void b_read(vsl_b_istream &);
};

#endif // clsfy_binary_hyperplane_logit_builder_h
