// This is mul/clsfy/clsfy_mean_square_1d_builder.h
#ifndef clsfy_mean_square_1d_builder_h_
#define clsfy_mean_square_1d_builder_h_
//:
// \file
// \brief Describe an concrete classifier builder for scalar data
// \author Tim Cootes

#include "clsfy_builder_1d.h"
#include <vcl_string.h>
#include <vcl_iosfwd.h>
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
  virtual ~clsfy_mean_square_1d_builder();

  //: Create empty model
  virtual clsfy_classifier_1d* new_classifier() const;


  //: Build a binary_threshold classifier
  //  Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the missclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  virtual double build(clsfy_classifier_1d& classifier,
                       const vnl_vector<double>& egs,
                       const vnl_vector<double>& wts,
                       const vcl_vector<unsigned> &outputs) const;

  //: Build a mean_square classifier
  // Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the missclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  virtual double build(clsfy_classifier_1d& classifier,
                       vnl_vector<double>& egs0,
                       vnl_vector<double>& wts0,
                       vnl_vector<double>& egs1,
                       vnl_vector<double>& wts1) const;

  //: Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples,
  //  weighting examples appropriately when estimating the missclassification rate.
  //  data[i] is a triple, {value,class_number,weight}
  //  Returns weighted sum of error.
  //  Note that input "data" must be sorted to use this routine
  virtual double build_from_sorted_data(clsfy_classifier_1d& classifier,
                                        const vbl_triple<double,int,int> *data,
                                        const vnl_vector<double>& wts) const;

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

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_mean_square_1d_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_mean_square_1d_builder& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_mean_square_1d_builder& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_mean_square_1d_builder* b);

#endif // clsfy_mean_square_1d_builder_h_
