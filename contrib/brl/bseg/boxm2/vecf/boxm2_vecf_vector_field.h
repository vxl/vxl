#ifndef boxm2_vecf_vector_field_h_included_
#define boxm2_vecf_vector_field_h_included_

#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>

//: Abstract vector field type - implementations must define forward and inverse mapping
class boxm2_vecf_vector_field : public vbl_ref_count
{
  public:

    //: write vector field to source's BOXM2_VEC3D data
    virtual bool compute_forward_transform(boxm2_scene_sptr source, 
                                           boxm2_block_id const& blk_id,
                                           boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field) = 0;
    
    //: write inverse vector field to target's BOXM2_VEC3D data
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field) = 0;

};

typedef vbl_smart_ptr<boxm2_vecf_vector_field> boxm2_vecf_vector_field_sptr;

#endif
