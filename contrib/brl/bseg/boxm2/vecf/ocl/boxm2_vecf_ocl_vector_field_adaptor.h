#ifndef boxm2_vecf_ocl_vector_field_adaptor_h_included_
#define boxm2_vecf_ocl_vector_field_adaptor_h_included_

#include <bocl/bocl_cl.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/vecf/boxm2_vecf_vector_field.h>

#include "boxm2_vecf_ocl_vector_field.h"

//: Convert a boxm2_vecf_vector_field to a boxm2_vecf_ocl_vector_field by inhereting from this class
class boxm2_vecf_ocl_vector_field_adaptor: public boxm2_vecf_ocl_vector_field
{
  public:
    boxm2_vecf_ocl_vector_field_adaptor(const boxm2_vecf_vector_field_base_sptr& cpu_xform);

    //: compute vector field, writing to gpu cache
    bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* pts_source,
                                           bocl_mem* pts_target,
                                           cl_command_queue &queue) override;

    //: compute inverse vector field, writing result to gpu cache
    bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* pts_target,
                                           bocl_mem* pts_source,
                                           cl_command_queue &queue) override;

  private:
    boxm2_vecf_vector_field_base_sptr cpu_xform_;

};

#endif
