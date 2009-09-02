// This is mul/clsfy/clsfy_binary_1d_wrapper_builder.h
// Copyright: (C) 2000 British Telecommunications PLC
#ifndef clsfy_binary_1d_wrapper_builder_h_
#define clsfy_binary_1d_wrapper_builder_h_
//:
// \file
// \brief Wrap a builder_1d in general builder_base derivative.
// \author Ian Scott
// \date 2 Sep 2009

#include <vcl_iosfwd.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <clsfy/clsfy_binary_1d_wrapper.h>
#include <clsfy/clsfy_builder_1d.h>
#include <clsfy/clsfy_builder_base.h>

//: Wrap a builder_1d in general builder_base derivative.
class clsfy_binary_1d_wrapper_builder : public clsfy_builder_base
{
  mbl_cloneable_nzptr<clsfy_builder_1d> builder_1d_;

 public:
  // Dflt ctor
  clsfy_binary_1d_wrapper_builder();

  //: Set the underlying builder.
  // The object will take and maintain its own deep copy of the data.
  // This method should only be used by builders.
  void set_builder_1d(const clsfy_builder_1d &builder)
  { builder_1d_ = builder; }

  //: Get the underlying builder.
  const clsfy_builder_1d &builder_1d() const
  { return *builder_1d_; }

  //: Create a new untrained linear classifier with binary output
  virtual clsfy_classifier_base* new_classifier() const;

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


  //: Name of the class
  vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Print class to os
  void print_summary(vcl_ostream& os) const;

  //: Create a deep copy.
  // client is responsible for deleting returned object.
  virtual clsfy_builder_base* clone() const
  { return new clsfy_binary_1d_wrapper_builder(*this); }

  virtual void b_write(vsl_b_ostream &) const;
  virtual void b_read(vsl_b_istream &);

  //: Initialise the parameters from a text stream.
  void config(vcl_istream &as);

};

#endif // clsfy_binary_1d_wrapper_builder_h_
