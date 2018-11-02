//:
// \file
// \author Andy Miller
// \date 26-Oct-2010
#include <vector>
#include <iostream>
#include <map>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <boxm2/io/boxm2_asio_mgr.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "test_utils.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_asio_data()
{
  // load one block from disk with blocking (like we'd imagine startup)'
  boxm2_data<BOXM2_ALPHA>* loaded = boxm2_sio_mgr::load_block_data<BOXM2_ALPHA>("",boxm2_block_id(0,0,0));
  boxm2_data<BOXM2_MOG3_GREY>* mog= boxm2_sio_mgr::load_block_data<BOXM2_MOG3_GREY>("",boxm2_block_id(0,0,0));

  // send some ASIO requests to ASIO manager
  boxm2_asio_mgr mgr;
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      for (int k=0; k<2; k++) {
        mgr.load_block_data<BOXM2_ALPHA>("", boxm2_block_id(i,j,k));
        mgr.load_block_data<BOXM2_MOG3_GREY>("", boxm2_block_id(i,j,k));
      }
    }
  }

  // check to see which block is ready, do some computation
  typedef std::map<boxm2_block_id, boxm2_data<BOXM2_ALPHA>* > alphamap_t;
  typedef std::map<boxm2_block_id, boxm2_data<BOXM2_MOG3_GREY>* > mogmap_t;
  std::vector<boxm2_data<BOXM2_MOG3_GREY>* > mog_list;
  std::vector<boxm2_data<BOXM2_ALPHA>* > alpha_list;
  int flopCount = 0;
  while (mog_list.size() < 8 || alpha_list.size() < 8)
  {
    //find out the loaded alphas
    alphamap_t lmap = mgr.get_loaded_data<BOXM2_ALPHA>();
    for (auto & iter : lmap)
      alpha_list.push_back(iter.second);

    //find out the loaded mog3s
    mogmap_t mmap = mgr.get_loaded_data<BOXM2_MOG3_GREY>();
    for (auto & iter : mmap)
      mog_list.push_back(iter.second);

    //keep track of looping
    flopCount++;
  }

  // make sure id's are unique
  std::cout<<"flop count: "<<flopCount<<std::endl;
  for (unsigned int i=0; i<mog_list.size(); ++i) {
    for (unsigned int j=0; j<mog_list.size(); ++j) {
      if (i!=j && mog_list[i]->block_id() == mog_list[j]->block_id()) {
        TEST("Non unique id's loaded in MOG3 list ", true, false);
        return;
      }
      if (i!=j && alpha_list[i]->block_id() == alpha_list[j]->block_id()) {
        TEST("Non unique id's loaded in ALPHA list ", true, false);
        return;
      }
    }
  }
  TEST("Id's are unique in both MOG and ALPHA loads ", true, true);


  //// test two of the blocks - make sure they're the same
  //boxm2_test_utils::test_data_equivalence<BOXM2_ALPHA>(*loaded, *alpha_list[3]);
  //boxm2_test_utils::test_data_equivalence<BOXM2_MOG3_GREY>(*mog, *mog_list[7]);

  // delete blocks loaded
  delete loaded;
  delete mog;
  for (auto & i : mog_list)
    if (i) delete i;
  for (auto & i : alpha_list)
    if (i) delete i;
}

static void test_asio_blocks()
{
  // load one block from disk with blocking (like we'd imagine startup)'
  std::map<boxm2_block_id,boxm2_block_metadata> mdata = boxm2_test_utils::generate_simple_metadata();

  boxm2_block* loaded = boxm2_sio_mgr::load_block("", boxm2_block_id(0,0,0),mdata[boxm2_block_id(0,0,0)]);

  // send some ASIO requests to ASIO manager
  boxm2_asio_mgr mgr;

  mgr.load_block("", boxm2_block_id(0,0,1),mdata[boxm2_block_id(0,0,0)]);
  mgr.load_block("", boxm2_block_id(0,1,0),mdata[boxm2_block_id(0,1,0)]);
  mgr.load_block("", boxm2_block_id(0,1,1),mdata[boxm2_block_id(0,1,1)]);
  mgr.load_block("", boxm2_block_id(1,0,0),mdata[boxm2_block_id(1,0,0)]);
  mgr.load_block("", boxm2_block_id(1,0,1),mdata[boxm2_block_id(1,0,1)]);
  mgr.load_block("", boxm2_block_id(1,1,0),mdata[boxm2_block_id(1,1,0)]);
  mgr.load_block("", boxm2_block_id(1,1,1),mdata[boxm2_block_id(1,1,1)]);

  // check to see which block is ready, do some computation
  std::vector<boxm2_block*> block_list;
  int flopCount = 0;
  while (block_list.size() < 6)  {
    typedef std::map<boxm2_block_id, boxm2_block*> maptype;
    maptype lmap = mgr.get_loaded_blocks();
    maptype::iterator iter;
    for (iter = lmap.begin(); iter != lmap.end(); ++iter)
      block_list.push_back(iter->second);
    flopCount++;
  }

  // report flop count and block ids
  std::cout<<"flop count: "<<flopCount<<std::endl;
  for (auto & i : block_list) {
    std::cout<<i->block_id()<<std::endl;
  }

  // test two of the blocks - make sure they're the same
  boxm2_test_utils::test_block_equivalence(*loaded, *block_list[3]);

  // delete blocks loaded
  delete loaded;
  for (auto & i : block_list)
    delete i;
}


void test_io()
{
  // ensure 8 test blocks and 8 data blocks are saved to disk
  boxm2_test_utils::save_test_scene_to_disk();

  // run some aio tests on blocks
  test_asio_blocks();
  test_asio_data();

  //delete those blocks
  boxm2_test_utils::delete_test_scene_from_disk();
}


TESTMAIN(test_io);
