#include "bvxm_io_voxel_grid_base.h"
//:
// \file

#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <vsl/vsl_binary_io.h>

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, bvxm_voxel_grid_base const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use bvxm_voxel_grid_base as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, bvxm_voxel_grid_base & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use bvxm_voxel_grid_base as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& /*is*/, bvxm_voxel_grid_base* /*ph*/)
{
}

void vsl_b_write(vsl_b_ostream& /*os*/, const bvxm_voxel_grid_base* & /*ph*/)
{
}
