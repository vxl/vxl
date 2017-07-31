#include "bstm_block_metadata.h"
#include <cmath>
#include <iostream>
//:
// \file

#include <vcl_compiler.h>

// the local time is [0,sub_block_num_t_)
bool bstm_block_metadata::contains_t(double const t, double &local_time) const {
  if (t >= local_origin_t_ &&
      t < local_origin_t_ + sub_block_num_t_ * sub_block_dim_t_) {
    local_time = (t - local_origin_t_) / sub_block_dim_t_;
    return true;
  } else
    return false;
}

bool bstm_block_metadata::operator==(bstm_block_metadata const &m) const {
  return (this->id_ == m.id_) && (this->local_origin_ == m.local_origin_) &&
         (this->local_origin_t_ == m.local_origin_t_) &&
         (this->sub_block_dim_ == m.sub_block_dim_) &&
         (this->sub_block_num_ == m.sub_block_num_) &&
         (this->sub_block_num_t_ == m.sub_block_num_t_) &&
         (this->sub_block_dim_t_ == m.sub_block_dim_t_);
}

bool bstm_block_metadata::operator==(boxm2_block_metadata const &m) const {
  return (this->id_ == m.id_) && (this->local_origin_ == m.local_origin_) &&
         (this->sub_block_dim_ == m.sub_block_dim_) &&
         (this->sub_block_num_ == m.sub_block_num_);
}

vgl_box_3d<double> bstm_block_metadata::bbox() {
  // max point
  vgl_point_3d<double> max_pt(
      local_origin_.x() + sub_block_num_.x() * sub_block_dim_.x(),
      local_origin_.y() + sub_block_num_.y() * sub_block_dim_.y(),
      local_origin_.z() + sub_block_num_.z() * sub_block_dim_.z());
  vgl_box_3d<double> box(local_origin_, max_pt);
  return box;
}

void bstm_block_metadata::to_xml(vsl_basic_xml_element &block) const {
  // add block id attribute
  block.add_attribute("id_i", id_.i());
  block.add_attribute("id_j", id_.j());
  block.add_attribute("id_k", id_.k());
  block.add_attribute("id_t", id_.t());

  // block local origin
  block.add_attribute("origin_x", local_origin_.x());
  block.add_attribute("origin_y", local_origin_.y());
  block.add_attribute("origin_z", local_origin_.z());
  block.add_attribute("origin_t", local_origin_t_);

  // sub block dimensions
  block.add_attribute("dim_x", sub_block_dim_.x());
  block.add_attribute("dim_y", sub_block_dim_.y());
  block.add_attribute("dim_z", sub_block_dim_.z());
  block.add_attribute("dim_t", sub_block_dim_t_);

  // sub block numbers
  block.add_attribute("num_x", (int)sub_block_num_.x());
  block.add_attribute("num_y", (int)sub_block_num_.y());
  block.add_attribute("num_z", (int)sub_block_num_.z());
  block.add_attribute("num_t", (int)sub_block_num_t_);

  // block init level
  block.add_attribute("init_level", init_level_);
  block.add_attribute("init_level_t", init_level_t_);

  // block max level
  block.add_attribute("max_level", max_level_);
  block.add_attribute("max_level_t", max_level_t_);

  // block max_mb
  block.add_attribute("max_mb", max_mb_);

  // block prob init
  block.add_attribute("p_init", p_init_);

  // block prob init
  block.add_attribute("random", 0);
}

vcl_ostream &operator<<(vcl_ostream &s, bstm_block_metadata &metadata) {
  s << metadata.id_ << ' ';
  vgl_point_3d<double> org = metadata.local_origin_;
  s << ", org( " << org.x() << ' ' << org.y() << ' ' << org.z() << ' '
    << metadata.local_origin_t_ << ") ";
  vgl_vector_3d<double> dim = metadata.sub_block_dim_;
  s << ", dim( " << dim.x() << ' ' << dim.y() << ' ' << dim.z() << ' '
    << metadata.sub_block_dim_t_ << ") ";
  vgl_vector_3d<unsigned> num = metadata.sub_block_num_;
  s << ", num( " << num.x() << ' ' << num.y() << ' ' << num.z() << ' '
    << metadata.sub_block_num_t_ << ")\n";
  return s;
}
