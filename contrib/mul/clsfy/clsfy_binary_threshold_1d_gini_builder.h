// This is mul/clsfy/clsfy_binary_threshold_1d_gini_builder.h
#ifndef clsfy_binary_threshold_1d_gini_builder_h_
#define clsfy_binary_threshold_1d_gini_builder_h_
//:
// \file
// \brief Builder  of 1d threshold using gini index
// \author Martin Roberts

#include <clsfy/clsfy_builder_1d.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <vbl/vbl_triple.h>
#include <clsfy/clsfy_classifier_1d.h>
#include <clsfy/clsfy_binary_threshold_1d_builder.h>
// Note this is used by clsfy_binary_tree_builder
// Derived from clsfy_binary_threshold_1d_builder but uses a slightly different
// interface to do the gini index optimisation, as tis returns the reduction
// in the gini impurity (not classification error).

class clsfy_binary_threshold_1d_gini_builder : public clsfy_binary_threshold_1d_builder
{
 public:

  // Dflt ctor
  clsfy_binary_threshold_1d_gini_builder();

  // Destructor
  virtual ~clsfy_binary_threshold_1d_gini_builder();

  //: Create empty model
  virtual clsfy_classifier_1d* new_classifier() const;


  //: Build a binary_threshold classifier
  //  Train classifier
  //  Selects parameters of classifier which best separate examples from two classes,
  // Uses the gini impurity index
  // Note it returns the -reduction in Gini impurity produced by the split
  // Not the misclassification rate
  // (i.e. but minimise as per error rate)
  virtual double build_gini(clsfy_classifier_1d& classifier,
                            const vnl_vector<double>& inputs,
                            const vcl_vector<unsigned> &outputs) const;

 
  //  Note that input "data" must be sorted to use this routine
  virtual double build_gini_from_sorted_data(clsfy_classifier_1d& classifier,
                                             const vcl_vector<vbl_triple<double,int,int> >& data) const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Name of the class
  virtual bool is_class(vcl_string const& s) const;

  //: Version number for I/O
  short version_no() const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_1d* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};


#endif // clsfy_binary_threshold_1d_gini_builder_h_
