#ifndef clsfy_builder_base_h_
#define clsfy_builder_base_h_

// Copyright: (C) 2000 British Telecommunications plc


//:
// \file
// \brief Describe an abstract classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//  2 May 2001 IMS Converted to VXL
// \endverbatim


#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>


class clsfy_classifier_base;


//: Base for classes to build clsfy_classifier_base objects
class clsfy_builder_base {

public:

  // Dflt ctor
  clsfy_builder_base();

  // Destructor
  virtual ~clsfy_builder_base();

  //: Create empty model
  virtual clsfy_classifier_base* new_classifier() const = 0;

  //: Build model from data
  // Return the mean error over the training set.
  // For many classifiers, you may use nClasses==1 to
  // indicate a binary classifier
  virtual double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const vcl_vector<unsigned> &outputs) const = 0;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_base* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_builder_base& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_builder_base& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_builder_base& b);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const clsfy_builder_base& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const clsfy_builder_base* b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_base& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_base* b);

#endif // clsfy_builder_base_h_
