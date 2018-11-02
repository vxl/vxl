#ifndef mfpf_norm_corr2d_builder_h_
#define mfpf_norm_corr2d_builder_h_
//:
// \file
// \brief Builder for mfpf_norm_corr2d objects.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder_builder.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>

//: Builder for mfpf_norm_corr2d objects.
class mfpf_norm_corr2d_builder : public mfpf_point_finder_builder
{
 private:
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_x_;
  //: Kernel reference point (in kni_ x knj_ grid)
  double ref_y_;

  //: Kernel mask is ni_ x nj_
  unsigned ni_;
  //: Kernel mask is ni_ x nj_
  unsigned nj_;

  //: Number of angles either side of 0 to sample at
  unsigned nA_;

  //: Angle displacement
  double dA_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: Workspace for sum
  vil_image_view<double> sum_;

  //: Number of examples added
  unsigned n_added_;

  //: Define default values
  void set_defaults();

  //: Add one example to the model
  void add_one_example(const vimt_image_2d_of<float>& image,
                       const vgl_point_2d<double>& p,
                       const vgl_vector_2d<double>& u);

 public:

  // Dflt ctor
  mfpf_norm_corr2d_builder();

  // Destructor
  ~mfpf_norm_corr2d_builder() override;

  //: Define size of mask
  void set_kernel_size(unsigned ni, unsigned nj,
                       double ref_x, double ref_y);

  //: Define size of mask
  // Ref point is centre
  void set_kernel_size(unsigned ni, unsigned nj);

  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Currently just defines as a box.
  void set_region_size(double wi, double wj) override;

  //: Kernel mask is ni x nj
  unsigned ni() const { return ni_; }

  //: Kernel mask is ni x nj
  unsigned nj() const { return nj_; }

  //: Number of dimensions in the model
  unsigned model_dim() override;

  //: Create new mfpf_norm_corr2d on heap
  mfpf_point_finder* new_finder() const override;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  void clear(unsigned n_egs) override;

  //: Get sample of region around specified point in image
  void get_sample_vector(const vimt_image_2d_of<float>& image,
                                 const vgl_point_2d<double>& p,
                                 const vgl_vector_2d<double>& u,
                                 std::vector<double>& v) override;

  //: Add one example to the model
  void add_example(const vimt_image_2d_of<float>& image,
                           const vgl_point_2d<double>& p,
                           const vgl_vector_2d<double>& u) override;

  //: Build object from the data supplied in add_example()
  void build(mfpf_point_finder&) override;

  //: Initialise from a string stream
  bool set_from_stream(std::istream &is) override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif
