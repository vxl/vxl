// This is mul/clsfy/clsfy_binary_hyperplane_ls_builder.h
// Copyright: (C) 2000 British Telecommunications PLC
#ifndef clsfy_binary_hyperplane_ls_builder_h_
#define clsfy_binary_hyperplane_ls_builder_h_
//:
// \file
// \brief Describe a binary linear classifier builder
// \author Ian Scott
// \date 2000/05/26

#include <clsfy/clsfy_binary_hyperplane.h>
#include <clsfy/clsfy_builder_base.h>
#include <vcl_iosfwd.h>

//: Build a binary linear classifier using least squares
class clsfy_binary_hyperplane_ls_builder : public clsfy_builder_base
{
 public:
  // Dflt ctor
  clsfy_binary_hyperplane_ls_builder() {}

  //: Create a new untrained linear classifier with binary output
  virtual clsfy_classifier_base* new_classifier() const
  { return new clsfy_binary_hyperplane; }

  //: Build a linear classifier, with the given data.
  // Return the mean error over the training set.
  double build(clsfy_classifier_base &classifier,
               mbl_data_wrapper<vnl_vector<double> > &inputs,
               const vcl_vector<unsigned> &outputs) const;

  //: Build a linear classifier, with the given data.
  // Return the mean error over the training set.
  // n_classes must be 1
  double build(clsfy_classifier_base &classifier,
               mbl_data_wrapper<vnl_vector<double> > &inputs,
               unsigned n_classes, const vcl_vector<unsigned> &outputs) const;

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
  virtual clsfy_builder_base* clone() const
  { return new clsfy_binary_hyperplane_ls_builder(*this); }

  virtual void b_write(vsl_b_ostream &) const;
  virtual void b_read(vsl_b_istream &);
};

#endif // clsfy_binary_hyperplane_ls_builder_h_
