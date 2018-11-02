// This is brl/bseg/boxm2/pro/processes/boxm2_scene_statistics_process.cxx
//:
// \file
// \brief  A process for computing out various statistics of a given scene.
//
// \author Ali Osman Ulusoy
// \date Nov 27, 2012

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_metadata.h>

#include <boxm2/basic/boxm2_array_3d.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_intersection.h>
namespace boxm2_scene_statistics_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 1;
}

#define MAX_CELLS_ 585

bool boxm2_scene_statistics_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_statistics_process_globals;

  //process takes 1 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "float"; //center x
  input_types_[3] = "float"; //center y
  input_types_[4] = "float"; //center z
  input_types_[5] = "float"; //len x
  input_types_[6] = "float"; //len y
  input_types_[7] = "float"; //len z

  std::vector<std::string> output_types(n_outputs_);
  output_types[0] = "float";

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

bool boxm2_scene_statistics_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_statistics_process_globals;

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
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

  unsigned total_num_cells = 0;

  //get blocks
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::const_iterator boxm2_iter = blocks.begin();
  for (; boxm2_iter != blocks.end(); ++boxm2_iter)
  {
    boxm2_block_id boxm2_id = boxm2_iter->first;
    boxm2_block_metadata boxm2_metadata = boxm2_iter->second;

    if(!vgl_intersection<double>( boxm2_metadata.bbox(), box).is_empty() ) //if the two boxes intersect
    {
      boxm2_block* blk = cache->get_block(scene,boxm2_id);

      const boxm2_array_3d<uchar16>&  trees = blk->trees();
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
         for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
           uchar16 tree = trees(x, y, z); //load current block/tree
           boct_bit_tree bit_tree((unsigned char*) tree.data_block());

           //first check if the tree box is contained in the box,
           vgl_point_3d<double> tree_min_pt(boxm2_metadata.local_origin_.x() + x*boxm2_metadata.sub_block_dim_.x(),
                                             boxm2_metadata.local_origin_.y() + y*boxm2_metadata.sub_block_dim_.y(),
                                             boxm2_metadata.local_origin_.z() + z*boxm2_metadata.sub_block_dim_.z());
           vgl_box_3d<double> tree_box(tree_min_pt,boxm2_metadata.sub_block_dim_.x(), boxm2_metadata.sub_block_dim_.y(),boxm2_metadata.sub_block_dim_.z(),vgl_box_3d<double>::min_pos);
           if(!vgl_intersection<double>(tree_box,box).is_empty()) //if the tree intersects the box
           {
             //iterate through leaves of the tree
             for (int j=0; j<MAX_CELLS_ ; ++j)
             {
               int pj = (j-1)>>3;           //Bit_index of parent bit
               bool validCellOld = (j==0) || bit_tree.bit_at(pj);

               if(!validCellOld)
                 continue;

               int currBitIndex = j;

               //compute cell box
               int curr_depth = bit_tree.depth_at(currBitIndex);
               double side_len = 1.0 / (double) (1<<curr_depth);

               vgl_point_3d<double> localCenter = bit_tree.cell_center(currBitIndex);
               vgl_point_3d<double> cellCenter(localCenter.x() + x, localCenter.y() + y, localCenter.z() + z);
               vgl_point_3d<double> cellCenter_global(   float(cellCenter.x()*boxm2_metadata.sub_block_dim_.x() + boxm2_metadata.local_origin_.x()),
                                                          float(cellCenter.y()*boxm2_metadata.sub_block_dim_.y() + boxm2_metadata.local_origin_.y()),
                                                          float(cellCenter.z()*boxm2_metadata.sub_block_dim_.z() + boxm2_metadata.local_origin_.z()));
               vgl_box_3d<double> cell_box(cellCenter_global, boxm2_metadata.sub_block_dim_.x() * side_len, boxm2_metadata.sub_block_dim_.y() * side_len, boxm2_metadata.sub_block_dim_.z() * side_len,
                                             vgl_box_3d<double>::centre);

               //check if cell box and box intersect
               if(!vgl_intersection<double>(cell_box,box).is_empty())
                 total_num_cells++;

            }

           }
         }
        }
      }


    }


  }

  pro.set_output_val<float>(0, total_num_cells);
  return true;
}
