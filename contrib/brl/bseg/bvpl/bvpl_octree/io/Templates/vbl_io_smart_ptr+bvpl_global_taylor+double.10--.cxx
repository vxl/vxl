#include <vbl/io/vbl_io_smart_ptr.hxx>
#include <bvpl_octree/io/bvpl_io_global_taylor.h>

typedef bvpl_global_taylor<double, 10> bvpl_global_taylor_double_10;

VBL_IO_SMART_PTR_INSTANTIATE(bvpl_global_taylor_double_10);
