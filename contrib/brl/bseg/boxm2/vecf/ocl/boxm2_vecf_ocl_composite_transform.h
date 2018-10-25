#ifndef boxm2_vecf_ocl_composite_transform_h_included_
#define boxm2_vecf_ocl_composite_transform_h_included_

#include <bocl/bocl_cl.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/vecf/boxm2_vecf_vector_field.h>

#include "boxm2_vecf_ocl_vector_field.h"

//: string a series of vector_field transforms together
class boxm2_vecf_ocl_composite_transform: public boxm2_vecf_ocl_vector_field
{
  public:
    //: constructor
    boxm2_vecf_ocl_composite_transform(std::vector<boxm2_vecf_ocl_vector_field_sptr> xforms);

    //: compute forward transform
    // Note that both pts_source and pts_target are defined for each cell in source scene using data type float4
    bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* pts_source, // in
                                           bocl_mem* pts_target, // out
                                           cl_command_queue &queue) override;

    //: compute inverse vector field, writing result to gpu cache
    // Note that both pts_target and pts_source are defined for each cell in target scene using data type float4
    bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* pts_target, // in
                                           bocl_mem* pts_source, // out
                                           cl_command_queue &queue) override;

  private:
    std::vector<boxm2_vecf_ocl_vector_field_sptr> xforms_;
};

#endif
