// This is mul/clsfy/clsfy_builder_1d.h
#ifndef clsfy_builder_1d_h_
#define clsfy_builder_1d_h_
//:
// \file
// \brief Describe an abstract classifier builder for scalar data
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <vnl/vnl_vector.h>
class clsfy_classifier_1d;
#include <vbl/vbl_triple.h>

//: Base for classes to build clsfy_classifier_1d objects
class clsfy_builder_1d
{
 public:

  // Dflt ctor
  clsfy_builder_1d();

  // Destructor
  virtual ~clsfy_builder_1d();

  //: Create empty model
  virtual clsfy_classifier_1d* new_classifier() const = 0;

  //: Build a binary_threshold classifier
  // Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples from two classes,
  //  weighting examples appropriately when estimating the missclassification rate.
  //  Returns weighted sum of error, e.wts, where e_i =0 for correct classifications,
  //  e_i=1 for incorrect.
  virtual double build(clsfy_classifier_1d& classifier,
                       vnl_vector<double>& egs0,
                       vnl_vector<double>& wts0,
                       vnl_vector<double>& egs1,
                       vnl_vector<double>& wts1) const = 0;

   //: Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate examples,
  //  weighting examples appropriately when estimating the missclassification rate.
  //  data[i] is a triple, {value,class_number,weight}
  //  Returns weighted sum of error.
  //  Note that input "data" must be sorted to use this routine
  virtual double build_from_sorted_data(clsfy_classifier_1d& classifier,
               const vbl_triple<double,int,int> *data,
               const vnl_vector<double>& wts) const = 0;


  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual clsfy_builder_1d* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const clsfy_builder_1d& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const clsfy_builder_1d& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, clsfy_builder_1d& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_1d& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const clsfy_builder_1d* b);

#endif // clsfy_builder_1d_h_
