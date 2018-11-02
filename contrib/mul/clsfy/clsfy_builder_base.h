// Copyright: (C) 2000 British Telecommunications plc
#ifndef clsfy_builder_base_h_
#define clsfy_builder_base_h_
//:
// \file
// \brief Describe an abstract classifier
// \author Ian Scott
// \date 2000-05-10
// \verbatim
//  Modifications
//   2 May 2001 IMS Converted to VXL
// \endverbatim

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_io.h>

class clsfy_classifier_base;

//: Base for classes to build clsfy_classifier_base objects
class clsfy_builder_base
{
 public:
  // Destructor
  virtual ~clsfy_builder_base() = default;

  //: Create empty model
  virtual clsfy_classifier_base* new_classifier() const = 0;

  //: Build model from data
  // Return the mean error over the training set.
  // For many classifiers, you may use nClasses==1 to
  // indicate a binary classifier
  virtual double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const std::vector<unsigned> &outputs) const = 0;

  //: Name of the class
  virtual std::string is_a() const;

  //: Name of the class
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_base* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;

  //: Load description from a text stream
  static std::unique_ptr<clsfy_builder_base> new_builder(
    std::istream &as);

  //: Initialise the parameters from a text stream.
  // Default case accepts no parameters.
  virtual void config(std::istream &as);

};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_builder_base& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_builder_base& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_builder_base& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const clsfy_builder_base& b);

//: Stream output operator for class pointer
void vsl_print_summary(std::ostream& os,const clsfy_builder_base* b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const clsfy_builder_base& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const clsfy_builder_base* b);

#endif // clsfy_builder_base_h_
