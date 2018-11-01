// This is mul/clsfy/clsfy_random_builder.h
// Copyright: (C) 2001 British Telecommunications plc
#ifndef clsfy_random_builder_h_
#define clsfy_random_builder_h_
//:
// \file
// \brief Describe a random classifier builder
// \author Ian Scott
// \date 2001-10-07

//=======================================================================

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


//: Builds clsfy_random_classifier classifiers.
// This class doesn't do any calculation, it is easy to create a
// random classifier directly - but it allows for consistent swappable
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
std::ostream& operator<<(std::ostream& os,const clsfy_random_builder& b);

#endif // clsfy_random_builder_h_
