#ifndef boxm2_vecf_landmark_warp_h_included_
#define boxm2_vecf_landmark_warp_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include "boxm2_vecf_vector_field.h"

//: helper class for mapping points from source to target and back
class boxm2_vecf_landmark_mapper
{
  public:
  boxm2_vecf_landmark_mapper(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                             vcl_vector<vgl_point_3d<double> > const& control_pts_target);

  vgl_point_3d<double> operator() (vgl_point_3d<double> const& x) const;

  private:
    const vcl_vector<vgl_point_3d<double> > control_pts_source_;
    const vcl_vector<vgl_point_3d<double> > control_pts_target_;
};

class boxm2_vecf_landmark_warp : public boxm2_vecf_vector_field<boxm2_vecf_landmark_mapper>
{
  public:
    boxm2_vecf_landmark_warp(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                             vcl_vector<vgl_point_3d<double> > const& control_pts_target);

  private:
    typedef boxm2_vecf_landmark_mapper MAPPER_T;

    //: Create a function object that maps source pts to target pts.
    virtual MAPPER_T make_forward_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id);
    //: Create a function object that maps target pts to source pts.
    virtual MAPPER_T make_inverse_mapper(boxm2_scene_sptr target, boxm2_block_id const& blk_id);

    //: data
    const vcl_vector<vgl_point_3d<double> > control_pts_source_;
    const vcl_vector<vgl_point_3d<double> > control_pts_target_;


};

#endif
