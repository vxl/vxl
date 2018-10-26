#include <cmath>
#include <bsol/bsol_algs.h>
#include "betr_gridded_geo_polygon_3d.h"
void betr_gridded_geo_polygon_3d::construct_grid(){
  grid_pts_.clear();
  grid_polys_.clear();
  // convert vsol_polygon_3d to vgl_polygon in tangent plane
  if(!so_){
    std::cout << "CONTRUCT GRID FAILS - null spatial object" << std::endl;
    return;
  }
  vsol_region_3d* reg_ptr = so_->cast_to_region();
  if(!reg_ptr){
    std::cout << "CONTRUCT GRID FAILS - spatial object not a region" << std::endl;
    return;
  }
  vsol_polygon_3d* poly_3d = reg_ptr->cast_to_polygon();
  if(!poly_3d){
    std::cout << "CONTRUCT GRID FAILS - spatial object not a polygon" << std::endl;
    return;
  }
  std::vector<vgl_point_2d<double> > verts;
  for(unsigned i = 0; i<poly_3d->size(); ++i)
    verts.emplace_back(poly_3d->vertex(i)->x(), poly_3d->vertex(i)->y());
  vgl_polygon<double>  vpoly(verts);

  vsol_box_3d_sptr bb = poly_3d->get_bounding_box();
  double x_min = bb->get_min_x(), y_min=bb->get_min_y();
  double x_max = bb->get_max_x(), y_max=bb->get_max_y();
  double z_min = bb->get_min_z();
  unsigned nv = poly_3d->size();
  // make the bounds an integral number of grid cells
  double n_cells_x = ceil((x_max-x_min)/grid_spacing_);
  double n_cells_y = ceil((y_max-y_min)/grid_spacing_);
  double delta_x = 0.5*(n_cells_x*grid_spacing_-(x_max - x_min));
  double delta_y = 0.5*(n_cells_y*grid_spacing_-(y_max - y_min));
  x_min -= delta_x; y_min -= delta_y;
  x_max += delta_x; y_max += delta_y;
  for(double y = y_min; y<=y_max; y += grid_spacing_){
    std::vector<vsol_point_3d_sptr> row_pts;
    for(double x = x_min; x<=x_max; x += grid_spacing_){
      vsol_point_3d_sptr p = new vsol_point_3d(x, y, z_min);
      row_pts.push_back(p);
    }
    grid_pts_.push_back(row_pts);
  }
  // construct grid polys
  unsigned ni = grid_pts_[0].size(), nj = grid_pts_.size();
  for(unsigned j = 1; j<nj; ++j){
    for(unsigned i = 1; i<ni; ++i){
      vsol_point_3d_sptr p00 = grid_pts_[j-1][i-1], p01 = grid_pts_[j-1][i];
      vsol_point_3d_sptr p10 = grid_pts_[j][i-1],  p11 = grid_pts_[j][i];
      //the entire grid cell must be inside the polygon
      if(!vpoly.contains(p00->x(), p00->y()))
        continue;
      if(!vpoly.contains(p01->x(), p01->y()))
        continue;
      if(!vpoly.contains(p10->x(), p10->y()))
        continue;
      if(!vpoly.contains(p11->x(), p11->y()))
        continue;
      std::vector<vsol_point_3d_sptr> verts;
      //insert in counter-clockwise order to form the cell polygon
      verts.push_back(p00); verts.push_back(p01);
      verts.push_back(p11); verts.push_back(p10);
      vsol_polygon_3d_sptr poly = new vsol_polygon_3d(verts);
      grid_polys_.push_back(poly);
    }
  }
}
