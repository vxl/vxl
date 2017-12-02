// This is mul/clsfy/clsfy_parzen_builder.h
// Copyright: (C) 2001 British Telecommunications plc
#ifndef clsfy_parzen_builder_h_
#define clsfy_parzen_builder_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
#include <vcl_compiler.h>
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
  virtual clsfy_classifier_base* new_classifier() const;

  //: Build classifier from data
  // return the mean error over the training set.
  virtual double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const std::vector<unsigned> &outputs) const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Name of the class
  virtual bool is_class(std::string const& s) const;

  //: IO Version number
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_base* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

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
  void config(std::istream &as);
};


#endif // clsfy_parzen_builder_h_
