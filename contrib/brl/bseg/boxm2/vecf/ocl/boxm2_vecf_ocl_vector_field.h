#ifndef boxm2_vecf_ocl_vector_field_h_included_
#define boxm2_vecf_ocl_vector_field_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <bocl/bocl_mem.h>

class boxm2_vecf_ocl_vector_field
{
  public:

    //: compute vector field, writing to gpu cache
    virtual bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* blk_data,
                                           cl_command_queue &queue) = 0;

    //: compute inverse vector field, writing result to gpu cache
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* blk_data,
                                           cl_command_queue &queue) = 0;

};

#endif
