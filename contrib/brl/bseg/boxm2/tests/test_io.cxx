//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_sio_mgr.h>
#include <boxm2/boxm2_asio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vnl/vnl_random.h>

void test_asio_data()
{
  //: load one block from disk with blocking (like we'd imagine startup)'
  boxm2_data<BOXM2_ALPHA>* loaded = boxm2_sio_mgr::load_block_data<BOXM2_ALPHA>("",boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_ALPHA>* loaded2 =  boxm2_sio_mgr::load_block_data<BOXM2_ALPHA>("",boxm2_block_id(0,0,1));
  
  //: send some ASIO requests to ASIO manager
  boxm2_asio_mgr mgr; 
  mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(0,0,1));
  mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(0,1,0)); 
  mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(0,1,1)); 
  mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(1,0,1)); 
  mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(1,1,0)); 
  mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(1,1,1)); 

  //: check to see which block is ready, do some computation 
  typedef vcl_map<boxm2_block_id, boxm2_data<BOXM2_ALPHA>* > maptype;
  vcl_vector<boxm2_data<BOXM2_ALPHA>* > data_list; 
  int flopCount = 0;
  while(data_list.size() < 6)  {
    maptype lmap = mgr.get_loaded_data<BOXM2_ALPHA>(); 
    for(maptype::iterator iter = lmap.begin(); iter != lmap.end(); ++iter) 
      data_list.push_back(iter->second); 
    flopCount++; 
  }
  
  //: report flop count and block ids
  vcl_cout<<"flop count: "<<flopCount<<vcl_endl;
  for(int i=0; i<data_list.size(); ++i) {
    vcl_cout<<data_list[i]->block_id()<<vcl_endl;
  }
  
  ////: test two of the blocks - make sure they're the same
  boxm2_test_utils::test_data_equivalence<BOXM2_ALPHA>(*loaded, *loaded2); 

  //: delete blocks loaded 
  delete loaded;
  delete loaded2;
  for(int i=0; i<data_list.size(); ++i)
    if(data_list[i]) delete data_list[i];
  
  
}

static void test_asio_blocks()
{
  //: load one block from disk with blocking (like we'd imagine startup)'
  boxm2_block* loaded = boxm2_sio_mgr::load_block("", boxm2_block_id(0,0,0));
  
  //: send some ASIO requests to ASIO manager
  boxm2_asio_mgr mgr; 
  mgr.load_block("", boxm2_block_id(0,0,1));
  mgr.load_block("", boxm2_block_id(0,1,0)); 
  mgr.load_block("", boxm2_block_id(0,1,1)); 
  mgr.load_block("", boxm2_block_id(1,0,1)); 
  mgr.load_block("", boxm2_block_id(1,1,0)); 
  mgr.load_block("", boxm2_block_id(1,1,1)); 

  //: check to see which block is ready, do some computation 
  vcl_vector<boxm2_block*> block_list; 
  int flopCount = 0;
  while(block_list.size() < 6)  {
    typedef vcl_map<boxm2_block_id, boxm2_block*> maptype;
    maptype lmap = mgr.get_loaded_blocks(); 
    maptype::iterator iter;
    for(iter = lmap.begin(); iter != lmap.end(); ++iter) 
      block_list.push_back(iter->second); 
    flopCount++; 
  }
  
  //: report flop count and block ids
  vcl_cout<<"flop count: "<<flopCount<<vcl_endl;
  for(int i=0; i<block_list.size(); ++i) {
    vcl_cout<<block_list[i]->block_id()<<vcl_endl;
  }
  
  //: test two of the blocks - make sure they're the same
  boxm2_test_utils::test_block_equivalence(*loaded, *block_list[3]); 

  //: delete blocks loaded 
  delete loaded;
  for(int i=0; i<block_list.size(); ++i)
    delete block_list[i];
}


void test_io()
{
  
  //: ensure 8 test blocks and 8 data blocks are saved to disk
  vnl_random rand;
  int numBuffers = 50;
  int treeLen = 1200;
  int init_level = 1;
  int max_level  = 4;
  int max_mb     = 400;
  int nums[4] = { 30, 40, 50, 0 };
  double dims[4] = { 0.2, 0.4, 0.6, 0.0 };
  for(int i=0; i<2; i++) {
    for(int j=0; j<2; j++) {
      for(int k=0; k<2; k++) {
        char* stream = boxm2_test_utils::construct_block_test_stream( numBuffers, 
                                                                      treeLen, 
                                                                      nums, 
                                                                      dims, 
                                                                      init_level,
                                                                      max_level,
                                                                      max_mb );
        boxm2_block b(boxm2_block_id(i,j,k), stream);  
        boxm2_sio_mgr::save_block("", &b); 
      }
    }
  }
 
  //: save the same random data block 8 times
  const unsigned int array_size = 5*1024*1024; //roughly 20 megs for alpha 
  float * farray = new float[array_size]; 
  for(unsigned c=0; c<array_size; ++c) 
    farray[c] = (float) rand.drand32(0,100); 
  char * buffer = reinterpret_cast<char *>(farray); 
  boxm2_data<BOXM2_ALPHA> test_data(buffer, array_size*sizeof(float), boxm2_block_id(0,0,0));
  for(int i=0; i<2; i++) {
    for(int j=0; j<2; j++) {
      for(int k=0; k<2; k++) {
        boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>("", boxm2_block_id(i,j,k), &test_data);
      }
    }
  }
  
  
  //: run some aio tests on blocks
  //test_asio_blocks(); 
  test_asio_data(); 
}


TESTMAIN(test_io);
