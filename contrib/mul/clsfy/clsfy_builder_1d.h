#ifndef clsfy_builder_1d_h_
#define clsfy_builder_1d_h_


//:
// \file
// \brief Describe an abstract classifier builder for scalar data
// \author Tim Cootes


#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <mbl/mbl_data_wrapper.h>
#include <vnl/vnl_vector.h>


class clsfy_classifier_1d;


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

  //: Train classifier, returning weighted error
  //  Selects parameters of classifier which best separate n examples into
  //  classes indicated by class_index[i],
  //  weighting examples appropriately when estimating the missclassification rate.
  // Return the weighted error over the training set.
  // For many classifiers, you may use nClasses==1 to
  // indicate a binary classifier
  virtual double build(clsfy_classifier_1d& model,
                       const double* value,
                       const int* class_index,
                       const double* wts,
                       int n,
                       unsigned nClasses) const = 0;

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
