#include "boxm2_vecf_ocl_similarity_transform.h"

boxm2_vecf_ocl_similarity_transform::boxm2_vecf_ocl_similarity_transform(vgl_rotation_3d<double> rot,
                                                                         vgl_vector_3d<double> trans,
                                                                         vgl_vector_3d<double> scale)
: boxm2_vecf_ocl_vector_field_adaptor( 
      boxm2_vecf_vector_field_sptr(new boxm2_vecf_similarity_transform(rot, trans, scale))
      )
{
}

