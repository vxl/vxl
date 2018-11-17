// This is//projects/vxl/src/contrib/brl/bseg/bvpl/util/bvpl_pair_finder.h
#ifndef bvpl_pair_finder_h
#define bvpl_pair_finder_h
//:
// \file
// \brief
// \author Isabel Restrepo mir@lems.brown.edu
// \date  9/23/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_box_3d.h>

#include <vnl/vnl_float_3.h>
#include <vnl/vnl_float_4.h>

class bvpl_feature
{
 public:
  //:Default constructor
  bvpl_feature() : axis_(vnl_float_3(0.0f, 0.0f, 0.0f)), angle_(0.0f), response_(0.0f),
                   id_(-1), this_location_(vgl_point_3d<int>(-1,-1,-1)) {}

  //: Constructor from axis, angle and response
  bvpl_feature(vgl_point_3d<int> const &this_location, int const id, vnl_float_3 const&axis, float const angle, float const response)
  : axis_(axis), angle_(angle), response_(response), id_(id), this_location_(this_location) {}

  //: Copy constructor
  bvpl_feature(bvpl_feature const& f) = default;

  //: Accessors
  vnl_float_3 axis() const { return axis_; }
  float angle()      const { return angle_; }
  float response()   const { return response_; }
  int id()           const { return id_; }
  vgl_point_3d<int> this_location() const { return this_location_; }

 private:
  vnl_float_3 axis_;
  float angle_;
  float response_;
  int id_;
  vgl_point_3d<int> this_location_;
};

//: A simple class to store a pair of features
class bvpl_pair
{
 public:
  //: Default constructor
  bvpl_pair() : f1_(bvpl_feature()), f2_(bvpl_feature()), axis_(vnl_float_3(0.0f, 0.0f, 0.0f)),
                angle_(0.0f), response_(0.0f), id_(-1), this_location_(vgl_point_3d<int>(-1,-1,-1)) {}

  //: Constructor from features, rotation axis and angle of rotation
  bvpl_pair(vgl_point_3d<int> this_location, bvpl_feature f1,  bvpl_feature f2, int id, vnl_float_3 axis, float angle, float response )
  : f1_(f1), f2_(f2), axis_(axis), angle_(angle), response_(response), id_(id), this_location_(this_location) {}

  //: Copy constructor
  bvpl_pair(bvpl_pair const& pair) = default;

  // === Accessors ===

  bvpl_feature f1() const { return f1_; }
  bvpl_feature f2() const { return f2_; }
  vnl_float_3 axis() const { return axis_; }
  float angle() const { return angle_; }
  float response() const { return response_; }
  int id() const { return id_; }
  vgl_point_3d<int> this_location() const { return this_location_; }

 private:
  bvpl_feature f1_;
  bvpl_feature f2_;
  vnl_float_3 axis_;
  float angle_;
  float response_;
  int id_;
  vgl_point_3d<int> this_location_;
};

// A simple class to hold a vector of vgl_lines for visualizing corner pairs
class bvpl_corner_pairs: public vbl_ref_count
{
 public:
  //: Default constructor
  bvpl_corner_pairs() = default;

  //: Constructor from a vector
  bvpl_corner_pairs(std::vector<std::vector<vgl_line_segment_3d<int> > >& all_lines) { pairs_ = all_lines; }

  //: Constructor from  2 vectors
  bvpl_corner_pairs(std::vector<std::vector<vgl_line_segment_3d<int> > >& all_lines,
                    std::vector<std::vector<vgl_box_3d<int> > > all_boxes)
  {
    pairs_ = all_lines;
    boxes_ =all_boxes;
  }

  int size() const { return pairs_.size(); }

  //: vector of pairs
  std::vector<std::vector<vgl_line_segment_3d<int> > > pairs_;

  // a vector of kernel coverage at certain points
  std::vector<std::vector<vgl_box_3d<int> > > boxes_;
};

typedef vbl_smart_ptr<bvpl_corner_pairs> bvpl_corner_pairs_sptr;

class bvpl_corner_pair_finder
{
 public:
  // Constructor
  bvpl_corner_pair_finder() = default;

  //: Find pairs that are 90 degrees apart e.g |_ with _|
  static bvpl_corner_pairs_sptr find_pairs(bvxm_voxel_grid<int>* id_grid,
                                           const bvpl_kernel_vector_sptr& search_kernels,
                                           const bvpl_kernel_vector_sptr& corner_kernels);

  //: Find pairs that are 90 degrees apart e.g |_ with _|
  static bvpl_corner_pairs_sptr find_pairs(bvxm_voxel_grid<int>* id_grid,
                                           bvxm_voxel_grid<float>* response_grid,
                                           const bvpl_kernel_vector_sptr& search_kernels,
                                           const bvpl_kernel_vector_sptr& corner_kernels);

  //: Find pairs that are 90 degrees apart e.g |_ with _|
  static bvpl_corner_pairs_sptr find_pairs(bvxm_voxel_grid<int>* id_grid,
                                           bvxm_voxel_grid<float>* response_grid,
                                           const bvpl_kernel_vector_sptr& search_kernels,
                                           const bvpl_kernel_vector_sptr& corner_kernels,
                                           bvxm_voxel_grid<bvpl_pair> *pair_grid);

  //: Find pairs that are 90 degrees apart e.g |_ with _|
  static void find_pairs_no_lines(bvxm_voxel_grid<int>* id_grid,
                                  bvxm_voxel_grid<float>* response_grid,
                                  const bvpl_kernel_vector_sptr& search_kernels,
                                  const bvpl_kernel_vector_sptr& corner_kernels,
                                  bvxm_voxel_grid<bvpl_pair> *pair_grid);

  //: Find pairs that are 90 degrees apart e.g _ with |
  static bvpl_corner_pairs_sptr find_pairs(bvxm_voxel_grid<bvpl_pair> *pair_grid_in,
                                           const bvpl_kernel_vector_sptr& search_kernels,
                                           bvxm_voxel_grid<bvpl_pair> *pair_grid_out,
                                           int opposite_angle);

  //: Find pairs that are 90 degrees apart e.g _ with |
  static void find_pairs_no_lines(bvxm_voxel_grid<bvpl_pair> *pair_grid_in,
                                  const bvpl_kernel_vector_sptr& search_kernels,
                                  bvxm_voxel_grid<bvpl_pair> *pair_grid_out,
                                  int opposite_angle);
};

void bvpl_convert_pair_grid_to_hsv_grid(bvxm_voxel_grid<bvpl_pair> *pair_grid,
                                        bvxm_voxel_grid<vnl_float_4> *out_grid,
                                        std::vector<float> colors);

void bvpl_convert_pair_grid_to_float_grid(bvxm_voxel_grid<bvpl_pair> *pair_grid,
                                          bvxm_voxel_grid<float> *out_grid );

#endif
