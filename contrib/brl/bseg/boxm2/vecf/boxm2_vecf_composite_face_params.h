#ifndef boxm2_vecf_composite_face_params_h_
#define boxm2_vecf_composite_face_params_h_
//:
// \file
// \brief  Parameters for the composite_face model
//
// \author J.L. Mundy
// \date   3 November 2015
//
#include <iostream>
#include "boxm2_vecf_articulated_params.h"
#include <vgl/vgl_vector_3d.h>
#include "boxm2_vecf_mandible_params.h"
#include "boxm2_vecf_cranium_params.h"
#include "boxm2_vecf_skin_params.h"
#include "boxm2_vecf_mouth_params.h"
#include "boxm2_vecf_middle_fat_pocket_params.h"
#include <vgl/algo/vgl_h_matrix_3d.h>
class boxm2_vecf_composite_face_params : public boxm2_vecf_articulated_params{
 public:
// reference points below picked manually from skull/skin_orig_appearance_sampled-r10-s1.05-t10.txt
boxm2_vecf_composite_face_params():  offset_(vgl_vector_3d<double>(0.0, 0.0, 0.0)), composite_face_intensity_((unsigned char)(200)),
    left_lateral_canthus_(vgl_point_3d<double>(46.4,20.35,88.0)), right_lateral_canthus_(vgl_point_3d<double>(-46.4,20.35,88.0)),
    left_medial_canthus_(vgl_point_3d<double>(27.0,20.1,89.5)), right_medial_canthus_(vgl_point_3d<double>(-27.0,20.1,89.5)),
    mid_upper_jaw_(vgl_point_3d<double>(0.0, -60.0, 96.0)), mid_forehead_(vgl_point_3d<double>(-0.14, 45.23,106.84)),
    mid_forehead_normal_(vgl_vector_3d<double>(0.004,0.033,0.9994)),nose_(vgl_point_3d<double>(0.0,-26.0, 119.0)),chin_(vgl_point_3d<double>(0.0,-107.0, 73.0)),trans_(vgl_h_matrix_3d<double>()){
    trans_.set_identity();
  }

 boxm2_vecf_composite_face_params(unsigned char composite_face_intensity):composite_face_intensity_(composite_face_intensity){
  }

  unsigned char composite_face_intensity_;
  vgl_vector_3d<double> offset_;
  boxm2_vecf_mandible_params mandible_params_;
  boxm2_vecf_cranium_params cranium_params_;
  boxm2_vecf_skin_params skin_params_;
  boxm2_vecf_mouth_params mouth_params_;
  boxm2_vecf_middle_fat_pocket_params middle_fat_pocket_params_;
  //
  // ==== parameters forfitting the face to a subject ======
  //

  //: the auxillary point is computed from the forehead plane
  void compute_auxillary_pts();

  // the points and vectors below are defined for the source face geometry
  // these points define the affine transformation of the face to the target head
  vgl_point_3d<double> left_lateral_canthus_;
  vgl_point_3d<double> right_lateral_canthus_;
  vgl_point_3d<double> left_medial_canthus_;
  vgl_point_3d<double> right_medial_canthus_;
  vgl_point_3d<double> mid_upper_jaw_;
  vgl_point_3d<double> mid_forehead_;
  vgl_vector_3d<double> mid_forehead_normal_;
  vgl_vector_3d<double> canth_mid_vect_;
  vgl_point_3d<double> forehead_intersection_;
  vgl_point_3d<double> nose_;
  vgl_point_3d<double> chin_;

  //: transform face from source to target
  vgl_h_matrix_3d<double> trans_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_composite_face_params const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_composite_face_params& pr);

#endif// boxm2_vecf_composite_face_params
