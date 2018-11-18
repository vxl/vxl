#include "boxm2_vecf_ocl_similarity_transform.h"
#include <boxm2/vecf/boxm2_vecf_vector_field.h>

#include <utility>

boxm2_vecf_ocl_similarity_transform::boxm2_vecf_ocl_similarity_transform(vgl_rotation_3d<double> rot,
                                                                         vgl_vector_3d<double> trans,
                                                                         vgl_vector_3d<double> scale)
: boxm2_vecf_ocl_vector_field_adaptor(
      boxm2_vecf_vector_field_base_sptr(new boxm2_vecf_similarity_transform(std::move(rot), trans, scale))
      )
{
}
