#ifndef mfpf_sad_vec_cost_builder_h_
#define mfpf_sad_vec_cost_builder_h_
//:
// \file
// \brief Builder for mfpf_sad_vec_cost objects.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <vector>
#include <mfpf/mfpf_vec_cost_builder.h>
#include <vnl/vnl_vector.h>
#include <vcl_compiler.h>

//: Builder for mfpf_sad_vec_cost objects.
class mfpf_sad_vec_cost_builder : public mfpf_vec_cost_builder
{
 private:
  //: Minimum mean abs diff allowed
  double min_mad_;

  //: Space to record examples
  std::vector<vnl_vector<double> > data_;

  //: If true recalculate min_mad_ based on median of training set
  bool impose_robust_min_mad_;

  //: Define default values
  void set_defaults();
 public:

  // Dflt ctor
  mfpf_sad_vec_cost_builder();

  // Destructor
  virtual ~mfpf_sad_vec_cost_builder();


  //: Create new mfpf_sad_vec_cost on heap
  virtual mfpf_vec_cost* new_cost() const;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs);

  //: Add one example to the model
  virtual void add_example(const vnl_vector<double>& v);

  //: Build object from the data supplied in add_example()
  virtual void build(mfpf_vec_cost&);

  //: Initialise from a string stream
  virtual bool set_from_stream(std::istream &is);

  //: Set flag for robust min mad (based on training set)
  void set_impose_robust_min_mad(bool on) {impose_robust_min_mad_=on;}

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_vec_cost_builder* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // mfpf_sad_vec_cost_builder_h_
