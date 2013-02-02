// This is brl/bseg/boxm2/volm/boxm2_volm_matcher_p0.h
#ifndef boxm2_volm_matcher_p0_h_
#define boxm2_volm_matcher_p0_h_
//:
// \file
// \brief  A class to match query regions and their attributes to indexed reference regions and attributes
//
// \author Ozge C. Ozcanli 
// \date January 25, 2013
// \verbatim
//   Modifications
// \endverbatim
//

#include <volm/volm_io.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_query_sptr.h>

#include "boxm2_volm_wr3db_index_sptr.h"

#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>

class boxm2_volm_matcher_p0
{
public:
  boxm2_volm_matcher_p0(volm_camera_space_sptr cam_space, volm_query_sptr query, float threshold, unsigned max_cam_per_loc = 200) : 
    cam_space_(cam_space), query_(query), threshold_(threshold), max_cam_per_loc_(max_cam_per_loc) {}
  ~boxm2_volm_matcher_p0();

  //: matcher function
  bool match(vgl_point_3d<double> const& hyp, vcl_vector<unsigned char> const& index_values, volm_score_sptr score);

  //: find the score of orientation of all the flat and vertical objects
  float orientation_score(unsigned cam_id, vcl_vector<unsigned char> const& index_values);
  //: find the score measuring overlap of sky region
  float sky_score(unsigned cam_id, vcl_vector<unsigned char> const& index_values);

private:
  volm_camera_space_sptr cam_space_;
  volm_query_sptr query_;
  float threshold_;
  unsigned max_cam_per_loc_;
  
};

#endif // boxm2_volm_matcher_p0_h_