// This is mul/clsfy/clsfy_binary_hyperplane_ls_builder.h
// Copyright: (C) 2000 British Telecommunications PLC
#ifndef clsfy_binary_hyperplane_ls_builder_h_
#define clsfy_binary_hyperplane_ls_builder_h_
//:
// \file
// \brief Describe a binary linear classifier builder
// \author Ian Scott
// \date 2000-05-26

#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_binary_hyperplane.h>
#include <clsfy/clsfy_builder_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Build a binary linear classifier using least squares
class clsfy_binary_hyperplane_ls_builder : public clsfy_builder_base
{
 public:
  // Dflt ctor
  clsfy_binary_hyperplane_ls_builder() = default;

  //: Create a new untrained linear classifier with binary output
  clsfy_classifier_base* new_classifier() const override
  { return new clsfy_binary_hyperplane; }

  //: Build a linear classifier, with the given data.
  // Return the mean error over the training set.
  double build(clsfy_classifier_base &classifier,
               mbl_data_wrapper<vnl_vector<double> > &inputs,
               const std::vector<unsigned> &outputs) const;

  //: Build a linear classifier, with the given data.
  // Return the mean error over the training set.
  // n_classes must be 1
  double build(clsfy_classifier_base &classifier,
               mbl_data_wrapper<vnl_vector<double> > &inputs,
               unsigned n_classes, const std::vector<unsigned> &outputs) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Create a deep copy.
  // client is responsible for deleting returned object.
  clsfy_builder_base* clone() const override
  { return new clsfy_binary_hyperplane_ls_builder(*this); }

  void b_write(vsl_b_ostream &) const override;
  void b_read(vsl_b_istream &) override;
};

#endif // clsfy_binary_hyperplane_ls_builder_h_
