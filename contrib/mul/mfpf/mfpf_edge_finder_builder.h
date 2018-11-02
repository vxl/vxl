#ifndef mfpf_edge_finder_builder_h_
#define mfpf_edge_finder_builder_h_
//:
// \file
// \brief Builder for mfpf_edge_finder objects.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder_builder.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Builder for mfpf_edge_finder objects.
class mfpf_edge_finder_builder : public mfpf_point_finder_builder
{
 private:
 public:

  //: Dflt ctor
  mfpf_edge_finder_builder();

  //: Destructor
  ~mfpf_edge_finder_builder() override;

  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Currently does nothing.
  void set_region_size(double wi, double wj) override;

  //: Create new mfpf_edge_finder on heap
  mfpf_point_finder* new_finder() const override;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  void clear(unsigned n_egs) override;

  //: Add one example to the model
  void add_example(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u) override;

  //: Build object from the data supplied in add_example()
  void build(mfpf_point_finder&) override;

  //: Initialise from a string stream
  bool set_from_stream(std::istream &is) override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif
