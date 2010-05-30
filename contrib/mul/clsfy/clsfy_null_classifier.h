// This is mul/clsfy/clsfy_null_classifier.h
#ifndef clsfy_null_classifier_h_
#define clsfy_null_classifier_h_
// Copyright: (C) 2001 British Telecommunications plc
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Describe a null classifier
// \author Ian Scott
// \date 2009-08-25

#include "clsfy_classifier_base.h"
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vcl_iosfwd.h>

//:  A classifier that always returns the same answer.
class clsfy_null_classifier : public clsfy_classifier_base
{
 public:
  // Dflt constructor
  clsfy_null_classifier(): n_dims_(0u), default_class_(0u) {}

  //: Constructor for when you need a different default
  clsfy_null_classifier(unsigned dc): n_dims_(0u), default_class_(dc) {}

  //: Return the probability the input being in each class.
  virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

  //: Log likelihood of being in class
  virtual double log_l(const vnl_vector<double> &input) const;

  //: The number of possible output classes.
  virtual unsigned n_classes() const;

  //: The dimensionality of input vectors.
  virtual unsigned n_dims() const
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
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_classifier_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: The probabilities of returning a value in each class.
  const vcl_vector<double> & probs() const;

  //: Set the prior probabilities of each class
  // The values are normalised to sum to 1.
  void set_probs(const vcl_vector<double> &);


 private:

  //: The classifier may get asked this
  unsigned n_dims_;

  //: The class value that is always returned, whatever the input vector.
  unsigned default_class_;
};

#endif // clsfy_null_classifier_h_
