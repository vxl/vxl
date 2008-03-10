#ifndef mfpf_edge_finder_builder_h_
#define mfpf_edge_finder_builder_h_

//:
// \file
// \brief Builder for mfpf_edge_finder objects.
// \author Tim Cootes

#include <mfpf/mfpf_point_finder_builder.h>
#include <vcl_iosfwd.h>

//: Builder for mfpf_edge_finder objects.
class mfpf_edge_finder_builder : public mfpf_point_finder_builder
{
 private:
 public:

  //: Dflt ctor
  mfpf_edge_finder_builder();

  //: Destructor
  virtual ~mfpf_edge_finder_builder();

  //: Create new mfpf_edge_finder on heap
  virtual mfpf_point_finder* new_finder() const;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs);

  //: Add one example to the model
  virtual void add_example(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u);

  //: Build object from the data supplied in add_example()
  virtual void build(mfpf_point_finder&);

  //: Initialise from a string stream
  virtual bool set_from_stream(vcl_istream &is);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_point_finder_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif
