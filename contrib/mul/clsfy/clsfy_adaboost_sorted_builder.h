#ifndef clsfy_adaboost_sorted_builder_h_
#define clsfy_adaboost_sorted_builder_h_

// Copyright: (C) 2000 Britsh Telecommunications plc


//:
// \file
// \brief Describe an concrete classifier
// \author Ian Scott
// \date 2000/05/10
// \verbatim
//  Modifications
//  2 May 2001 IMS Converted to VXL
// \endverbatim


#include "clsfy/clsfy_builder_base.h"
#include <vcl_vector.h>
#include <vcl_string.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>

//#include "clsfy_builder_1d.h"
//#include "clsfy_classifier_base.h"
class clsfy_builder_1d;
class clsfy_classifier_base;

//: Base for classes to build clsfy_classifier_base objects
class clsfy_adaboost_sorted_builder : public clsfy_builder_base
{
  // Parameters of builder

  //: bool indicating whether or not to save data to disk.
  // NB useful to save data to disk, if don't have enough RAM
  // but also makes training very slow
  bool save_data_to_disk_;

  //: batch size
  // ie number of training examples held in RAM whilst sorting takes place
  int bs_;

  //: maximum number of classifiers found by Adaboost algorithm
  int max_n_clfrs_;

  //: pointer to 1d builder used to build each weak classifier
  clsfy_builder_1d* weak_builder_;

public:

  // Dflt ctor
  clsfy_adaboost_sorted_builder();

  // Destructor
  virtual ~clsfy_adaboost_sorted_builder();

  //: Create empty model
  virtual clsfy_classifier_base* new_classifier() const;

  //: set batch size 
  void set_batch_size(int bs) { bs_ = bs; }

  //: set save data to disk bool
  void set_save_data_to_disk(bool x) { save_data_to_disk_ = x; }

  //: set max_n_clfrs 
  void set_max_n_clfrs(int max_n_clfrs) { max_n_clfrs_ = max_n_clfrs; }

  //: set weak builder ( a pointer is retained )
  void set_weak_builder(clsfy_builder_1d& weak_builder)
  { weak_builder_ = &weak_builder; }

  //: Build model from data
  // Return the mean error over the training set.
  // For many classifiers, you may use nClasses==1 to
  // indicate a binary classifier
  virtual double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const vcl_vector<unsigned> &outputs) const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_adaboost_sorted_builder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_adaboost_sorted_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_adaboost_sorted_builder& b);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const clsfy_adaboost_sorted_builder& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const clsfy_adaboost_sorted_builder* b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_adaboost_sorted_builder& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_adaboost_sorted_builder* b);

#endif // clsfy_adaboost_sorted_builder_h_
