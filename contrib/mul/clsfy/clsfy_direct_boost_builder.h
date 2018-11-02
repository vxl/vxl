// This is mul/clsfy/clsfy_direct_boost_builder.h
// Copyright: (C) 2000 British Telecommunications plc
#ifndef clsfy_direct_boost_builder_h_
#define clsfy_direct_boost_builder_h_
//:
// \file
// \brief Describe a concrete classifier
// \author dac
// \date 2000-05-10

#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#include <clsfy/clsfy_builder_base.h> // parent class
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>
class clsfy_builder_1d;
class clsfy_classifier_base;
class clsfy_direct_boost;

//: Base for classes to build clsfy_classifier_base objects
class clsfy_direct_boost_builder : public clsfy_builder_base
{
  // Parameters of builder

  //: calc all threshold not just last one
  bool calc_all_thresholds_;

  //: proportion of sum of square distance from mean used to say whether two classifiers are too similar.
  // typically = 0.1 ish.
  double prop_;


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

//==============================private methods============================

  //: Calc threshold for current version of strong classifier
  double calc_threshold(clsfy_direct_boost& strong_classifier,
                        mbl_data_wrapper<vnl_vector<double> >& inputs,
                        const std::vector<unsigned>& outputs) const;

  //: Calc similarity between two 1d input vectors
  double calc_prop_same(const std::vector<bool>& vec1,
                        const std::vector<bool>& vec2) const;

 public:

  // Dflt ctor
  clsfy_direct_boost_builder();

  // Destructor
  ~clsfy_direct_boost_builder() override;

  //: Create empty model
  clsfy_classifier_base* new_classifier() const override;

  //: set batch size
  void set_batch_size(int bs) { bs_ = bs; }

  //: set save data to disk bool
  void set_save_data_to_disk(bool x) { save_data_to_disk_ = x; }

  //: set max_n_clfrs
  void set_max_n_clfrs(int max_n_clfrs) { max_n_clfrs_ = max_n_clfrs; }

  //: set weak builder ( a pointer is retained )
  void set_weak_builder(clsfy_builder_1d& weak_builder)
  { weak_builder_ = &weak_builder; }

   //: set stuff
  void set_calc_all_thresholds(bool x) { calc_all_thresholds_ = x; }
  void set_prop(double prop) { prop_ = prop; }

  //: Build model from data
  // Return the mean error over the training set.
  // For many classifiers, you may use nClasses==1 to
  // indicate a binary classifier
  double build(clsfy_classifier_base& model,
                       mbl_data_wrapper<vnl_vector<double> >& inputs,
                       unsigned nClasses,
                       const std::vector<unsigned> &outputs) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Name of the class
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  clsfy_builder_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_direct_boost_builder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_direct_boost_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_direct_boost_builder& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const clsfy_direct_boost_builder& b);

//: Stream output operator for class pointer
void vsl_print_summary(std::ostream& os,const clsfy_direct_boost_builder* b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const clsfy_direct_boost_builder& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const clsfy_direct_boost_builder* b);

#endif // clsfy_direct_boost_builder_h_
