#ifndef boxm2_vecf_ocl_similarity_transform_h_included_
#define boxm2_vecf_ocl_similarity_transform_h_included_

#include <bocl/bocl_cl.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/vecf/boxm2_vecf_similarity_transform.h>

#include "boxm2_vecf_ocl_vector_field_adaptor.h"
#include "boxm2_vecf_ocl_vector_field.h"

class boxm2_vecf_ocl_similarity_transform : public boxm2_vecf_ocl_vector_field_adaptor
{
  public:
    boxm2_vecf_ocl_similarity_transform(vgl_rotation_3d<double> rot,
                                        vgl_vector_3d<double> trans,
                                        vgl_vector_3d<double> scale);


};

#endif
