#ifndef mfpf_vec_cost_builder_h_
#define mfpf_vec_cost_builder_h_
//:
// \file
// \brief Base for classes which build mfpf_vec_cost objects.
// \author Tim Cootes

#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_memory.h>

#include <mfpf/mfpf_vec_cost.h>

//: Base for classes which build mfpf_vec_cost objects.
class mfpf_vec_cost_builder
{

 public:

  //: Dflt ctor
  mfpf_vec_cost_builder();

  //: Destructor
  virtual ~mfpf_vec_cost_builder();


  //: Create new vec_cost of appropriate type on heap
  virtual mfpf_vec_cost* new_cost() const =0;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs)=0;

  //: Add one example to the model
  virtual void add_example(const vnl_vector<double>& v)=0;

  //: Build object from the data supplied in add_example()
  virtual void build(mfpf_vec_cost&)=0;

  //: Initialise from a string stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_vec_cost_builder* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const =0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const=0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs)=0;

  //: Create a concrete object, from a text specification.
  static vcl_auto_ptr<mfpf_vec_cost_builder> create_from_stream(vcl_istream &is);
};

//: Allows derived class to be loaded by base-class pointer
void vsl_add_to_binary_loader(const mfpf_vec_cost_builder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_vec_cost_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_vec_cost_builder& b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_vec_cost_builder& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_vec_cost_builder* b);

#endif // mfpf_vec_cost_builder_h_
