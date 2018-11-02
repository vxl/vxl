#include <sstream>
#include <iostream>
#include <iomanip>
#include "volm_geo_index2_node_base.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::string volm_geo_index2_node_base::get_string() const
{
  std::stringstream str;
  str << "node_"
      << std::setprecision(6) << std::fixed << this->extent_.min_point().x() << '_'
      << std::setprecision(6) << std::fixed << this->extent_.min_point().y() << '_'
      << std::setprecision(6) << std::fixed << this->extent_.max_point().x() << '_'
      << std::setprecision(6) << std::fixed << this->extent_.max_point().y();
  return str.str();
}

std::string volm_geo_index2_node_base::get_label_name(std::string const& geo_index_name_pre, std::string const& identifier)
{
  if (identifier.compare("") == 0)
    return geo_index_name_pre + '_' + this->get_string() + "_content.bin";
  else
    return geo_index_name_pre + '_' + this->get_string() + '_' + identifier + ".bin";
}
