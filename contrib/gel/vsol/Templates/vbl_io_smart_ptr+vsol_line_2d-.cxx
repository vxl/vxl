#include <vsol/vsol_line_2d.h>
#include <vbl/io/vbl_io_smart_ptr.txx>
 
vsl_binary_loader<vsol_line_2d>* vsl_binary_loader<vsol_line_2d>::instance_ = 0;
vsl_binary_loader<vsol_line_2d>& vsl_binary_loader<vsol_line_2d>::instance() { return *instance_; }
void vsl_binary_loader<vsol_line_2d>::load_object( vsl_b_istream&, vsol_line_2d*&) {}
VBL_IO_SMART_PTR_INSTANTIATE(vsol_line_2d);
