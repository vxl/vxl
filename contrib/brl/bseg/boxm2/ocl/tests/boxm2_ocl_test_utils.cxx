//:
// \file
// \author Isabel Restrepo
// \date 4/16/12

#include "boxm2_ocl_test_utils.h"

#include <boct/boct_bit_tree.h>
#include <boxm2/io/boxm2_lru_cache.h>

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpl/vpl.h>

typedef vnl_vector_fixed<unsigned char, 16> uchar16;

void boxm2_ocl_test_utils::create_edge_scene(boxm2_scene_sptr scene, float val1, float val2)
{
  //std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";
  std::string test_file =  "edge_test_scene.xml";

  boxm2_lru_cache::create(scene);

  //create block metadata
  std::map<boxm2_block_id, boxm2_block_metadata> blocks;

  double big_block_side = 1.0;

  //half of the blocks are empty
  for (int i=0; i<1; i++) {
    for (int j=0; j<2; j++) {
      boxm2_block_id id(i,j,0);
      boxm2_block_metadata data;
      data.id_ = id;
      data.local_origin_ = vgl_point_3d<double>(big_block_side*i, big_block_side*j, 0.0);
      data.sub_block_dim_ = vgl_vector_3d<double>(.5, .5, .5);
      data.sub_block_num_ = vgl_vector_3d<unsigned>(2, 2, 2);
      data.init_level_ = 4;
      data.max_level_ = 4;
      data.max_mb_ = 400;
      data.p_init_ = val1;
      data.version_ = 2;

      //push it into the map
      blocks[id] = data;
    }
  }

  //half of the blocks are full
  for (int i=1; i<2; i++) {
    for (int j=0; j<2; j++) {
      boxm2_block_id id(i,j,0);
      boxm2_block_metadata data;
      data.id_ = id;
      data.local_origin_ = vgl_point_3d<double>(big_block_side*i, big_block_side*j, 0.0);
      data.sub_block_dim_ = vgl_vector_3d<double>(.5, .5, .5);
      data.sub_block_num_ = vgl_vector_3d<unsigned>(2, 2, 2);
      data.init_level_ = 4;
      data.max_level_ = 4;
      data.max_mb_ = 400;
      data.p_init_ = val2;
      data.version_ = 2;

      //push it into the map
      blocks[id] = data;
    }
  }

  //create scene
  scene->set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene->set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  vpgl_lvcs lvcs;
  scene->set_lvcs(lvcs);
  scene->set_xml_path(test_file);
  scene->set_data_path(".");
  scene->set_blocks(blocks);
  scene->save_scene();

  //load all blocks in cpu cache
  for (int i=0; i<2; i++) {
    for (int j=0; j<2; j++) {
      boxm2_block_id id(i,j,0);
      //loads block structure and data into cahe, data is initialized
      /* boxm2_block *  blk      = */ boxm2_cache::instance()->get_block(scene,id);
      /* boxm2_data_base * alpha = */ boxm2_cache::instance()->get_data_base_new(scene,id, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    }
  }

  //writes the alpha and tree structure
  boxm2_cache::instance()->write_to_disk();
}
bool boxm2_ocl_test_utils::create_test_simple_scene(boxm2_scene_sptr & scene)
{
  std::map<boxm2_block_id, boxm2_block_metadata> blocks;
  for (int i=0; i<1; i++) {
      for (int j=0; j<1; j++) {
          double big_block_side = 1;
          boxm2_block_id id(i,j,0);
          boxm2_block_metadata data;
          data.id_ = id;
          data.local_origin_ = vgl_point_3d<double>(big_block_side*i, big_block_side*j, 0.0);
          data.sub_block_dim_ = vgl_vector_3d<double>(0.5, 0.5, 0.5);
          data.sub_block_num_ = vgl_vector_3d<unsigned>(2, 2, 1);
          data.init_level_ = 1;
          data.max_level_ = 4;
          data.max_mb_ = 400;
          data.p_init_ = .001;
          data.version_ = 2;
          //push it into the map
          blocks[id] = data;
      }
  }
  scene = new boxm2_scene();
  scene->set_local_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  scene->set_rpc_origin(vgl_point_3d<double>(0.0, 0.0, 0.0));
  vpgl_lvcs lvcs;
  scene->set_lvcs(lvcs);
  scene->set_xml_path("./scene.xml");
  scene->set_data_path(".");
  scene->set_blocks(blocks);
  scene->set_version(2);
  return true;
}

void boxm2_ocl_test_utils::print_alpha_scene(boxm2_scene_sptr scene)
{
  //iterate through blocks
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    std::cout << "Printing results for block: " << id << std::endl;

    boxm2_block *     blk     = boxm2_cache::instance()->get_block(scene,id);
    boxm2_data_base * alphas    = boxm2_cache::instance()->get_data_base(scene,id, boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_block_metadata data = blk_iter->second;

    //3d array of trees
    const boxm2_array_3d<uchar16>& trees = blk->trees();
    auto * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alphas->data_buffer();

    //iterate through each block, filtering the root level first
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      std::cout << '[' << x << '/' << trees.get_row1_count() << ']' << std::flush;
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
        for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
          //load current block/tree
          uchar16 tree = trees(x, y, z);
          boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

          //FOR ALL LEAVES IN CURRENT TREE
          std::vector<int> leafBits = bit_tree.get_leaf_bits();
          std::vector<int>::iterator iter;
          for (iter = leafBits.begin(); iter != leafBits.end(); ++iter)
          {
            int currBitIndex = (*iter);
            int currIdx = bit_tree.get_data_index(currBitIndex);
            double side_len = 1.0 / (double) (1 << 3);
            //float prob = 1.0f - std::exp(-alpha_data[currIdx] * side_len * data.sub_block_dim_.x());
            //std::cout << "Leaf with index: " << currIdx << " has prob: " << prob << std::endl;
          }
        }
      }
    }
  }
}

void boxm2_ocl_test_utils::clean_up()
{
  //clean temporary files
  vul_file_iterator file_it("./*.bin");
  for (; file_it; ++file_it)
  {
    vpl_unlink(file_it());
    vul_file::delete_file_glob(file_it());
  }
  vul_file_iterator s_file_it("./*scene.xml");
  for (; s_file_it; ++s_file_it)
  {
    vpl_unlink(s_file_it());
    vul_file::delete_file_glob(s_file_it());
  }
}
char* boxm2_ocl_test_utils::construct_block_test_stream(int  /*numBuffers*/,
                                                    int treeLen,
                                                    const int* nums,
                                                    double*  /*dims*/,
                                                    int  /*init_level*/,
                                                    int  /*max_level*/,
                                                    int  /*max_mb*/ )
{
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;

    //write size, init_level, max_level, max_mb
    int numTrees = nums[0]*nums[1]*nums[2];
    long size = numTrees*(sizeof(uchar16));

    //1. construct a dummy block byte stream manually
    char* bsize = new char[size];
    for (int i=0; i<size; i++) bsize[i] = (char) 0;
    int curr_byte = 0;


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

    if (curr_byte != size)
      std::cerr<<"size "<<size<<" doesn't match offset "<<curr_byte<<'\n';

    return bsize;
}

std::string boxm2_ocl_test_utils::save_test_simple_scene(const std::string& filename )
{
    std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/ocl/tests/";
    std::string test_file = test_dir + filename;

    std::map<boxm2_block_id, boxm2_block_metadata> blocks;
    for (int i=0; i<1; i++) {
        for (int j=0; j<1; j++) {
            double big_block_side = 1;
            boxm2_block_id id(i,j,0);
            boxm2_block_metadata data;
            data.id_ = id;
            data.local_origin_ = vgl_point_3d<double>(big_block_side*i, big_block_side*j, 0.0);
            data.sub_block_dim_ = vgl_vector_3d<double>(0.5, 0.5, 0.5);
            data.sub_block_num_ = vgl_vector_3d<unsigned>(2, 2, 1);
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
  vpgl_lvcs lvcs;
  scene.set_lvcs(lvcs);
  scene.set_xml_path(test_file);
  scene.set_data_path(test_dir);
  scene.set_blocks(blocks);
  scene.set_version(2);
  scene.save_scene();

  // ensure 8 test blocks and 8 data blocks are saved to disk
  vnl_random rand;
  int nums[4] = { 2, 2, 1, 0 };
  double dims[4] = { 0.5, 0.5, 0.5, 0.0 };
  int numBuffers = 1;
  int treeLen    = 2*2*1;
  int init_level = 1;
  int max_level  = 4;
  int max_mb     = 400;
  for (int i=0; i<1; i++) {
    for (int j=0; j<1; j++) {
      for (int k=0; k<1; k++) {
        char* stream = boxm2_ocl_test_utils::construct_block_test_stream( numBuffers,
                                                                      treeLen,
                                                                      nums,
                                                                      dims,
                                                                      init_level,
                                                                      max_level,
                                                                      max_mb );
        boxm2_block_id id(i,j,k);
        boxm2_block b(id, stream);

        std::cout<<"saving test block for "<<id<<std::endl;
        boxm2_sio_mgr::save_block(test_dir+"/", &b);
      }
    }
  }

  // save the same random data block 8 times
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;
  constexpr unsigned int array_size = 4; //roughly 20 megs for alpha
  auto * farray = new float[array_size];
  auto* carray = new uchar8[array_size];
  for (unsigned c=0; c<array_size; ++c) {
    auto rnd = (float) rand.drand32(0,100);
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
        std::cout<<"saving alpha and mog3 test data for "<<id<<std::endl;
        boxm2_sio_mgr::save_block_data<BOXM2_ALPHA>(test_dir+"/", id, &test_data);
        boxm2_sio_mgr::save_block_data<BOXM2_MOG3_GREY>(test_dir+"/", id, &test_mog);
      }
    }
  }

  return test_file;
}
