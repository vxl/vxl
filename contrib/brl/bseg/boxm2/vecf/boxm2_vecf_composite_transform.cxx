#include <iostream>
#include <stdexcept>
#include <utility>
#include "boxm2_vecf_composite_transform.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

boxm2_vecf_composite_transform::
boxm2_vecf_composite_transform(std::vector<boxm2_vecf_vector_field_base_sptr> xforms)
  : xforms_(std::move(xforms))
{
  if (xforms_.size() == 0) {
    throw std::runtime_error("boxm2_vecf_composite_transform passed an empty vector of transforms");
  }
}


bool boxm2_vecf_composite_transform::
compute_forward_transform(boxm2_scene_sptr source,
                          boxm2_block_id const& blk_id,
                          const boxm2_data_traits<BOXM2_POINT>::datatype *source_pts,
                          boxm2_data_traits<BOXM2_POINT>::datatype *target_pts)
{
  // first transform maps source pts to target pts
  bool result = xforms_[0]->compute_forward_transform(source, blk_id, source_pts, target_pts);
  // the remaining transforms take target pts as input
  for (unsigned i=1; i<xforms_.size(); ++i) {
    result &= xforms_[i]->compute_forward_transform(source, blk_id, target_pts, target_pts);
  }
  return result;
}

//: write the locations of the cooresponding source points to target's BOXM2_POINT data
bool boxm2_vecf_composite_transform::
compute_inverse_transform(boxm2_scene_sptr target,
                          boxm2_block_id const& blk_id,
                          const boxm2_data_traits<BOXM2_POINT>::datatype *target_pts,
                          boxm2_data_traits<BOXM2_POINT>::datatype *source_pts)
{
  // inverse transform applies list of inverse transforms in reverse order
  // last transform maps target pts to source pts
  bool result = xforms_.back()->compute_inverse_transform(target, blk_id, target_pts, source_pts);
  // the remaining transforms take source pts as input
  for (int i=xforms_.size()-2; i>=0; --i) {
    result &= xforms_[i]->compute_inverse_transform(target, blk_id, source_pts, source_pts);
  }
  return result;
}
