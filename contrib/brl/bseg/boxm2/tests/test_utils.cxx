// test utils to produce test blocks, data, scene, etc.
#include "test_utils.h"
#include <vnl/vnl_random.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

char* boxm2_test_utils::construct_block_test_stream(int numBuffers,
                                                    int treeLen,
                                                    int* nums,
                                                    double* dims,
                                                    int init_level,
                                                    int max_level,
                                                    int max_mb )
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;
    typedef unsigned short                      ushort;

    //write size, init_level, max_level, max_mb
    int numTrees = nums[0]*nums[1]*nums[2];
    long size = numTrees*(sizeof(int) + sizeof(uchar16)) +
                numBuffers*(sizeof(ushort) + sizeof(ushort2)) +
                sizeof(long) + 3*sizeof(int) + 4*sizeof(double) + 6*sizeof(int);

    //1. construct a dummy block byte stream manually
    char* bsize = new char[size];
    for (int i=0; i<size; i++) bsize[i] = (char) 0;
    int curr_byte = 0;

    //2.a write size, init_level, max_level, max_mb
    vcl_memcpy(bsize,   &size, sizeof(long));
    curr_byte += sizeof(long);
    vcl_memcpy(bsize+curr_byte, &init_level, sizeof(int));
    curr_byte += sizeof(int);
    vcl_memcpy(bsize+curr_byte, &max_level, sizeof(int));
    curr_byte += sizeof(int);
    vcl_memcpy(bsize+curr_byte, &max_mb, sizeof(int));
    curr_byte += sizeof(int);

    //2.b write dimension and buffer shape
    vcl_memcpy(bsize+curr_byte, dims, 4 * sizeof(double));
    curr_byte += 4 * sizeof(double);
    vcl_memcpy(bsize+curr_byte, nums, 4 * sizeof(int));
    curr_byte += 4 * sizeof(int);

    //3. write number of buffers
    vcl_memcpy(bsize+curr_byte, &numBuffers, sizeof(numBuffers));
    curr_byte += sizeof(numBuffers);

    //3.a write length of tree buffers
    vcl_memcpy(bsize+curr_byte, &treeLen, sizeof(treeLen));
    curr_byte += sizeof(treeLen);

    //4. put some tree values in there
    //write in the buffer some values for the trees (each tree gets a 1 as the root)
    short buff_index=0;
    short data_index=0;
    for (int i=0; i<numTrees; i++)
    {
      bsize[curr_byte + 16*i] = (unsigned char) 0;
      bsize[curr_byte + 16*i+10]=(unsigned char)(data_index>>8);
      bsize[curr_byte + 16*i+11]=(unsigned char)(data_index&255);
      bsize[curr_byte + 16*i+12]=(unsigned char)(buff_index>>8);
      bsize[curr_byte + 16*i+13]=(unsigned char)(buff_index&255);

      data_index++;
      if (data_index>=treeLen)
      {
          data_index=0;
          buff_index++;
      }
    }
    curr_byte += sizeof(uchar16)*numTrees;

    //5. 2d array of tree pointers
    int* treePtrsBuff = (int*) (bsize+curr_byte);
    for (int i=0; i<numTrees; i++)
      treePtrsBuff[i] = i;
    curr_byte += sizeof(int) * numTrees;

    //6. fill in some blocks in buffers numbers
    ushort* treeCountBuff = (ushort*) (bsize + curr_byte);
    for (int i=0; i<numBuffers; i++)
      treeCountBuff[i] = (ushort) treeLen;
    curr_byte += sizeof(ushort) * numBuffers;

    //7. 1d array of mem pointers
    ushort2* memPtrsBuff = (ushort2*) (bsize + curr_byte);
    for (int i=0; i<numBuffers; i++) {
      memPtrsBuff[i][0] = 0;
      memPtrsBuff[i][1] = treeLen+1;
    }
    curr_byte += sizeof(ushort2) * numBuffers;

    if (curr_byte != size)
      vcl_cerr<<"size "<<size<<" doesn't match offset "<<curr_byte<<'\n';

    return bsize;
}


void boxm2_test_utils::save_test_scene_to_disk()
{
  // ensure 8 test blocks and 8 data blocks are saved to disk
  vnl_random rand;
  int nums[4] = { 64, 64, 64, 0 };
  double dims[4] = { 0.5, 0.5, 0.5, 0.0 };
  int numBuffers = 64;
  int treeLen    = 64*64;
  int init_level = 1;
  int max_level  = 4;
  int max_mb     = 400;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers,
                                                                      treeLen,
                                                                      nums,
                                                                      dims,
                                                                      init_level,
                                                                      max_level,
                                                                      max_mb );
        boxm2_block_id id(i,j,k);
        boxm2_block b(id, stream);

        vcl_cout<<"saving test block for "<<id<<vcl_endl;
        boxm2_sio_mgr::save_block("", &b);
      }
    }
  }

  // save the same random data block 8 times
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;
  const unsigned int array_size = 5*1024*1024; //roughly 20 megs for alpha
  float * farray = new float[array_size];
  uchar8* carray = new uchar8[array_size];
  for (unsigned c=0; c<array_size; ++c) {
    float rnd = (float) rand.drand32(0,100);
    farray[c] = rnd;
    carray[c] = uchar8((unsigned char) rnd);
  }
  char * buffer = reinterpret_cast<char *>(farray);
  char * cbuffer = reinterpret_cast<char *>(carray);
  boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float), boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_MOG3_GREY> test_mog(cbuffer, array_size*sizeof(uchar8), boxm2_block_id(0,0,0));
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        boxm2_block_id id(boxm2_block_id(i,j,k));
        vcl_cout<<"saving alpha and mog3 test data for "<<id<<vcl_endl;
        boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>("", id, &test_data);
        boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>("", id, &test_mog);
      }
    }
  }
}

void boxm2_test_utils::delete_test_scene_from_disk(vcl_string dir)
{
  //use vul_file to
  vul_file::delete_file_glob(dir+"/"+"*id_*.bin");
}


void boxm2_test_utils::test_block_equivalence(boxm2_block& a, boxm2_block& b)
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;
    typedef unsigned short                      ushort;

#if 0 // omitting ID for sake of ASIO testing
    if (a.block_id() != b.block_id()) {
        TEST("boxm2_block: id failed", true, false);
        return;
    }
#endif
    if (a.num_buffers() != b.num_buffers()) {
        TEST("boxm2_block: num buffers failed", true, false);
        return;
    }
    if (a.tree_buff_length() != b.tree_buff_length()) {
        TEST("boxm2_block: tree_buff_length failed", true, false);
        return;
    }
    if (a.init_level() != b.init_level()) {
        TEST("boxm2_block: init_level failed", true, false);
        return;
    }
    if (a.max_level() != b.max_level()) {
        TEST("boxm2_block: max_level failed", true, false);
        return;
    }
    if (a.max_mb() != b.max_mb()) {
        TEST("boxm2_block: max_mb failed", true, false);
        return;
    }
    if (a.sub_block_dim() != b.sub_block_dim()) {
        TEST("boxm2_block: sub_block_dim failed", true, false);
        return;
    }
    if (a.sub_block_num() != b.sub_block_num()) {
        TEST("boxm2_block: sub_block_num failed", true, false);
        return;
    }
    TEST("boxm2_block: meta data from disk passed", true, true);

    boxm2_array_3d<uchar16>&  treesa = a.trees();
    boxm2_array_3d<uchar16>&  treesb = b.trees();
    for (int i=0; i<a.sub_block_num().x(); i++) {
      for (int j=0; j<a.sub_block_num().y(); j++) {
        for (int k=0; k<a.sub_block_num().z(); k++) {
          if (treesa[i][j][k] != treesb[i][j][k]) {
            TEST("boxm2_block: trees not initialized properly", true, false);
            return;
          }
        }
      }
    }
    TEST("boxm2_block: trees initialized properly", true, true);

    boxm2_array_2d<int>& treePtrsA = a.tree_ptrs();
    boxm2_array_2d<int>& treePtrsB = b.tree_ptrs();
    for (int i=0; i<a.num_buffers(); i++) {
      for (int j=0; j<a.tree_buff_length(); j++) {
        if (treePtrsA[i][j] != treePtrsB[i][j]) {
          TEST("boxm2_block: trees ptrs not initialized properly", true, false);
          return;
        }
      }
    }
    TEST("boxm2_block: tree ptrs initialized properly", true, true);

    boxm2_array_1d<ushort> b_in_ba = a.trees_in_buffers();
    boxm2_array_1d<ushort> b_in_bb = b.trees_in_buffers();
    for (int i=0; i<a.num_buffers(); i++) {
      if (b_in_ba[i] != b_in_bb[i]) {
        TEST("boxm2_block: blocks in buffers not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: blocks in buffers initialized properly", true, true);

    boxm2_array_1d<ushort2> memPtrsA = a.mem_ptrs();
    boxm2_array_1d<ushort2> memPtrsB = b.mem_ptrs();
    for (int i=0; i<a.num_buffers(); i++) {
      if (memPtrsA[i] != memPtrsB[i]) {
        TEST("boxm2_block: mem_ptrs not initialized properly", true, false);
        return;
      }
    }
    TEST("boxm2_block: mem_ptrs initialized properly", true, true);
}


vcl_string boxm2_test_utils::save_test_simple_scene()
{
    vcl_string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";
    vcl_string test_file = test_dir + "test.xml";

    vcl_map<boxm2_block_id, boxm2_block_metadata> blocks;
    for (int i=0; i<1; i++) {
        for (int j=0; j<1; j++) {
            double big_block_side = 1;
            boxm2_block_id id(i,j,0);
            boxm2_block_metadata data;
            data.id_ = id;
            data.local_origin_ = vgl_point_3d<double>(big_block_side*i, big_block_side*j, 0.0);
            data.sub_block_dim_ = vgl_vector_3d<double>(0.5, 0.5, 0.5);
            data.sub_block_num_ = vgl_vector_3d<unsigned>(2, 2, 2);
            data.init_level_ = 1;
            data.max_level_ = 4;
            data.max_mb_ = 400;
            data.p_init_ = .001;

            //push it into the map
            blocks[id] = data;
        }
    }

  //create scene
  boxm2_scene scene;
  scene.set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene.set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  bgeo_lvcs lvcs;
  scene.set_lvcs(lvcs);
  scene.set_xml_path(test_file);
  scene.set_data_path(test_dir);
  scene.set_blocks(blocks);
  scene.save_scene();

  // ensure 8 test blocks and 8 data blocks are saved to disk
  vnl_random rand;
  int nums[4] = { 2, 2, 2, 0 };
  double dims[4] = { 0.5, 0.5, 0.5, 0.0 };
  int numBuffers = 2;
  int treeLen    = 2*2;
  int init_level = 1;
  int max_level  = 4;
  int max_mb     = 400;
  for (int i=0; i<1; i++) {
    for (int j=0; j<1; j++) {
      for (int k=0; k<1; k++) {
        char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers,
                                                                      treeLen,
                                                                      nums,
                                                                      dims,
                                                                      init_level,
                                                                      max_level,
                                                                      max_mb );
        boxm2_block_id id(i,j,k);
        boxm2_block b(id, stream);

        vcl_cout<<"saving test block for "<<id<<vcl_endl;
        boxm2_sio_mgr::save_block(test_dir+"/", &b);
      }
    }
  }

  // save the same random data block 8 times
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;
  const unsigned int array_size = 8; //roughly 20 megs for alpha
  float * farray = new float[array_size];
  uchar8* carray = new uchar8[array_size];
  for (unsigned c=0; c<array_size; ++c) {
    float rnd = (float) rand.drand32(0,100);
    farray[c] = rnd;
    carray[c] = uchar8((unsigned char) rnd);
  }
  char * buffer = reinterpret_cast<char *>(farray);
  char * cbuffer = reinterpret_cast<char *>(carray);
  boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float), boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_MOG3_GREY> test_mog(cbuffer, array_size*sizeof(uchar8), boxm2_block_id(0,0,0));
  for (int i=0; i<1; i++) {
    for (int j=0; j<1; j++) {
      for (int k=0; k<1; k++) {
        boxm2_block_id id(boxm2_block_id(i,j,k));
        vcl_cout<<"saving alpha and mog3 test data for "<<id<<vcl_endl;
        boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>(test_dir+"/", id, &test_data);
        boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>(test_dir+"/", id, &test_mog);
      }
    }
  }

  return test_file;
}

