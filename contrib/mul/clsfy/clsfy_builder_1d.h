// This is mul/clsfy/clsfy_builder_1d.h
#ifndef clsfy_builder_1d_h_
#define clsfy_builder_1d_h_
//:
// \file
// \brief Describe an abstract classifier builder for scalar data
// \author Tim Cootes

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_triple.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_vector.h>

class clsfy_classifier_1d;

//: Base for classes to build clsfy_classifier_1d objects
class clsfy_builder_1d
{
 public:

  // Destructor
  virtual ~clsfy_builder_1d() = default;

  //: Create empty model
  virtual clsfy_classifier_1d* new_classifier() const = 0;


  //: Build a binary_threshold classifier
  //  Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the misclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  virtual double build(clsfy_classifier_1d& classifier,
                       const vnl_vector<double>& egs,
                       const vnl_vector<double>& wts,
                       const std::vector<unsigned> &outputs) const = 0;


  //: Build a binary_threshold classifier
  //  Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the misclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  virtual double build(clsfy_classifier_1d& classifier,
                       vnl_vector<double>& egs0,
                       vnl_vector<double>& wts0,
                       vnl_vector<double>& egs1,
                       vnl_vector<double>& wts1) const = 0;

  //: Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples,
  //  weighting examples appropriately when estimating the misclassification rate.
  //  data[i] is a triple, {value,class_number,weight}
  //  Returns weighted sum of error.
  //  Note that input "data" must be sorted to use this routine
  virtual double build_from_sorted_data(clsfy_classifier_1d& classifier,
                                        const vbl_triple<double,int,int> *data,
                                        const vnl_vector<double>& wts) const = 0;

  //: Name of the class
  virtual std::string is_a() const;

  //: Name of the class
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_1d* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream

  virtual void b_read(vsl_b_istream& bfs) = 0;
  //: Initialise the parameters from a text stream.
  // Default case accepts no parameters.
  virtual void config(std::istream &as);

  //: Load description from a text stream
  static std::unique_ptr<clsfy_builder_1d> new_builder(
    std::istream &as);
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_builder_1d& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_builder_1d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_builder_1d& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const clsfy_builder_1d& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const clsfy_builder_1d* b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const clsfy_builder_1d* b);

//! Stream output operator for class reference
inline void vsl_print_summary(std::ostream& os, const clsfy_builder_1d& b)
{ os << b; }

//! Stream output operator for class pointer
inline void vsl_print_summary(std::ostream& os, const clsfy_builder_1d* b)
{ os << b; }

#endif // clsfy_builder_1d_h_
