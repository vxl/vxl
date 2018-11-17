#include "boxm2_vecf_ocl_vector_field_adaptor.h"

boxm2_vecf_ocl_vector_field_adaptor::boxm2_vecf_ocl_vector_field_adaptor(const boxm2_vecf_vector_field_base_sptr& cpu_xform)
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
  auto *pts_source_host =
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_source->cpu_buffer());

  auto *pts_target_host =
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_target->cpu_buffer());

  // read input from gpu to host
  pts_source->read_to_buffer(queue);

  int status = clFinish(queue);
  bool good_read = check_val(status, CL_SUCCESS, "BUFFER READ FAILED: " + error_to_string(status));
  if(!good_read) {
    std::cerr << "ERROR: boxm2_vecf_ocl_vector_field_adaptor::compute_forward_transform(): bad read of source points" << std::endl;
    return false;
  }

  // compute the forward transform
  cpu_xform_->compute_forward_transform(source, blk_id, pts_source_host, pts_target_host);

  // copy output from host to gpu
  pts_target->write_to_buffer(queue);

  status = clFinish(queue);
  bool good_write= check_val(status, CL_SUCCESS, "BUFFER WRITE FAILED: " + error_to_string(status));
  if(!good_write) {
    std::cerr << "ERROR: boxm2_vecf_ocl_vector_field_adaptor::compute_forward_transform(): bad write of target points" << std::endl;
    return false;
  }

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
  auto *pts_source_host =
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_source->cpu_buffer());

  auto *pts_target_host =
    reinterpret_cast<boxm2_data_traits<BOXM2_POINT>::datatype*>(pts_target->cpu_buffer());

  // read input from gpu to host
  pts_target->read_to_buffer(queue);

  int status = clFinish(queue);
  bool good_read = check_val(status, CL_SUCCESS, "BUFFER READ FAILED: " + error_to_string(status));
  if(!good_read) {
    std::cerr << "ERROR: boxm2_vecf_ocl_vector_field_adaptor::compute_forward_transform(): bad read of target points" << std::endl;
    return false;
  }

  // compute the forward transform
  cpu_xform_->compute_inverse_transform(target, blk_id, pts_target_host, pts_source_host);

  // copy to gpu
  pts_source->write_to_buffer(queue);

  status = clFinish(queue);
  bool good_write = check_val(status, CL_SUCCESS, "BUFFER WRITE FAILED: " + error_to_string(status));
  if(!good_write) {
    std::cerr << "ERROR: boxm2_vecf_ocl_vector_field_adaptor::compute_forward_transform(): bad write of source points" << std::endl;
    return false;
  }

  return true;
}
