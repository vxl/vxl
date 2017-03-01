#include "vpgl_dem_manager.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
vpgl_dem_manager::vpgl_dem_manager(vil_image_resource_sptr const& resc, double zmin, double zmax) : bproj_dem_(vpgl_backproject_dem(resc,zmin,zmax)){
  std::cout << "Success to construct vpgl_dem_manager !!" << std::endl;
}

bool vpgl_dem_manager::back_project(vpgl_camera<double>* cam, double u, double v, double& x, double& y, double& z,
                                    double err_tol){
  vgl_point_2d<double> img_pt(u, v);
  double zmax_dem = bproj_dem_.zmax(), zmin_dem = bproj_dem_.zmin();
  vgl_point_3d<double> initial_guess = bproj_dem_.geo_center();
  vgl_point_3d<double> result;
  bool good = bproj_dem_.bproj_dem(cam, img_pt, zmax_dem, zmin_dem, initial_guess, result, err_tol);
  x = 0.0; y = 0.0; z = 0.0;
  if(good){
    x = result.x(); y = result.y(); z = result.z();
  }
  return good;
}
//: Binary write boxm2_site to stream
void vsl_b_write(vsl_b_ostream& /*os*/, vpgl_dem_manager const& /*bit_site*/) {}
//: Binary write vpgl_dem_manager pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, vpgl_dem_manager* const& /*ph*/) {}
//: Binary write vpgl_dem_manager smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, vpgl_dem_manager_sptr&) {}
//: Binary write vpgl_dem_manager smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, vpgl_dem_manager_sptr const&) {}

//: Binary load boxm site from stream.
void vsl_b_read(vsl_b_istream& /*is*/, vpgl_dem_manager& /*dem_mgr*/) {}
//: Binary load boxm site pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, vpgl_dem_manager* /*dem_mgr_ptr*/) {}
//: Binary load boxm site smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, vpgl_dem_manager_sptr&) {}
//: Binary load boxm site smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, vpgl_dem_manager_sptr const&) {}
