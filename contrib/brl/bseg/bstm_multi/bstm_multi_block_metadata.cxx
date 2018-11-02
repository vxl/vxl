#include <iostream>
#include <cstring>
#include "bstm_multi_block_metadata.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm_multi/bstm_multi_tree_util.h>

template <typename T> void convert(const char *t, T &d) {
  std::stringstream strm(t);
  strm >> d;
}

void append_bbox_to_xml(vsl_basic_xml_element &block,
                        const vgl_box_3d<double> &bbox) {
  block.add_attribute("min_x", bbox.min_x());
  block.add_attribute("min_y", bbox.min_y());
  block.add_attribute("min_z", bbox.min_z());

  block.add_attribute("max_x", bbox.max_x());
  block.add_attribute("max_y", bbox.max_y());
  block.add_attribute("max_z", bbox.max_z());
}

bool bstm_multi_block_metadata::contains_t(const double t,
                                           double &local_time) const {
  if (t >= bbox_t_.first && t < bbox_t_.second) {
    local_time = (t - bbox_t_.first) / (bbox_t_.second - bbox_t_.first);
    return true;
  } else
    return false;
}

std::pair<vgl_vector_3d<double>, double>
bstm_multi_block_metadata::resolution() const {
  vgl_vector_3d<double> voxel_sizes =
      this->bbox().max_point() - this->bbox().min_point();
  double time_step = bbox_t_.second - bbox_t_.first;
  for (auto subdivision : subdivisions_) {
    switch (subdivision) {
    case STE_SPACE:
      voxel_sizes /= 8.0;
      break;
    case STE_TIME:
      time_step /= 32.0;
      break;
    }
  }
  return std::pair<vgl_vector_3d<double>, double>(voxel_sizes, time_step);
}

bool bstm_multi_block_metadata::
operator==(const bstm_multi_block_metadata &m) const {
  return (this->id_ == m.id_) && (this->bbox_ == m.bbox_) &&
         (this->bbox_t_ == m.bbox_t_);
}

template <typename Metadata>
bool bstm_multi_block_metadata::operator==(Metadata const &m) const {
  return (this->id_ == m.id()) && (this->bbox_ == m.bbox());
}

void bstm_multi_block_metadata::to_xml(vsl_basic_xml_element &block) const {
  // add block id attribute
  block.add_attribute("id_i", id_.i());
  block.add_attribute("id_j", id_.j());
  block.add_attribute("id_k", id_.k());
  block.add_attribute("id_t", id_.t());
  // add block bounds
  append_bbox_to_xml(block, bbox_);
  block.add_attribute("min_t", bbox_t_.first);
  block.add_attribute("max_t", bbox_t_.second);

  // block max_mb
  block.add_attribute("max_mb", max_mb_);

  // block prob init
  block.add_attribute("p_init", p_init_);

  // block prob init
  block.add_attribute("random", 0);

  // Write subdivision information to tag. Right now each block is required to
  // be one tag, so need to fit a vector of values into a single attribute. This
  // should be fixed.
  block.add_attribute("subdivisions", print_subdivisions(subdivisions_));
}

bstm_multi_block_metadata
bstm_multi_block_metadata::from_xml(const char **atts) {
  bstm_multi_block_metadata metadata;
  int idi, idj, idk, idt;
  double min_x, min_y, min_z, min_t, max_x, max_y, max_z, max_t;
  std::string subdivs;

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
    else if (std::strcmp(atts[i], "min_x") == 0)
      convert(atts[i + 1], min_x);
    else if (std::strcmp(atts[i], "min_y") == 0)
      convert(atts[i + 1], min_y);
    else if (std::strcmp(atts[i], "min_z") == 0)
      convert(atts[i + 1], min_z);
    else if (std::strcmp(atts[i], "min_t") == 0)
      convert(atts[i + 1], min_t);
    else if (std::strcmp(atts[i], "max_x") == 0)
      convert(atts[i + 1], max_x);
    else if (std::strcmp(atts[i], "max_y") == 0)
      convert(atts[i + 1], max_y);
    else if (std::strcmp(atts[i], "max_z") == 0)
      convert(atts[i + 1], max_z);
    else if (std::strcmp(atts[i], "max_t") == 0)
      convert(atts[i + 1], max_t);
    else if (std::strcmp(atts[i], "max_mb") == 0)
      convert(atts[i + 1], metadata.max_mb_);
    else if (std::strcmp(atts[i], "p_init") == 0)
      convert(atts[i + 1], metadata.p_init_);
    else if (std::strcmp(atts[i], "subdivisions") == 0)
      convert(atts[i + 1], subdivs);
  }
  metadata.id_ = bstm_block_id(idi, idj, idk, idt);
  metadata.bbox_ = vgl_box_3d<double>(min_x, min_y, min_z, max_x, max_y, max_z);
  metadata.bbox_t_ = std::pair<double, double>(min_t, max_t);
  metadata.subdivisions_ = parse_subdivisions(subdivs);
  return metadata;
}

std::ostream &operator<<(std::ostream &s, bstm_multi_block_metadata &metadata) {
  s << metadata.id_ << ' ';
  s << ", bbox( " << metadata.bbox_ << ") ";
  s << ", bbox_t( " << metadata.bbox_t_.first << ", " << metadata.bbox_t_.second
    << ") ";
  s << "subdivs( " << print_subdivisions(metadata.subdivisions_) << " ) ";
  return s;
}

bool voxel_resolutions_match(const bstm_multi_block_metadata &metadata,
                             const boxm2_block_metadata &boxm2_metadata) {
  return metadata.resolution().first == (boxm2_metadata.sub_block_dim_ / 8.0);
}
