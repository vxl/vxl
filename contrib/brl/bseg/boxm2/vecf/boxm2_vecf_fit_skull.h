#ifndef boxm2_vecf_fit_skull_h_
#define boxm2_vecf_fit_skull_h_
// :
// \file
// \brief  Fit parameters of the skull model to data
//
// \author J.L. Mundy
// \date   6 Sept 2015
//
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include "boxm2_vecf_skull_params.h"
#include "boxm2_vecf_labeled_point.h"
#include <vcl_map.h>
#include <vgl/vgl_vector_3d.h>
class boxm2_vecf_fit_skull
{
public:
  // ids for skull measurements
  enum mids { LEFT_LATERAL_CANTHUS, RIGHT_LATERAL_CANTHUS, MID_UPPER_JAW, MID_FOREHEAD, MID_FOREHEAD_NORMAL,
              CANTHUS_MIDPOINT, FOREHEAD_INTERSECTION };

  boxm2_vecf_fit_skull() {fill_smid_map(); }

  // : read a 3-d anchor file:
  // canthi, forehead, jaw
  bool read_anchor_file(vcl_string const& path);

  // : add a labeled point to lpts_
  bool add_labeled_point(boxm2_vecf_labeled_point lp);

  // : find additional points from the specfied geometry
  bool compute_auxillary_points();

  // : set the affine transform between source skull and target head
  bool set_trans();

  // : transform skull
  bool transform_skull(vcl_string const& source_skull_path, vcl_string const& target_skull_path) const;

private:
  // : the map between string and enum
  void fill_smid_map();

  // : map a string label to the corresponding enum value
  vcl_map<vcl_string, mids> smid_map_;

  // : a map of labeled points, e.g. left_eye_lateral_canthus
  vcl_map<mids, boxm2_vecf_labeled_point> lpts_;

  // : normal to forehead (converted from point format)
  vgl_vector_3d<double> forehead_normal_;

  // : A vector to the canthus midpoint
  vgl_vector_3d<double> cmv_;

  // : parameters
  boxm2_vecf_skull_params params_;
};
#endif// boxm2_vecf_fit_skull
