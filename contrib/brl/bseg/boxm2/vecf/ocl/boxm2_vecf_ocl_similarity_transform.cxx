#include "boxm2_vecf_ocl_similarity_transform.h"

boxm2_vecf_ocl_similarity_transform::boxm2_vecf_ocl_similarity_transform(vgl_rotation_3d<double> rot,
                                                                         vgl_vector_3d<double> trans,
                                                                         vgl_vector_3d<double> scale)
  : cpu_xform_(rot, trans, scale)
{
}

bool boxm2_vecf_ocl_similarity_transform::
compute_forward_transform(boxm2_scene_sptr source, 
                          boxm2_block_id const& blk_id,
                          bocl_mem* blk_data,
                          cl_command_queue &queue)
{
  // cast to approprate type
  boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field_host = 
    reinterpret_cast<boxm2_data_traits<BOXM2_VEC3D>::datatype*>(blk_data->cpu_buffer());

  // compute the forward transform
  cpu_xform_.compute_forward_transform(source, blk_id, vec_field_host);

  // copy to gpu
  blk_data->write_to_buffer(queue);

  return true;
}

bool boxm2_vecf_ocl_similarity_transform::
compute_inverse_transform(boxm2_scene_sptr target,
                          boxm2_block_id const& blk_id,
                          bocl_mem* blk_data,
                          cl_command_queue &queue)
{
  // cast to approprate type
  boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field_host = 
    reinterpret_cast<boxm2_data_traits<BOXM2_VEC3D>::datatype*>(blk_data->cpu_buffer());

  // compute the forward transform
  cpu_xform_.compute_inverse_transform(target, blk_id, vec_field_host);

  // copy to gpu
  blk_data->write_to_buffer(queue);

  return true;
}
