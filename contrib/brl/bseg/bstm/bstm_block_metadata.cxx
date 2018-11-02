#include <cmath>
#include <iostream>
#include <cstring>
#include "bstm_block_metadata.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

vgl_box_3d<double> bstm_block_metadata::bbox() const {
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

template <typename T> void convert(const char *t, T &d) {
  std::stringstream strm(t);
  strm >> d;
}

bstm_block_metadata bstm_block_metadata::from_xml(const char **atts) {
  bstm_block_metadata metadata;
  int idi, idj, idk, idt;
  double ox, oy, oz, ot;
  double dim_x, dim_y, dim_z, dim_t;
  unsigned num_x, num_y, num_z, num_t;

  // iterate over attributes...
  for (int i = 0; atts[i]; i += 2) {

    if (std::strcmp(atts[i], "id_i") == 0)
      convert(atts[i + 1], idi);
    else if (std::strcmp(atts[i], "id_j") == 0)
      convert(atts[i + 1], idj);
    else if (std::strcmp(atts[i], "id_k") == 0)
      convert(atts[i + 1], idk);
    else if (std::strcmp(atts[i], "id_t") == 0)
      convert(atts[i + 1], idt);
    else if (std::strcmp(atts[i], "origin_x") == 0)
      convert(atts[i + 1], ox);
    else if (std::strcmp(atts[i], "origin_y") == 0)
      convert(atts[i + 1], oy);
    else if (std::strcmp(atts[i], "origin_z") == 0)
      convert(atts[i + 1], oz);
    else if (std::strcmp(atts[i], "origin_t") == 0)
      convert(atts[i + 1], ot);
    else if (std::strcmp(atts[i], "dim_x") == 0)
      convert(atts[i + 1], dim_x);
    else if (std::strcmp(atts[i], "dim_y") == 0)
      convert(atts[i + 1], dim_y);
    else if (std::strcmp(atts[i], "dim_z") == 0)
      convert(atts[i + 1], dim_z);
    else if (std::strcmp(atts[i], "dim_t") == 0)
      convert(atts[i + 1], dim_t);
    else if (std::strcmp(atts[i], "num_x") == 0)
      convert(atts[i + 1], num_x);
    else if (std::strcmp(atts[i], "num_y") == 0)
      convert(atts[i + 1], num_y);
    else if (std::strcmp(atts[i], "num_z") == 0)
      convert(atts[i + 1], num_z);
    else if (std::strcmp(atts[i], "num_t") == 0)
      convert(atts[i + 1], num_t);
    else if (std::strcmp(atts[i], "init_level") == 0)
      convert(atts[i + 1], metadata.init_level_);
    else if (std::strcmp(atts[i], "init_level_t") == 0)
      convert(atts[i + 1], metadata.init_level_t_);
    else if (std::strcmp(atts[i], "max_level") == 0)
      convert(atts[i + 1], metadata.max_level_);
    else if (std::strcmp(atts[i], "max_level_t") == 0)
      convert(atts[i + 1], metadata.max_level_t_);
    else if (std::strcmp(atts[i], "max_mb") == 0)
      convert(atts[i + 1], metadata.max_mb_);
    else if (std::strcmp(atts[i], "p_init") == 0)
      convert(atts[i + 1], metadata.p_init_);
  }
  metadata.id_ = bstm_block_id(idi, idj, idk, idt);
  metadata.local_origin_ = vgl_point_3d<double>(ox, oy, oz);
  metadata.local_origin_t_ = ot;
  metadata.sub_block_dim_ = vgl_vector_3d<double>(dim_x, dim_y, dim_z);
  metadata.sub_block_dim_t_ = dim_t;
  metadata.sub_block_num_ = vgl_vector_3d<unsigned>(num_x, num_y, num_z);
  metadata.sub_block_num_t_ = num_t;
  return metadata;
}

std::ostream &operator<<(std::ostream &s, const bstm_block_metadata &metadata) {
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
