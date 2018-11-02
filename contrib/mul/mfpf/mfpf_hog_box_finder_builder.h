#ifndef mfpf_hog_box_finder_builder_h_
#define mfpf_hog_box_finder_builder_h_
//:
// \file
// \brief Builder for mfpf_region_finder objects.
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder_builder.h>
#include <mipa/mipa_vector_normaliser.h>
#include <mfpf/mfpf_vec_cost_builder.h>
#include <mbl/mbl_cloneable_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mfpf/mfpf_region_form.h>


//: Builder for mfpf_region_finder objects.
// Text for configuring:
// \verbatim
// mfpf_hog_box_finder_builder { shape: ellipse { ri: 5 rj: 3  }
//  vec_cost_builder: mfpf_sad_vec_cost_builder { min_mad: 1.0 }
//  normaliser: mipa_ms_block_normaliser
  // {
  //   nscales: 2
  //   include_overall_histogram: true
  //   normaliser: mipa_l2norm_vector_normaliser
  //   ni: 16
  //   nj: 16
  //   nc_per_block: 4
  // }
//  search_ni: 5 search_nj: 4
// }
// \endverbatim
// Alternative for shape:
// \verbatim
//   shape: box { ni: 5 nj: 3 ref_x: 2.5 ref_y: 1.5 }
// \endverbatim
//
// // Alternative for shape and block normalise:
// \verbatim
//   shape: box { ni: 16 nj: 16 ref_x: 8 ref_y: 8 }
//
//  normaliser: mipa_ms_block_normaliser
  // {
  //   nscales: 2
  //   include_overall_histogram: true
  //   normaliser: mipa_l2norm_vector_normaliser
  //   ni: 16
  //   nj: 16
  //   nc_per_block: 4
  // }
// \endverbatim

class mfpf_hog_box_finder_builder : public mfpf_point_finder_builder
{
 private:
  //: Kernel reference point (usually centre of sampled region [0,ni)
  double ref_x_;
  //: Kernel reference point (usually centre of sampled region) [0,nj)
  double ref_y_;

  //: Number of angle bins in histogram of orientations
  unsigned nA_bins_;

  //: When true, angles are 0-360, else 0-180
  bool full360_;

  //: Size of each cell for basic histogram is nc x nc
  unsigned nc_;

  //: Size of region is 2*ni by 2*nj cells (each cell is nc*nc)
  unsigned ni_;
  //: Size of region is 2*ni by 2*nj cells (each cell is nc*nc)
  unsigned nj_;

  //: Builder for cost model
  mbl_cloneable_ptr<mfpf_vec_cost_builder> cost_builder_;

  ////: Which normalisation to use (0=none, 1=linear)
  //short norm_method_;

  //: The normaliser
  mbl_cloneable_nzptr<mipa_vector_normaliser> normaliser_;

  //: Number of angles either side of 0 to sample at
  unsigned nA_;

  //: Angle displacement
  double dA_;

  //: Relative size of region used for estimating overlap
  //  If 0.5, then overlap requires pt inside central 50% of region.
  double overlap_f_;

  //: Define default values
  void set_defaults();

  //: Add one example to the model
  void add_one_example(const vimt_image_2d_of<float>& image,
                       const vgl_point_2d<double>& p,
                       const vgl_vector_2d<double>& u);

  //: Ensure any  block normaliser is consistent with region sizing, bin numbers etc
  void reconfigure_normaliser();

 public:

  // Dflt ctor
  mfpf_hog_box_finder_builder();

  // Destructor
  ~mfpf_hog_box_finder_builder() override;

  //: Define number of angle bins and size of cells over which to pool
  //  If use360 is true, angles range [0,360), otherwise [0,180)
  //  (ie wrap-around occurs at 180,  5==185).
  void set_angle_bins(unsigned nA_bins, bool full360, unsigned cell_size);

  //: Define model region as an ni x nj box
  void set_as_box(unsigned ni, unsigned nj,
                  double ref_x, double ref_y,
                  const mfpf_vec_cost_builder& builder);

  //: Define model region as an ni x nj box
  //  Ref. point in centre.
  void set_as_box(unsigned ni, unsigned nj,
                  const mfpf_vec_cost_builder& builder);

  //: Define model region as an ni x nj box
  void set_as_box(unsigned ni, unsigned nj,
                  double ref_x, double ref_y);

  //: Define region size in world co-ordinates
  //  Sets up ROI to cover given box (with samples at step_size()),
  //  with ref point at centre.
  //  Currently just defines as a box
  void set_region_size(double wi, double wj) override;

  //: Builder for PDF
  mfpf_vec_cost_builder& cost_builder() { return cost_builder_; }

  //: Create new mfpf_region_finder on heap
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

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mfpf_point_finder_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Prints ASCII representation of shape to os
  void print_shape(std::ostream& os) const;

  //: Version number for I/O
  short version_no() const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif
