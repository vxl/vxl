#ifndef boxm2_vecf_vector_field_base_h_included_
#define boxm2_vecf_vector_field_base_h_included_

#include <vcl_compiler.h>
#include <iostream>
#include <string>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>


class boxm2_vecf_vector_field_base : public vbl_ref_count
{
  public:
    //: virtual destructor to ensure proper cleanup of base class pointers
    virtual ~boxm2_vecf_vector_field_base(){};

    //: write the locations of the cooresponding target points to source's BOXM2_POINT data
    virtual bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           const boxm2_data_traits<BOXM2_POINT>::datatype *source_pts,
                                           boxm2_data_traits<BOXM2_POINT>::datatype *target_pts) = 0;

    //: write the locations of the cooresponding source points to target's BOXM2_POINT data
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           const boxm2_data_traits<BOXM2_POINT>::datatype *target_pts,
                                           boxm2_data_traits<BOXM2_POINT>::datatype *source_pts) = 0;

    static bool compute_cell_centers(boxm2_scene_sptr scene,
                                     boxm2_block_id const& blk_id,
                                     boxm2_data_traits<BOXM2_POINT>::datatype *centers);


};

typedef vbl_smart_ptr<boxm2_vecf_vector_field_base> boxm2_vecf_vector_field_base_sptr;

#endif
