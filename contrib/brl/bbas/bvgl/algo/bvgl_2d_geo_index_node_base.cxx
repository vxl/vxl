// This is brl/bbas/bvgl_2d_geo_index_node_base.cxx
#include "bvgl_2d_geo_index_node_base.h"
#include <vcl_sstream.h>
#include <vcl_iomanip.h>

vcl_string bvgl_2d_geo_index_node_base::get_string() const
{
  vcl_stringstream str;
  str << "node_"
      << vcl_setprecision(6) << vcl_fixed << this->extent_.min_point().x() << '_'
      << vcl_setprecision(6) << vcl_fixed << this->extent_.min_point().y() << '_'
      << vcl_setprecision(6) << vcl_fixed << this->extent_.max_point().x() << '_'
      << vcl_setprecision(6) << vcl_fixed << this->extent_.max_point().y();
  return str.str();
}

vcl_string bvgl_2d_geo_index_node_base::get_label_name(vcl_string const& geo_index_name_pre, vcl_string const& identifier)
{
  if (identifier.compare("") == 0)
    return geo_index_name_pre + '_' + this->get_string() + "_content.bin";
  else
    return geo_index_name_pre + '_' + this->get_string() + '_' + identifier + ".bin";
}