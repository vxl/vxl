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

void boxm2_ocl_test_utils::create_edge_scene(boxm2_scene* scene, float val1, float val2)
{
  //vcl_string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";
  vcl_string test_file =  "edge_test_scene.xml";
  
  boxm2_lru_cache::create(scene);
  
  //create block metadata
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks; 
  
  double big_block_side = 1.0; 

  //half of the blocks are empty
  for(int i=0; i<1; i++) {
    for(int j=0; j<2; j++) {
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
      
      //push it into the map
      blocks[id] = data; 
    }
  }
  
  //half of the blocks are full
  for(int i=1; i<2; i++) {
    for(int j=0; j<2; j++) {
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
  for(int i=0; i<2; i++) {
    for(int j=0; j<2; j++) {
      boxm2_block_id id(i,j,0); 
      //loads block structure and data into cahe, data is initialized
      boxm2_block *     blk     = boxm2_cache::instance()->get_block(id);
      boxm2_data_base * alpha    = boxm2_cache::instance()->get_data_base_new(id, boxm2_data_traits<BOXM2_ALPHA>::prefix());     
    }
  }  
  
  //writes the alpha and tree structure
  boxm2_cache::instance()->write_to_disk(); 
  
}


void boxm2_ocl_test_utils::print_alpha_scene(boxm2_scene* scene)
{
//iterate through blocks
vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
{
  boxm2_block_id id = blk_iter->first;
  vcl_cout << "Printing results for block: " << id << vcl_endl;
  
  boxm2_block *     blk     = boxm2_cache::instance()->get_block(id);
  boxm2_data_base * alphas    = boxm2_cache::instance()->get_data_base(id, boxm2_data_traits<BOXM2_ALPHA>::prefix());
  boxm2_block_metadata data = blk_iter->second;
  
  //3d array of trees
  boxm2_array_3d<uchar16>& trees = blk->trees();
  boxm2_data_traits<BOXM2_ALPHA>::datatype * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alphas->data_buffer();
  
  //iterate through each block, filtering the root level first
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    vcl_cout << '[' << x << '/' << trees.get_row1_count() << ']' << vcl_flush;
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
      for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
        //load current block/tree
        uchar16 tree = trees(x, y, z);
        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);
        
        //FOR ALL LEAVES IN CURRENT TREE
        vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
        vcl_vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) 
        {
          int currBitIndex = (*iter);
          int currIdx = bit_tree.get_data_index(currBitIndex);
          double side_len = 1.0 / (double) (1 << 3);
          float prob = 1.0f - vcl_exp(-alpha_data[currIdx] * side_len * data.sub_block_dim_.x());
          vcl_cout << "Leaf with index: " << currIdx << " has prob: " << prob << vcl_endl; 
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


