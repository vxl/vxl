#include <vsol/vsol_point_2d.h>
#include <vbl/io/vbl_io_smart_ptr.txx>
 
vsl_binary_loader<vsol_point_2d>* vsl_binary_loader<vsol_point_2d>::instance_ = 0;
vsl_binary_loader<vsol_point_2d>& vsl_binary_loader<vsol_point_2d>::instance() { return *instance_; }
void vsl_binary_loader<vsol_point_2d>::load_object( vsl_b_istream&, vsol_point_2d*&) {}
VBL_IO_SMART_PTR_INSTANTIATE(vsol_point_2d);
