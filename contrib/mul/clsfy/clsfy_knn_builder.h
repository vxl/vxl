// This is mul/clsfy/clsfy_knn_builder.h
#ifndef clsfy_knn_builder_h_
#define clsfy_knn_builder_h_

// Copyright: (C) 2001 Britsh Telecommunications plc

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Describe a knn classifier builder
// \author Ian Scott
// \date 2001/10/07


#include "clsfy/clsfy_builder_base.h"
#include <vcl_vector.h>
#include <vcl_string.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>


//: Builds clsfy_k_nearest_neighbour classifiers.
// This class doesn't do any calculation, it is easy to create a
// knn classfier directly - but it allows for consistent swappable
// classifiers.
class clsfy_knn_builder : public clsfy_builder_base
{
  //: The number of nearest neighbours to look for.
  unsigned k_;

 public:
  // Dflt ctor
  clsfy_knn_builder();

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

  //: The number of nearest neighbours to look for.
  unsigned k() const;

  //: Set the The number of nearest neighbours to look for.
  void set_k(unsigned);
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_knn_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_knn_builder& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_knn_builder& b);

#endif // clsfy_knn_builder_h_
