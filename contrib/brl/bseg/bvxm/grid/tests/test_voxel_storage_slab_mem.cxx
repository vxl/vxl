#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_storage.h"
#include "../bvxm_voxel_storage_slab_mem.h"
#include "../bvxm_voxel_slab.h"

static void test_voxel_storage_slab_mem()
{
  vgl_vector_3d<unsigned int> grid_size(50, 50, 10);
  unsigned nslabs = grid_size.z();

  // create a storage
  bvxm_voxel_storage_slab_mem<float> storage(grid_size, nslabs);

  // fill with test data
  float init_val = 0.5;
  storage.initialize_data(init_val);
  bool init_check = true;
  bool write_read_check = true;

  // read in each slice, check that init_val was set, and fill with new value
  unsigned cnt = 0;
  for (unsigned i = 0; i < storage.nz(); i++) {
    bvxm_voxel_slab<float> slab = storage.get_slab(i,1);
    bvxm_voxel_slab<float>::iterator vit;
    for (vit = slab.begin(); vit != slab.end(); vit++, cnt++) {
      if (*vit != init_val) {
#ifdef DEBUG
        std::cerr << "error: read in value does not match init value! slice = " << i << ", count = " << cnt << std::endl;
#endif
        init_check = false;
      }
      // write new value
      *vit = static_cast<float>(cnt);
    }
    storage.put_slab();  // do nothing here
  }

  TEST("Initialization correctly set voxel values?", init_check, true);

  // read in each slice, check that the written value is set
  cnt = 0;
  for (unsigned i = 0; i < storage.nz(); i++) {
    bvxm_voxel_slab<float> slab = storage.get_slab(i, 1);
    bvxm_voxel_slab<float>::iterator vit;
    for (vit = slab.begin(); vit != slab.end(); vit++, cnt++) {
      if (*vit != static_cast<float>(cnt)) {
#ifdef DEBUG
        std::cerr << "error: read in value does not match written value! slice = " << i << ", count = " << cnt << std::endl;
#endif
        write_read_check = false;
      }
    }
  }

  TEST("Read in voxel values match written values?", write_read_check, true);
}

TESTMAIN( test_voxel_storage_slab_mem );
