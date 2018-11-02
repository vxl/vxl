#ifndef boxm2_vecf_landmark_warp_h_included_
#define boxm2_vecf_landmark_warp_h_included_

#include <vector>
#include <iostream>
#include <utility>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// forward declare rsdl_kd_tree so we can use pointers to it
class rsdl_kd_tree;

#include "boxm2_vecf_vector_field.h"


//: helper class for mapping points from source to target and back
template<class F>
class boxm2_vecf_landmark_mapper
{
  public:
  boxm2_vecf_landmark_mapper(std::vector<vgl_point_3d<double> > const& control_pts_source,
                             std::vector<vgl_point_3d<double> >  control_pts_target,
                             F weight_function,
                             int n_nearest = 3);

  ~boxm2_vecf_landmark_mapper();

  vgl_point_3d<double> operator() (vgl_point_3d<double> const& x) const;

  private:

    const std::vector<vgl_point_3d<double> > control_pts_source_;
    const std::vector<vgl_point_3d<double> > control_pts_target_;
    F weight_function_;
    int n_nearest_;
    rsdl_kd_tree* source_kd_tree_;
};

template<class F>
class boxm2_vecf_landmark_warp : public boxm2_vecf_vector_field<boxm2_vecf_landmark_mapper<F> >
{
  public:
    boxm2_vecf_landmark_warp(std::vector<vgl_point_3d<double> >  control_pts_source,
                             std::vector<vgl_point_3d<double> >  control_pts_target,
                             F weight_function);

  private:
    typedef boxm2_vecf_landmark_mapper<F> MAPPER_T;

    //: Create a function object that maps source pts to target pts.
    MAPPER_T make_forward_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id) override;
    //: Create a function object that maps target pts to source pts.
    MAPPER_T make_inverse_mapper(boxm2_scene_sptr target, boxm2_block_id const& blk_id) override;

    //: data
    const std::vector<vgl_point_3d<double> > control_pts_source_;
    const std::vector<vgl_point_3d<double> > control_pts_target_;
    F weight_function_;


};

#endif
