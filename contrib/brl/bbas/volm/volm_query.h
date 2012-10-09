//This is brl/bbas/volm/volm_query.h
#ifndef volm_query_h_
#define volm_query_h_
//:
// \file
// \brief  A class to represent a volumetric matching query through an image mark-up and estimated camera parameters
//
// \author  
// \date October 07, 2012
// \verbatim
//   Modifications
//
// \endverbatim
// 

#include <vbl/vbl_ref_count.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>

class volm_query : public vbl_ref_count
{
public:
  volm_query(depth_map_scene_sptr dm, double focal, int ni, int nj, double heading, double tilt, double roll);
};

#endif  // volm_query_h_
