// This is mul/clsfy/clsfy_mean_square_1d_builder.h
#ifndef clsfy_mean_square_1d_builder_h_
#define clsfy_mean_square_1d_builder_h_
//:
// \file
// \brief Describe a concrete classifier builder for scalar data
// \author Tim Cootes

#include <string>
#include <iostream>
#include <iosfwd>
#include "clsfy_builder_1d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vbl/vbl_triple.h>
#include <clsfy/clsfy_classifier_1d.h>

//: Base for classes to build clsfy_classifier_1d objects
class clsfy_mean_square_1d_builder : public clsfy_builder_1d
{
 public:

  // Dflt ctor
  clsfy_mean_square_1d_builder();

  // Destructor
  ~clsfy_mean_square_1d_builder() override;

  //: Create empty model
  clsfy_classifier_1d* new_classifier() const override;


  //: Build a binary_threshold classifier
  //  Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the misclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  double build(clsfy_classifier_1d& classifier,
                       const vnl_vector<double>& egs,
                       const vnl_vector<double>& wts,
                       const std::vector<unsigned> &outputs) const override;

  //: Build a mean_square classifier
  // Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the misclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  double build(clsfy_classifier_1d& classifier,
                       vnl_vector<double>& egs0,
                       vnl_vector<double>& wts0,
                       vnl_vector<double>& egs1,
                       vnl_vector<double>& wts1) const override;

  //: Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples,
  //  weighting examples appropriately when estimating the misclassification rate.
  //  data[i] is a triple, {value,class_number,weight}
  //  Returns weighted sum of error.
  //  Note that input "data" must be sorted to use this routine
  double build_from_sorted_data(clsfy_classifier_1d& classifier,
                                        const vbl_triple<double,int,int> *data,
                                        const vnl_vector<double>& wts) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Version number for I/O
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  clsfy_builder_1d* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_mean_square_1d_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_mean_square_1d_builder& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const clsfy_mean_square_1d_builder& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const clsfy_mean_square_1d_builder* b);

#endif // clsfy_mean_square_1d_builder_h_
