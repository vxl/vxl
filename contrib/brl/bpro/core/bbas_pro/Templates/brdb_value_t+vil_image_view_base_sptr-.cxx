#include <brdb/brdb_value.hxx>
#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_smart_ptr.h>
#include <vil/io/vil_io_image_view_base.h>

BRDB_VALUE_INSTANTIATE(vil_image_view_base_sptr, "vil_image_view_base_sptr");
void vsl_b_read(vsl_b_istream&, vil_image_view_base_sptr&) {}
void vsl_b_write(vsl_b_ostream&, vil_image_view_base_sptr const&) {}
