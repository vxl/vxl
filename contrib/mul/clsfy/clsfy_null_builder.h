// This is mul/clsfy/clsfy_null_builder.h
// Copyright: (C) 2001 British Telecommunications plc
#ifndef clsfy_null_builder_h_
#define clsfy_null_builder_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Describe a knn classifier builder
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


//: Builds clsfy_null_classifier.
// This class doesn't do any calculation, it is easy to create a
// null classifier directly - but it allows for consistent swappable
// classifiers.
class clsfy_null_builder : public clsfy_builder_base
{
  //: The classification result to always return.
  unsigned default_class_;

 public:
  // Dflt ctor
   clsfy_null_builder(): default_class_(0) {}

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

  //: The number of nearest neighbours to look for.
  unsigned k() const;

  //: Set the number of nearest neighbours to look for.
  void set_k(unsigned);

  //: Initialise the parameters from a text stream.
  void config(std::istream &as);
};

#endif // clsfy_null_builder_h_
