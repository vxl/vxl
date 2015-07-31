#include "boxm2_vecf_ocl_vector_field_adaptor.h"

boxm2_vecf_ocl_vector_field_adaptor::boxm2_vecf_ocl_vector_field_adaptor(boxm2_vecf_vector_field_base_sptr cpu_xform)
  : cpu_xform_(cpu_xform)
{
}

bool boxm2_vecf_ocl_vector_field_adaptor::
compute_forward_transform(boxm2_scene_sptr source, 
                          boxm2_block_id const& blk_id,
                          bocl_mem* pts_source,
                          bocl_mem* pts_target,
                          cl_command_queue &queue)
{
  // cast to approprate type
  boxm2_data_traits<BOXM2_POINT>::datatype *pts_source_host = 
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_source->cpu_buffer());

  boxm2_data_traits<BOXM2_POINT>::datatype *pts_target_host = 
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_target->cpu_buffer());

  // read input from gpu to host
  pts_source->read_to_buffer(queue);

  // compute the forward transform
  cpu_xform_->compute_forward_transform(source, blk_id, pts_source_host, pts_target_host);

  // copy output from host to gpu
  pts_target->write_to_buffer(queue);

  return true;
}

bool boxm2_vecf_ocl_vector_field_adaptor::
compute_inverse_transform(boxm2_scene_sptr target,
                          boxm2_block_id const& blk_id,
                          bocl_mem* pts_target,
                          bocl_mem* pts_source,
                          cl_command_queue &queue)
{
  // cast to approprate type
  boxm2_data_traits<BOXM2_POINT>::datatype *pts_source_host = 
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_source->cpu_buffer());

  boxm2_data_traits<BOXM2_POINT>::datatype *pts_target_host = 
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_target->cpu_buffer());

  // read input from gpu to host
  pts_target->read_to_buffer(queue);

  // compute the forward transform
  cpu_xform_->compute_inverse_transform(target, blk_id, pts_target_host, pts_source_host);

  // copy to gpu
  pts_source->write_to_buffer(queue);

  return true;
}
