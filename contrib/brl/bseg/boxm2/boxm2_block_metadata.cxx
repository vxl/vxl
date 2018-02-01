#include "boxm2_block_metadata.h"
//:
// \file
#include <boxm2/boxm2_data_traits.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

vgl_box_3d<double> boxm2_block_metadata::bbox() const {
  // max point
  vgl_point_3d<double> max_pt(
      local_origin_.x() + sub_block_num_.x() * sub_block_dim_.x(),
      local_origin_.y() + sub_block_num_.y() * sub_block_dim_.y(),
      local_origin_.z() + sub_block_num_.z() * sub_block_dim_.z());
  vgl_box_3d<double> box(local_origin_, max_pt);
  return box;
}
