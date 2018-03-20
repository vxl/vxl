#ifndef vpgl_dem_manager_h_
#define vpgl_dem_manager_h_
//:
// \file
// \brief vpgl_dem_manager provides dem services such as backproject
// \author J.L. Mundy
// \date October 25, 2016
//
//  the coordinates are based on the standard WGS84 spheroidal reference surface
//  each event object has a local vertical coordinate system
//
#include <string>
#include <vector>
#include <map>
#include <vsl/vsl_fwd.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource_sptr.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/algo/vpgl_backproject_dem.h>
class vpgl_dem_manager : public vbl_ref_count{
 public:
  vpgl_dem_manager(vil_image_resource_sptr const& resc, double zmin=0.0, double zmax=-1.0);
  bool back_project(vpgl_camera<double>* cam, double u, double v, double& x, double& y, double& z, double err_tol = 1.0);
  double zmin() const {return bproj_dem_.zmin();}
  double zmax() const {return bproj_dem_.zmax();}
 private:
  vpgl_backproject_dem bproj_dem_;
};
#endif //guard
#include "vpgl_dem_manager_sptr.h"
  //: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, vpgl_dem_manager const& dm);
void vsl_b_write(vsl_b_ostream& os, const vpgl_dem_manager* &dmp);
void vsl_b_write(vsl_b_ostream& os, vpgl_dem_manager_sptr& dm_sptr);
void vsl_b_write(vsl_b_ostream& os, vpgl_dem_manager_sptr const& dm_sptr);

//: Binary load vpgl_dem_manager site from stream.
void vsl_b_read(vsl_b_istream& is, vpgl_dem_manager &dm);
void vsl_b_read(vsl_b_istream& is, vpgl_dem_manager* dmp);
void vsl_b_read(vsl_b_istream& is, vpgl_dem_manager_sptr& dm_sptr);
void vsl_b_read(vsl_b_istream& is, vpgl_dem_manager_sptr const& dm_sptr);
