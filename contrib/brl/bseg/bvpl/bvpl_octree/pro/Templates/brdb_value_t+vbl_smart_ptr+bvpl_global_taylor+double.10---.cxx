#include <brdb/brdb_value.hxx>

#include <vbl/io/vbl_io_smart_ptr.h>
#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

typedef vbl_smart_ptr<bvpl_global_taylor<double, 10> > bvpl_global_taylor_sptr;

BRDB_VALUE_INSTANTIATE(bvpl_global_taylor_sptr, "bvpl_global_taylor_sptr");
