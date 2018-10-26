#ifndef betr_gridded_geo_polygon_3d_h_
#define betr_gridded_geo_polygon_3d_h_
//:
// \file
// \brief a 3-d geographic polygon with defined grid
// \author J.L. Mundy
// \date January 5, 2017
//
// The polygon is, by definition, planar and assumed tangent to the Earth at the lvcs
// The grid is strictly inside the polygon. That is no vertex of a grid cell lies outside
// the defining polygon.
#include <string>
#include <vector>
#include <iostream>
#include <vbl/vbl_ref_count.h>
#include <vpgl/vpgl_lvcs.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include "betr_geo_object_3d.h"
class betr_gridded_geo_polygon_3d : public betr_geo_object_3d
{
 public:
 betr_gridded_geo_polygon_3d():grid_spacing_(10.0){}
  betr_gridded_geo_polygon_3d(const vpgl_lvcs& lvcs):betr_geo_object_3d(lvcs){}
 betr_gridded_geo_polygon_3d(vsol_spatial_object_3d_sptr so, vpgl_lvcs const& lvcs, double grid_spacing):
 betr_geo_object_3d(so, lvcs),grid_spacing_(grid_spacing){this->construct_grid();}
  ~betr_gridded_geo_polygon_3d() override= default;
  //accessors
  void set_grid_spacing(double spacing){grid_spacing_ = spacing; this->construct_grid();}
  double grid_spacing() const {return grid_spacing_;}
  const std::vector<vsol_polygon_3d_sptr>& grid_polys() const {return grid_polys_;}
  const std::vector<std::vector<vsol_point_3d_sptr> > grid_points() const {return grid_pts_;}
 protected:
  void construct_grid();
  double grid_spacing_;// in meters
  std::vector<std::vector<vsol_point_3d_sptr> > grid_pts_;
  std::vector<vsol_polygon_3d_sptr> grid_polys_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_gridded_geo_polygon_3d.
