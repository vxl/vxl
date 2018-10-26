#ifndef betr_kml_utils_h_
#define betr_kml_utils_h_
//:
// \file
// \brief convert betr structures to kml for display on GE
// \author J.L. Mundy
// \date April 30, 2016
//
#include <string>
#include <fstream>
#include "betr_geo_box_3d.h"
#include "betr_geo_object_3d.h"
class betr_kml_utils
{
 public:
  static void write_geo_box(std::ofstream& ostr, betr_geo_box_3d const& box);
  static void write_mesh(std::ofstream& ostr, betr_geo_object_3d const& geo_obj);

 private:
  betr_kml_utils() = delete;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_kml_utils.
