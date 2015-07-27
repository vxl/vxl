#ifndef boxm2_vecf_ocl_similarity_transform_h_included_
#define boxm2_vecf_ocl_similarity_transform_h_included_

#include <bocl/bocl_cl.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/vecf/boxm2_vecf_similarity_transform.h>

#include "boxm2_vecf_ocl_vector_field.h"

class boxm2_vecf_ocl_similarity_transform : public boxm2_vecf_ocl_vector_field
{
  public:
    boxm2_vecf_ocl_similarity_transform(vgl_rotation_3d<double> rot,
                                        vgl_vector_3d<double> trans,
                                        vgl_vector_3d<double> scale);

    //: compute vector field, writing to gpu cache
    virtual bool compute_forward_transform(boxm2_scene_sptr source,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* blk_data,
                                           cl_command_queue &queue);

    //: compute inverse vector field, writing result to gpu cache
    virtual bool compute_inverse_transform(boxm2_scene_sptr target,
                                           boxm2_block_id const& blk_id,
                                           bocl_mem* blk_data,
                                           cl_command_queue &queue);

  private:
    boxm2_vecf_similarity_transform cpu_xform_;

};

#endif
