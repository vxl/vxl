// This is mul/clsfy/clsfy_parzen_builder.h
// Copyright: (C) 2001 British Telecommunications plc
#ifndef clsfy_parzen_builder_h_
#define clsfy_parzen_builder_h_
//:
// \file
// \brief Describe a Parzen window classifier builder
// \author Ian Scott
// \date 2001-10-07

#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_builder_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>


//: Builds clsfy_rbf_parzen classifiers.
// This class doesn't do any calculation, it is easy to create a
// Parzen window classifier directly - but it allows for consistent swappable
// classifiers.
class clsfy_parzen_builder : public clsfy_builder_base
{
  //: The width of the RBF window function.  Default value is 1.0.
  double sigma_;

  //: The power, p, in the window function.  Default value is 2.0.
  double power_;

 public:
  // Dflt ctor
  clsfy_parzen_builder();

  //: Create empty model
  // Caller is responsible for deletion
  clsfy_classifier_base* new_classifier() const override;

  //: Build classifier from data
  // return the mean error over the training set.
  double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const std::vector<unsigned> &outputs) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: IO Version number
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  clsfy_builder_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: The 1st standard deviation width of the RBF window.
  // The default value is 1.
  double rbf_width() const { return sigma_;}

  //: Set the 1st standard deviation width of the RBF window.
  // The default value is 1.
  void set_rbf_width(double sigma);

  //: The value p in the window function $exp(-1/(2*sigma^p) * |x-y|^p)$.
  // The value p affects the kurtosis, or peakyness of the window. Towards 0 gives a more peaked central spike, and longer tail.
  // Toward +inf gives a broader peak, and shorter tail.
  // The default value is 2, giving a Gaussian distribution.
  double power() const { return power_;}

  //: The value p in the window function $exp(-1/(2*sigma^p) * |x-y|^p)$.
  // The value p affects the kurtosis, or peakyness of the window. Towards 0 gives a more peaked central spike, and longer tail.
  // Toward +inf gives a broader peak, and shorter tail.
  // The default value is 2, giving a Gaussian distribution.
  void set_power(double p);

  //: Initialise the parameters from a text stream.
  void config(std::istream &as) override;
};


#endif // clsfy_parzen_builder_h_
