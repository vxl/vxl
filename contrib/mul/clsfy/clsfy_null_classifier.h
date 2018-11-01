// This is mul/clsfy/clsfy_null_classifier.h
#ifndef clsfy_null_classifier_h_
#define clsfy_null_classifier_h_
// Copyright: (C) 2001 British Telecommunications plc
//:
// \file
// \brief Describe a null classifier
// \author Ian Scott
// \date 2009-08-25

#include <iostream>
#include <iosfwd>
#include "clsfy_classifier_base.h"
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:  A classifier that always returns the same answer.
class clsfy_null_classifier : public clsfy_classifier_base
{
 public:
  // Dflt constructor
  clsfy_null_classifier(): n_dims_(0u), default_class_(0u) {}

  //: Constructor for when you need a different default
  clsfy_null_classifier(unsigned dc): n_dims_(0u), default_class_(dc) {}

  //: Return the probability the input being in each class.
  void class_probabilities(std::vector<double> &outputs, const vnl_vector<double> &input) const override;

  //: Log likelihood of being in class
  double log_l(const vnl_vector<double> &input) const override;

  //: The number of possible output classes.
  unsigned n_classes() const override;

  //: The dimensionality of input vectors.
  unsigned n_dims() const override
  { return n_dims_; }

  //: Set the number of dimensions the classifier reports that it uses.
  // The classifier itself pays no attention to this value, but it
  // may be useful for other error checking code which calls n_dims()
  void set_n_dims(unsigned n)
  { n_dims_ = n; }

  //: The classification result that is always returned.
  unsigned default_class() const
  { return default_class_; }

  //: Set the number of dimensions the classifier reports that it uses.
  // The classifier itself pays no attention to this value, but it
  // may be useful for other error checking code which calls n_dims()
  void set_default_class(unsigned dc)
  { default_class_ = dc; }

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  clsfy_classifier_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: The probabilities of returning a value in each class.
  const std::vector<double> & probs() const;

  //: Set the prior probabilities of each class
  // The values are normalised to sum to 1.
  void set_probs(const std::vector<double> &);


 private:

  //: The classifier may get asked this
  unsigned n_dims_;

  //: The class value that is always returned, whatever the input vector.
  unsigned default_class_;
};

#endif // clsfy_null_classifier_h_
