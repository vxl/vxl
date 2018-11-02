// This is brl/bseg/bstm/pro/processes/bstm_scene_statistics_process.cxx
//:
// \file
// \brief  A process for printing out various statistics of a given scene.
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/bstm_scene.h>
#include <bstm/io/bstm_cache.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>

#include <boxm2/basic/boxm2_array_3d.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_intersection.h>
namespace bstm_scene_statistics_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 3;
}

#define MAX_CELLS_ 585

bool bstm_scene_statistics_process_cons(bprb_func_process& pro)
{
  using namespace bstm_scene_statistics_process_globals;

  //process takes 1 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "bstm_cache_sptr";
  input_types_[2] = "float"; //center x
  input_types_[3] = "float"; //center y
  input_types_[4] = "float"; //center z
  input_types_[5] = "float"; //len x
  input_types_[6] = "float"; //len y
  input_types_[7] = "float"; //len z

  std::vector<std::string> output_types(n_outputs_);
  output_types[0] = "float";
  output_types[1] = "float";
  output_types[2] = "unsigned";

  //default values for the box is empty
  brdb_value_sptr def_center_x = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_center_y = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_center_z = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_len_x = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_len_y = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_len_z = new brdb_value_t<float>(-1.0f);
  pro.set_input(2, def_center_x);
  pro.set_input(3, def_center_y);
  pro.set_input(4, def_center_z);
  pro.set_input(5, def_len_x);
  pro.set_input(6, def_len_y);
  pro.set_input(7, def_len_z);

  return pro.set_input_types(input_types_)  &&  pro.set_output_types(output_types);
}

bool bstm_scene_statistics_process(bprb_func_process& pro)
{
  using namespace bstm_scene_statistics_process_globals;

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene = pro.get_input<bstm_scene_sptr>(i++);
  bstm_cache_sptr cache = pro.get_input<bstm_cache_sptr>(i++);
  auto center_x = pro.get_input<float>(i++);
  auto center_y = pro.get_input<float>(i++);
  auto center_z = pro.get_input<float>(i++);
  auto len_x = pro.get_input<float>(i++);
  auto len_y = pro.get_input<float>(i++);
  auto len_z = pro.get_input<float>(i++);

  //create vgl box
  const vgl_point_3d<double> center(center_x,center_y,center_z);
  vgl_box_3d<double> box(center,len_x,len_y,len_z, vgl_box_3d<double>::centre);
  if(len_x <= 0.0f ||len_y <= 0.0f ||len_z <= 0.0f ) //if box is empty, set it to scene's
    box = scene->bounding_box();

  unsigned total_num_time_tree_leaf_cells = 0;
  unsigned num_time_trees = 0;
  unsigned total_time_tree_dephts = 0;
  unsigned num_cells = 0;

  //get blocks
  std::map<bstm_block_id, bstm_block_metadata> blocks = scene->blocks();
  std::map<bstm_block_id, bstm_block_metadata>::const_iterator bstm_iter = blocks.begin();
  for (; bstm_iter != blocks.end(); ++bstm_iter)
  {
    bstm_block_id bstm_id = bstm_iter->first;
    bstm_block_metadata bstm_metadata = bstm_iter->second;


    if(!vgl_intersection<double>( bstm_metadata.bbox(), box).is_empty() ) //if the two boxes intersect
    {
      bstm_block* blk = cache->get_block(bstm_id);
      bstm_time_block* blk_t = cache->get_time_block(bstm_id);

      boxm2_array_3d<uchar16>&  trees = blk->trees();
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
         for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
           uchar16 tree = trees(x, y, z); //load current block/tree
           boct_bit_tree bit_tree((unsigned char*) tree.data_block());

           //first check if the tree box is contained in the box,
           vgl_point_3d<double> tree_min_pt(bstm_metadata.local_origin_.x() + x*bstm_metadata.sub_block_dim_.x(),
                                             bstm_metadata.local_origin_.y() + y*bstm_metadata.sub_block_dim_.y(),
                                             bstm_metadata.local_origin_.z() + z*bstm_metadata.sub_block_dim_.z());
           vgl_box_3d<double> tree_box(tree_min_pt,bstm_metadata.sub_block_dim_.x(), bstm_metadata.sub_block_dim_.y(),bstm_metadata.sub_block_dim_.z(),vgl_box_3d<double>::min_pos);
           if(!vgl_intersection<double>(tree_box,box).is_empty()) //if the tree intersects the box
           {
             //iterate through leaves of the tree
             std::vector<int> leafBits = bit_tree.get_leaf_bits();
             std::vector<int>::iterator iter;

             for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
               int currBitIndex = (*iter);

               //compute cell box
               int curr_depth = bit_tree.depth_at(currBitIndex);
               double side_len = 1.0 / (double) (1<<curr_depth);

               vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
               vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);
               vgl_point_3d<double> cellCenter_global(   float(cellCenter.x()*bstm_metadata.sub_block_dim_.x() + bstm_metadata.local_origin_.x()),
                                                          float(cellCenter.y()*bstm_metadata.sub_block_dim_.y() + bstm_metadata.local_origin_.y()),
                                                          float(cellCenter.z()*bstm_metadata.sub_block_dim_.z() + bstm_metadata.local_origin_.z()));
               vgl_box_3d<double> cell_box(cellCenter_global, bstm_metadata.sub_block_dim_.x() * side_len, bstm_metadata.sub_block_dim_.y() * side_len, bstm_metadata.sub_block_dim_.z() * side_len,
                                             vgl_box_3d<double>::centre);

               //check if cell box and box intersect
               if(!vgl_intersection<double>(cell_box,box).is_empty())
               {
                 num_cells++;
                 //finally found the cell!
                 int data_offset = bit_tree.get_data_index(currBitIndex); //mdata index
                 boxm2_array_1d<vnl_vector_fixed<unsigned char, 8> >time_treebits = blk_t->get_cell_all_tt(data_offset);
                 for(auto & time_treebit : time_treebits)
                 {
                   bstm_time_tree time_tree( time_treebit.data_block(),bstm_metadata.max_level_t_);
                   total_num_time_tree_leaf_cells += time_tree.num_leaves();
                   total_time_tree_dephts += time_tree.max_depth(0);
                   num_time_trees++;
                 }
               }
            }
           }
         }
        }
      }

    }


  }

  pro.set_output_val<float>(0, total_num_time_tree_leaf_cells);
  pro.set_output_val<float>(1, float(total_time_tree_dephts)/num_time_trees);
  pro.set_output_val<unsigned>(2, num_cells);
  return true;
}
