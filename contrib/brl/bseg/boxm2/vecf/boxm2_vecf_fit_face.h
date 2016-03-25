#ifndef boxm2_vecf_fit_face_h_
#define boxm2_vecf_fit_face_h_
//:
// \file
// \brief  Fit parameters of the 3-d composite face model to anchor data
//
// \author J.L. Mundy
// \date   31 December 2015
//
// A newer version of the original fit_skull class
//
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include "boxm2_vecf_composite_face_params.h"
#include "boxm2_vecf_labeled_point.h"
#include <vgl/vgl_vector_3d.h>
class boxm2_vecf_fit_face{
 public:
  // ids for face anchors
  enum mids {LEFT_LATERAL_CANTHUS, RIGHT_LATERAL_CANTHUS, LEFT_MEDIAL_CANTHUS, RIGHT_MEDIAL_CANTHUS, MID_UPPER_JAW, MID_FOREHEAD, MID_FOREHEAD_NORMAL,
             CANTHUS_MIDPOINT, FOREHEAD_INTERSECTION, NOSE, CHIN};

  boxm2_vecf_fit_face(){
   fill_smid_map();
   params_.compute_auxillary_pts();
 }

  //: read a 3-d anchor file:
  // canthi, forehead, jaw
  bool read_anchor_file(std::string const& path);

  //: load face params with existing transformation
  bool load_composite_face_params(std::string const& params_path);

  //: add a labeled point to lpts_
  bool add_labeled_point(boxm2_vecf_labeled_point lp);

  //: find additional points from the specfied geometry
  bool compute_auxillary_points();

  //: set the affine transform between source face and target head
  bool set_trans();

  //: transform face according to the computed affine transform
  bool transform_face(std::string const& source_face_path, std::string const& target_face_path) const;

  //: inverse transform face
  bool inverse_transform_face(std::string const& source_face_path, std::string const& target_face_path) const;

  //: return current parameter settings
  boxm2_vecf_composite_face_params params() const {return params_;}

 private:
  //: the map between string and enum
  void fill_smid_map();

  //: map a string label to the corresponding enum value
  std::map<std::string, mids> smid_map_;

  //: a map of labeled points, e.g. left_eye_lateral_canthus
  std::map<mids, boxm2_vecf_labeled_point> lpts_;

  //: normal to forehead (converted from point format)
  vgl_vector_3d<double> forehead_normal_;

  //: A vector to the canthus midpoint
  vgl_vector_3d<double> cmv_;

  //: parameters, contains nominal anchors for average face
  boxm2_vecf_composite_face_params params_;
};
#endif// boxm2_vecf_fit_face
