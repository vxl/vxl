#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_storage.h"
#include "../bvxm_voxel_storage_disk_cached.h"
#include "../bvxm_voxel_slab.h"


static void test_voxel_storage_disk_cached()
{
  // we need temporary disk storage for this test.
  std::string storage_fname("bvxm_voxel_storage_cached_test_temp.vox");
  if (vul_file::exists(storage_fname)) // accidentally left from an earlier run
    vul_file::delete_file_glob(storage_fname);
  vgl_vector_3d<unsigned> grid_size(300,300,120);

  bool init_check = true;
  bool write_read_check = true;

  // create block so storage goes out of scope and filepointers close at end of tests.
  {
    unsigned max_cache_size = grid_size.x()*grid_size.y()*10*sizeof(float);
    bvxm_voxel_storage_disk_cached<float> storage(storage_fname,grid_size,max_cache_size);

    // fill with test data
    float init_val = 0.5f;
    storage.initialize_data(init_val);

    // read in each slice, check that init_val was set, and fill with new value
    unsigned count = 0;
    std::cout << "read/write: ";
    for (unsigned i=0; i < storage.nz(); i++) {
      std::cout << '.';
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
    std::cout << "done." << std::endl;
  }

  // new scope block so we get a new storage instance
  {
    // use a different cache size this time
    unsigned max_cache_size = grid_size.x()*grid_size.y()*sizeof(float);
    bvxm_voxel_storage_disk_cached<float> storage(storage_fname,grid_size,max_cache_size);

    TEST("Initialization correctly set voxel values?",init_check,true);

    // read in each slice, check that written value is set.
    unsigned count = 0;
    std::cout << "read: ";
    for (unsigned i=0; i < storage.nz(); i++) {
      std::cout << '.';
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
    std::cout << "done." << std::endl;

    TEST("Read in voxel values match written values?",write_read_check,true);

  } // end of block, storage should go out of scope here and files should close.

  // remove temporary file
  vul_file::delete_file_glob(storage_fname.c_str());
}

TESTMAIN( test_voxel_storage_disk_cached );
