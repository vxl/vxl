#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_storage.h"
#include "../bvxm_voxel_storage_mem.h"
#include "../bvxm_voxel_slab.h"


static void test_voxel_storage_mem()
{
  vgl_vector_3d<unsigned int> grid_size(50,50,10);
  bvxm_voxel_storage_mem<float> storage(grid_size);

  // fill with test data
  float init_val = 0.5;
  storage.initialize_data(init_val);
  bool init_check = true;
  bool write_read_check = true;

  // read in each slice, check that init_val was set, and fill with new value
  unsigned count = 0;
  for (unsigned i=0; i < storage.nz(); i++) {
    bvxm_voxel_slab<float> slab = storage.get_slab(i,1);
    bvxm_voxel_slab<float>::iterator vit;
    for (vit = slab.begin(); vit != slab.end(); vit++, count++) {
      if (*vit != init_val) {
#ifdef DEBUG
        std::cerr << "error: read in value does not match init value! slice = " << i << ", count = " << count << std::endl;
#endif
        init_check = false;
      }
      // write new value
      *vit = static_cast<float>(count);
    }
    storage.put_slab();
  }

  TEST("Initialization correctly set voxel values?",init_check,true);

  // read in each slice, check that written value is set.
  count = 0;
  for (unsigned i=0; i < storage.nz(); i++) {
    bvxm_voxel_slab<float> slab = storage.get_slab(i,1);
    bvxm_voxel_slab<float>::iterator vit;
    for (vit = slab.begin(); vit != slab.end(); vit++, count++) {
      if (*vit != static_cast<float>(count)) {
#ifdef DEBUG
        std::cerr << "error: read in value does not match written value! slice = " << i << ", count = " << count << std::endl;
#endif
        write_read_check = false;
      }
    }
  }

  TEST("Read in voxel values match written values?",write_read_check,true);
}

TESTMAIN( test_voxel_storage_mem );
