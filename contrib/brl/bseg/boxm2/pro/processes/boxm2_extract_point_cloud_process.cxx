// This is brl/bseg/boxm2/pro/processes/boxm2_extract_point_cloud_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for extracting a point cloud from a boxm2 scene. The points correspond to the cell centroids of leaf cells in the scene.
//         There is a probability threshold on cells to save for convenience. The points are saved in the BOXM2_POINT datatype.
//
// \author Ali Osman Ulusoy
// \date Mar 21, 2011

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>

namespace boxm2_extract_point_cloud_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;

  typedef unsigned char uchar;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
}

bool boxm2_extract_point_cloud_process_cons (bprb_func_process& pro)
{
  using namespace boxm2_extract_point_cloud_process_globals;

  //process takes 3 inputs, no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //prob. threshold
  input_types_[3] = "unsigned"; //deptht of the tree ( 0,1,2,3)


  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(2, prob_t);
  brdb_value_sptr default_depth = new brdb_value_t<unsigned int>(3);
  pro.set_input(3, default_depth);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool boxm2_extract_point_cloud_process (bprb_func_process& pro)
{
  using namespace boxm2_extract_point_cloud_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  float prob_t = pro.get_input<float>(i++);
  unsigned int depth = pro.get_input<unsigned>(i++);


  //zip through each block
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
  {
    boxm2_block_id id = blk_iter->first;
    boxm2_block_metadata data = blk_iter->second;

    //get data from cache
    boxm2_data_base * alpha = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());

    vcl_size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    vcl_size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix());

    boxm2_data_base * points= cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix(), (alpha->buffer_length() /alphaTypeSize) * pointTypeSize, false);

    //3d array of trees
    boxm2_array_3d<uchar16>& trees = cache->get_block(id)->trees();

    boxm2_data_traits<BOXM2_ALPHA>::datatype * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha->data_buffer();
    boxm2_data_traits<BOXM2_POINT>::datatype * points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();

    for ( unsigned i = 0 ; i < (alpha->buffer_length() /alphaTypeSize) ; ++i)
    {
      points_data[i].fill(-1.0);
    }
    //iterate through each tree
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      //vcl_cout << '[' << x << '/' << trees.get_row1_count() << ']' << vcl_flush;
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
       for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
         //load current block/tree
         uchar16 tree = trees(x, y, z);
         boct_bit_tree bit_tree((unsigned char*) tree.data_block(), data.max_level_);

         //iterate through leaves of the tree
         vcl_vector<int> leafBits = bit_tree.get_leaf_bits(0,depth);
         vcl_vector<int>::iterator iter;
         for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
           int currBitIndex = (*iter);
           int currIdx = bit_tree.get_data_index(currBitIndex); //data index

           //compute probability
           int curr_depth = bit_tree.depth_at(currBitIndex);
           double side_len = 1.0 / (double) (1<<curr_depth);
           float prob = 1.0f - (float)vcl_exp(-alpha_data[currIdx] * side_len * data.sub_block_dim_.x());
           if (prob < prob_t)
           {
             points_data[currIdx][3] = -1.0f;
             continue;
           }
           vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
           vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y()+ y, localCenter.z() + z);         
           points_data[currIdx][0] = float(cellCenter.x()*data.sub_block_dim_.x() + data.local_origin_.x());
           points_data[currIdx][1] = float(cellCenter.y()*data.sub_block_dim_.y() + data.local_origin_.y());
           points_data[currIdx][2] = float(cellCenter.z()*data.sub_block_dim_.z() + data.local_origin_.z());
           points_data[currIdx][3] = 0.0f;
         }

       }
      }
    }
   
  }
  
  return true;
}

