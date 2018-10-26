#ifndef boxm2_vecf_ocl_vector_field_h_included_
#define boxm2_vecf_ocl_vector_field_h_included_

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <bocl/bocl_mem.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class boxm2_vecf_ocl_vector_field : public vbl_ref_count
{
  public:

    //: compute vector field, writing to gpu cache
    // Note that both pts_source and pts_target are defined for each cell in source scene using data type float4
    virtual bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* pts_source, // in
                                           bocl_mem* pts_target, // out
                                           cl_command_queue &queue) = 0;

    //: compute inverse vector field, writing result to gpu cache
    // Note that both pts_target and pts_source are defined for each cell in target scene using data type float4
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* pts_target, // in
                                           bocl_mem* pts_source, // out
                                           cl_command_queue &queue) = 0;

    //: virtual destructor to ensure proper cleanup when used polymorphically
    ~boxm2_vecf_ocl_vector_field() override= default;

  private:
    //TODO: might want to just make virtual method that returns kernel so we can reuse other ocl boilerplate stuff

};

typedef vbl_smart_ptr<boxm2_vecf_ocl_vector_field> boxm2_vecf_ocl_vector_field_sptr;

#endif
