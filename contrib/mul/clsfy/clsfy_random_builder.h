// This is mul/clsfy/clsfy_random_builder.h
#ifndef clsfy_random_builder_h_
#define clsfy_random_builder_h_

// Copyright: (C) 2001 Britsh Telecommunications plc

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Describe a random classifier builder
// \author Ian Scott
// \date 2001/10/07

//=======================================================================

#include "clsfy/clsfy_builder_base.h"
#include <vcl_vector.h>
#include <vcl_string.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>


//: Builds clsfy_random_classifier classifiers.
// This class doesn't do any calculation, it is easy to create a
// random classfier directly - but it allows for consistent swappable
// classifiers.
class clsfy_random_builder : public clsfy_builder_base
{
  //: Standard deviation of confidence noise added to class probabilities
  double confidence_; 

 public:
  // Dflt ctor
  clsfy_random_builder();

  //: Create empty model
  // Caller is responsible for deletion
  virtual clsfy_classifier_base* new_classifier() const;

  //: Build classifier from data
  // return the mean error over the training set.
  virtual double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const vcl_vector<unsigned> &outputs) const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: IO Version number
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: The standard deviation of confidence noise added to class probabilities.
  double confidence() const;

  //: Set the standard deviation of confidence noise added to class probabilities.
  void set_confidence(double);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_random_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_random_builder& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_random_builder& b);

#endif // clsfy_random_builder_h_
