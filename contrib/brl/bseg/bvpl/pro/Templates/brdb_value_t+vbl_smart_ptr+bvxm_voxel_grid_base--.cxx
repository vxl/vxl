#include <brdb/brdb_value.txx>

#include <bseg/bvxm/grid/bvxm_voxel_grid_base.h>
#include <vbl/io/vbl_io_smart_ptr.h>

typedef vbl_smart_ptr<bvxm_voxel_grid_base> bvxm_voxel_grid_base_sptr;

//stubs for binary IO, since we don't need to use it
void vsl_b_read(vsl_b_istream&, bvxm_voxel_grid_base_sptr&) {}
void vsl_b_write(vsl_b_ostream&, bvxm_voxel_grid_base_sptr const&) {}

BRDB_VALUE_INSTANTIATE(bvxm_voxel_grid_base_sptr, "bvxm_voxel_grid_base_sptr");
