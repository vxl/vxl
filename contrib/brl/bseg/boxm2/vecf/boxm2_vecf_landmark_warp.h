#ifndef boxm2_vecf_landmark_warp_h_included_
#define boxm2_vecf_landmark_warp_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_utility.h>

#include "boxm2_vecf_vector_field.h"

class boxm2_vecf_landmark_warp : public boxm2_vecf_vector_field
{
  public:
    boxm2_vecf_landmark_warp(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                             vcl_vector<vgl_point_3d<double> > const& control_pts_target);

    //: write vector field to source's BOXM2_VEC3D data
    virtual bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field);
    
    //: write vector field to target's BOXM2_VEC3D data
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field);

  private:
    typedef vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > control_pt_t;
    vcl_vector<control_pt_t> control_pts_;
};

#endif
