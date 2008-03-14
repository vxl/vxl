#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_storage.h"
#include "../bvxm_voxel_storage_disk.h"
#include "../bvxm_voxel_slab.h"


static void test_voxel_storage_disk()
{
  START("bvxm_voxel_storage_disk test");

  // we need temporary disk storage for this test.
  vcl_string storage_fname("bvxm_voxel_storage_test_temp.vox");
  vgl_vector_3d<unsigned> grid_size(300,300,120);

  bool init_check = true;
  bool write_read_check = true;

  // create block so storage goes out of scope and filepointers close at end of tests.
  {
  
  bvxm_voxel_storage_disk<float> storage(storage_fname,grid_size);

  // fill with test data
  float init_val = 0.5;
  storage.initialize_data(init_val);

  
  // read in each slice, check that init_val was set, and fill with new value
  unsigned count = 0;
  vcl_cout << "read/write: ";
  for (unsigned i=0; i < storage.nz(); i++) {
    vcl_cout << ".";
    bvxm_voxel_slab<float> slab = storage.get_slab(i,1);
    bvxm_voxel_slab<float>::iterator vit;
    for (vit = slab.begin(); vit != slab.end(); vit++, count++) {
      if (*vit != init_val) {
        vcl_cerr << "error: read in value does not match init value! slice = " << i << ", count = " << count << vcl_endl;
        init_check = false;
      }
      // write new value
      *vit = static_cast<float>(count);
    }
    storage.put_slab();
  }
  vcl_cout << "done." << vcl_endl;
  }

  // new scope block so we get a new storage instance
  {
    bvxm_voxel_storage_disk<float> storage(storage_fname,grid_size);

  TEST("Initialization correctly set voxel values?",init_check,true);

  // read in each slice, check that written value is set.
  unsigned count = 0;
  vcl_cout << "read: ";
  for (unsigned i=0; i < storage.nz(); i++) {
    vcl_cout << ".";
    bvxm_voxel_slab<float> slab = storage.get_slab(i,1);
    bvxm_voxel_slab<float>::iterator vit;
    for (vit = slab.begin(); vit != slab.end(); vit++, count++) {
      if (*vit != static_cast<float>(count)) {
        vcl_cerr << "error: read in value does not match written value! slice = " << i << ", count = " << count << vcl_endl;
        write_read_check = false;
      }
    }
  }
  vcl_cout << "done." << vcl_endl;

  TEST("Read in voxel values match written values?",write_read_check,true);

  } // end of block, storage should go out of scope here and files should close.

  // remove temporary file
  vul_file::delete_file_glob(storage_fname.c_str());
  return;
}


      
TESTMAIN( test_voxel_storage_disk );
