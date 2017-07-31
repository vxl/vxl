#include "bstm_multi_block_metadata.h"

vcl_string ste_as_string(space_time_enum ste) {
  switch (ste) {
  case STE_SPACE:
    return "space";
  case STE_TIME:
    return "time";
  default:
    return "INVALID STE";
  }
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

bool bstm_multi_block_metadata::
operator==(const bstm_multi_block_metadata &m) const {
  return (this->id_ == m.id_) && (this->bbox_ == m.bbox_);
}

bool bstm_multi_block_metadata::
operator==(const boxm2_block_metadata &m) const {
  return (this->id_ == m.id_) && (this->bbox_ == m.bbox());
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
  vcl_string subdivs_attr;
  for (vcl_vector<space_time_enum>::const_iterator iter = subdivisions_.begin();
       iter != subdivisions_.end();
       ++iter) {
    if (iter != subdivisions_.begin()) {
      subdivs_attr += ",";
    }
    subdivs_attr += ste_as_string(*iter);
  }
  block.add_attribute("subdivisions", subdivs_attr);
}
