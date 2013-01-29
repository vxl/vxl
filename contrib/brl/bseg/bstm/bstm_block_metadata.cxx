#include "bstm_block_metadata.h"
//:
// \file

#include <vcl_cmath.h>

//the local time is [0,sub_block_num_t_)
bool bstm_block_metadata::contains_t (double const t, double& local_time) const
{
  if (t >= local_origin_t_ && t < local_origin_t_ + sub_block_num_t_*sub_block_dim_t_)
  {
    local_time = (t - local_origin_t_) / sub_block_dim_t_;
    return true;
  }
  else
    return false;
}

bool bstm_block_metadata::operator==(bstm_block_metadata const& m) const
{
  return (this->id_ == m.id_) &&
          (this->local_origin_ == m.local_origin_) &&
          (this->local_origin_t_ == m.local_origin_t_) &&
          (this->sub_block_dim_ == m.sub_block_dim_) &&
          (this->sub_block_num_ == m.sub_block_num_) &&
          (this->sub_block_num_t_ == m.sub_block_num_t_) &&
          (this->sub_block_dim_t_ == m.sub_block_dim_t_);
}

bool bstm_block_metadata::operator==(boxm2_block_metadata const& m) const
{
  return (this->id_ == m.id_) &&
          (this->local_origin_ == m.local_origin_) &&
          (this->sub_block_dim_ == m.sub_block_dim_) &&
          (this->sub_block_num_ == m.sub_block_num_);
}

vgl_box_3d<double> bstm_block_metadata::bbox()
{
  //max point
  vgl_point_3d<double> max_pt( local_origin_.x() + sub_block_num_.x()*sub_block_dim_.x(),
                               local_origin_.y() + sub_block_num_.y()*sub_block_dim_.y(),
                               local_origin_.z() + sub_block_num_.z()*sub_block_dim_.z() );
  vgl_box_3d<double> box(local_origin_, max_pt);
  return box;
}
