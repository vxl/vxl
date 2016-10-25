#include "vpgl_dem_manager.h"
vpgl_dem_manager::vpgl_dem_manager(vil_image_resource_sptr const& resc) : bproj_dem_(vpgl_backproject_dem(resc)){
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
