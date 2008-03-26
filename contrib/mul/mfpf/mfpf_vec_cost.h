#ifndef  mfpf_vec_cost_h_
#define mfpf_vec_cost_h_

//:
// \file
// \brief Cost functions to evaluate vector of pixel samples
// \author Tim Cootes

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_cassert.h>
#include <vnl/vnl_vector.h>
#include <vcl_memory.h>

//: Cost functions to evaluate vector of pixel samples
//  Derived functions return some form of sum of differences 
//  from a mean, or Mahalanobis type measure.  Perfect match
//  returns zero, imperfect matches return positive numbers.
class mfpf_vec_cost
{
protected:
public:

  //: Dflt ctor
  mfpf_vec_cost();

  //: Destructor
  virtual ~mfpf_vec_cost();

  //: Evaluate function given vector
  virtual double evaluate(const vnl_vector<double>& v)=0;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_vec_cost* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const =0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const =0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) =0;
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_vec_cost& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_vec_cost& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_vec_cost& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_vec_cost& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_vec_cost* b);

#endif


