// This is mul/clsfy/clsfy_adaboost_trainer.h
#ifndef clsfy_adaboost_trainer_h_
#define clsfy_adaboost_trainer_h_
//:
// \file
// \brief Functions to train classifiers using AdaBoost algorithm
// \author dac
// \date   Fri Mar  1 23:49:39 2002
//  Functions to train classifiers using AdaBoost algorithm
//  AdaBoost combines a set of (usually simple, weak) classifiers into
//  a more powerful single classifier.  Essentially it selects the
//  classifiers one at a time, choosing the best at each step.
//  The classifiers are trained to distinguish the examples mis-classified
//  by the currently selected classifiers.

#include <string>
#include <iostream>
#include <iosfwd>
#include <vsl/vsl_binary_io.h>
#include <clsfy/clsfy_simple_adaboost.h>
#include <clsfy/clsfy_builder_1d.h>
#include <mbl/mbl_data_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

//: A class for some purpose.
// The purpose of this class is to prove that 1+1=3.
class clsfy_adaboost_trainer
{
 public:

  //: Dflt ctor
  clsfy_adaboost_trainer();

  //: Destructor
  virtual ~clsfy_adaboost_trainer();

  //: Build classifier composed of 1d classifiers working on individual vector elements
  //  Builds an n-component classifier, each component of which is a 1D classifier
  //  working on a single element of the input vector.
  void build_strong_classifier(clsfy_simple_adaboost& strong_classifier,
                               int max_n_clfrs,
                               clsfy_builder_1d& builder,
                               mbl_data_wrapper<vnl_vector<double> >& egs0,
                               mbl_data_wrapper<vnl_vector<double> >& egs1);

  //: Correctly classified examples have weights scaled by beta
  void clsfy_update_weights_weak(vnl_vector<double> &wts,
                                 const vnl_vector<double>& data,
                                 clsfy_classifier_1d& classifier,
                                 int class_number,
                                 double beta);

  //: Extracts the j-th element of each vector in data and puts into v
  void clsfy_get_elements(vnl_vector<double>& v,
                          mbl_data_wrapper<vnl_vector<double> >& data, int j);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const;

  //: Name of the class
  virtual bool is_class(std::string const& s) const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream.
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

 protected:
#if 0
  // This is required if there are any references to objects
  // created on the heap. A deep copy should be made of anything
  // referred to by pointer during construction by copy. The copy
  // constructor is protected to stop its use for class
  // instantiation. It should be implemented in terms of the
  // assignment operator.

  //: Copy constructor
  clsfy_adaboost_trainer( const clsfy_adaboost_trainer& b );

  //: Assignment operator
  clsfy_adaboost_trainer& operator=( const clsfy_adaboost_trainer& b );
#endif
};

//=======================================================================

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_adaboost_trainer& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_adaboost_trainer& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const clsfy_adaboost_trainer& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const clsfy_adaboost_trainer& b);

#endif // clsfy_adaboost_trainer_h_
