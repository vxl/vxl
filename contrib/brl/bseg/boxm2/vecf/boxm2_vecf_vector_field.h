#ifndef boxm2_vecf_vector_field_h_included_
#define boxm2_vecf_vector_field_h_included_

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include "boxm2_vecf_vector_field_base.h"


//: Vector Field warping - T must have () operator mapping vgl_point_3d<double> -> vgl_point_3d<double>
// i.e. vgl_point_3d<double> T::operator () (vgl_point_3d<double> const& in)
template <class T>
class boxm2_vecf_vector_field : public boxm2_vecf_vector_field_base
{
  public:

    //: destructor
    ~boxm2_vecf_vector_field() override= default;;

    //: write the locations of the cooresponding target points to source's BOXM2_POINT data
    bool compute_forward_transform(boxm2_scene_sptr source,
                                   boxm2_block_id const& blk_id,
                                   const boxm2_data_traits<BOXM2_POINT>::datatype *source_pts,
                                   boxm2_data_traits<BOXM2_POINT>::datatype *target_pts) override;

    //: write the locations of the cooresponding source points to target's BOXM2_POINT data
    bool compute_inverse_transform(boxm2_scene_sptr target,
                                   boxm2_block_id const& blk_id,
                                   const boxm2_data_traits<BOXM2_POINT>::datatype *target_pts,
                                   boxm2_data_traits<BOXM2_POINT>::datatype *source_pts) override;
  private:

    //: Create a function object that maps source pts to target pts. Must be implemented by derived classes.
    virtual T make_forward_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id) = 0;
    //: Create a function object that maps target pts to source pts. Must be implemented by derived classes.
    virtual T make_inverse_mapper(boxm2_scene_sptr target, boxm2_block_id const& blk_id) = 0;

    //: fwd/inverse agnostic transformation function
    bool compute_transformed_pts(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                                 T &f,
                                 const boxm2_data_traits<BOXM2_POINT>::datatype *input,
                                 boxm2_data_traits<BOXM2_POINT>::datatype *output);


};

#endif
