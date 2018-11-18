#include "bvxm_io_voxel_slab_base.h"
//:
// \file

#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <vsl/vsl_binary_io.h>

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, bvxm_voxel_slab_base const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use bvxm_voxel_slab_base as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, bvxm_voxel_slab_base & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use bvxm_voxel_slab_base as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, bvxm_voxel_slab_base* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new bvxm_voxel_slab<float>();  // dummy instance
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bvxm_voxel_slab_base* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
