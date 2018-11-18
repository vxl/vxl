// This is core/vpgl/xio/vpgl_xio_lvcs.cxx
#include "vpgl_xio_lvcs.h"
#include <vpgl/vpgl_lvcs.h>
#include <vsl/vsl_basic_xml_element.h>

#include <utility>

//=================================================================================
void x_write(std::ostream & os, vpgl_lvcs const& v, std::string element_name)
{
  vsl_basic_xml_element xml_element(std::move(element_name));
  xml_element.add_attribute("cs_name", v.cs_name_strings[ v.get_cs_name() ]);

  double lat, lon, elev;
  v.get_origin(lat, lon, elev);
  xml_element.add_attribute("origin_lon", lon);
  xml_element.add_attribute("origin_lat", lat);
  xml_element.add_attribute("origin_elev", elev);

  double lat_scale, lon_scale;
  v.get_scale(lat_scale, lon_scale);
  xml_element.add_attribute("lon_scale", lon_scale);
  xml_element.add_attribute("lat_scale", lat_scale);

  std::string len_u = "meters", ang_u="degrees";
  if (v.local_length_unit() == v.FEET)
    len_u = "feet";
  if (v.geo_angle_unit() == v.RADIANS)
    ang_u= "radians";
  xml_element.add_attribute("local_XYZ_unit", len_u);
  xml_element.add_attribute("geo_angle_unit", ang_u);

  double lox, loy, theta;
  v.get_transform(lox, loy, theta);
  xml_element.add_attribute("local_origin_x", lox);
  xml_element.add_attribute("local_origin_y", loy);
  xml_element.add_attribute("theta", theta);
  xml_element.x_write(os);
}
