// This is brl/bbas/bvgl_2d_geo_index_node_base.cxx
#include "bvgl_2d_geo_index_node_base.h"
#include <sstream>
#include <vcl_compiler.h>
#include <iostream>
#include <iomanip>

std::string bvgl_2d_geo_index_node_base::get_string() const
{
  std::stringstream str;
  str << "node_"
      << std::setprecision(8) << std::fixed << this->extent_.min_point().x() << '_'
      << std::setprecision(8) << std::fixed << this->extent_.min_point().y() << '_'
      << std::setprecision(8) << std::fixed << this->extent_.max_point().x() << '_'
      << std::setprecision(8) << std::fixed << this->extent_.max_point().y();
  return str.str();
}

std::string bvgl_2d_geo_index_node_base::get_label_name(std::string const& geo_index_name_pre, std::string const& identifier)
{
  if (identifier.compare("") == 0)
    return geo_index_name_pre + '_' + this->get_string() + "_content.bin";
  else
    return geo_index_name_pre + '_' + this->get_string() + '_' + identifier + ".bin";
}