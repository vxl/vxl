#include <utility>

#include "boxm2_vecf_ocl_composite_transform.h"


boxm2_vecf_ocl_composite_transform::
boxm2_vecf_ocl_composite_transform(std::vector<boxm2_vecf_ocl_vector_field_sptr> xforms)
  : xforms_(std::move(xforms))
{
}

//: compute vector field, writing to gpu cache
bool boxm2_vecf_ocl_composite_transform::
compute_forward_transform(boxm2_scene_sptr source,
                          boxm2_block_id const& blk_id,
                          bocl_mem* pts_source,
                          bocl_mem* pts_target,
                          cl_command_queue &queue)
{
  // first transform maps source pts to target pts
  bool result = xforms_[0]->compute_forward_transform(source, blk_id,
                                                      pts_source, pts_target, queue);
  // the remaining transforms take target pts as input
  for (unsigned i=1; i<xforms_.size(); ++i) {
    result &= xforms_[i]->compute_forward_transform(source, blk_id,
                                                    pts_target, pts_target, queue);
  }
  return result;
}

//: compute inverse vector field, writing result to gpu cache
bool boxm2_vecf_ocl_composite_transform::
compute_inverse_transform(boxm2_scene_sptr target,
                          boxm2_block_id const& blk_id,
                          bocl_mem* pts_target,
                          bocl_mem* pts_source,
                          cl_command_queue &queue)
{
  // inverse transform applies list of inverse transforms in reverse order
  // last transform maps target pts to source pts
  bool result = xforms_.back()->compute_inverse_transform(target, blk_id,
                                                          pts_target, pts_source, queue);
  // the remaining transforms take source pts as input
  for (int i=xforms_.size()-2; i>=0; --i) {
    result &= xforms_[i]->compute_inverse_transform(target, blk_id,
                                                    pts_source, pts_source, queue);
  }
  return result;
}
