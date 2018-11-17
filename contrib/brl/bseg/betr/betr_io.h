#ifndef betr_io_h_
#define betr_io_h_
//:
// \file
// \brief miscellaeous io operations
// \author J.L. Mundy
// \date June 27, 2016
//
#include <string>
#include <fstream>
#include "betr_geo_box_3d.h"
#include "betr_geo_object_3d.h"
#include "vsol_mesh_3d.h"
#include <vsol/vsol_spatial_object_3d.h>
class betr_io
{
 public:
  static bool read_so_ply(const std::string& ply_path, vsol_spatial_object_3d_sptr& obj);
 private:
  betr_io() = delete;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_io.
