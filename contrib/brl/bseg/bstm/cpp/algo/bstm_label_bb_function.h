#ifndef bstm_label_bb_function_h_
#define bstm_label_bb_function_h_


#include <iostream>
#include <map>
#include <bstm/bstm_time_block.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/io/bstm_cache.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_sphere_3d.h>
#include <bvgl/bvgl_volume_of_intersection.h>

typedef unsigned char uchar;
typedef vnl_vector_fixed<uchar, 16> uchar16;

inline void bstm_label_bb(bstm_block* blk, bstm_block_metadata blk_data, bstm_time_block* blk_t, bstm_data_traits<BSTM_LABEL>::datatype *labels,
                    bstm_data_traits<BSTM_ALPHA>::datatype * alpha, double local_time, unsigned char label, vgl_box_3d<double> box, float p_threshold)
{
  boxm2_array_3d<uchar16>&  trees = blk->trees();

  //iterate through each tree
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
     for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
       //load current block/tree
       uchar16 tree = trees(x, y, z);
       boct_bit_tree bit_tree((unsigned char*) tree.data_block());

       //first check if the tree box is contained in the box,
       vgl_point_3d<double> tree_min_pt(blk_data.local_origin_.x() + x*blk_data.sub_block_dim_.x(),
                                         blk_data.local_origin_.y() + y*blk_data.sub_block_dim_.y(),
                                         blk_data.local_origin_.z() + z*blk_data.sub_block_dim_.z());
       vgl_box_3d<double> tree_box(tree_min_pt,blk_data.sub_block_dim_.x(),
                                                blk_data.sub_block_dim_.y(),
                                                blk_data.sub_block_dim_.z(),
                                                vgl_box_3d<double>::min_pos);
       if(!vgl_intersection<double>(tree_box,box).is_empty())
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
           vgl_point_3d<double> cellCenter_global(   float(cellCenter.x()*blk_data.sub_block_dim_.x() + blk_data.local_origin_.x()),
                                                      float(cellCenter.y()*blk_data.sub_block_dim_.y() + blk_data.local_origin_.y()),
                                                      float(cellCenter.z()*blk_data.sub_block_dim_.z() + blk_data.local_origin_.z()));
           vgl_box_3d<double> cell_box(cellCenter_global, blk_data.sub_block_dim_.x() * side_len,
                                                           blk_data.sub_block_dim_.y() * side_len,
                                                           blk_data.sub_block_dim_.z() * side_len,
                                                           vgl_box_3d<double>::centre);

           //check if cell box and box intersect
           if(!vgl_intersection<double>(cell_box,box).is_empty())
           {
             //finally found the cell!
             int data_offset = bit_tree.get_data_index(currBitIndex); //mdata index
             //traverse the time tree
             vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,local_time);
             bstm_time_tree time_tree(time_treebits.data_block(),blk_data.max_level_t_);
             int bit_index_t = time_tree.traverse(local_time - blk_t->tree_index(local_time) );
             int data_offset_t = time_tree.get_data_index(bit_index_t);

             float prob = 1.0f - (float)std::exp(-alpha[data_offset_t] * side_len * blk_data.sub_block_dim_.x());
             //set the label if prob is high enough
             if(prob > p_threshold)
               labels[data_offset_t] = label;
           }
        }
      }
     }
    }
  }

}

inline void bstm_label_oriented_bb(bstm_block* blk, bstm_block_metadata blk_data, bstm_time_block* blk_t, bstm_data_traits<BSTM_LABEL>::datatype *labels,
                    bstm_data_traits<BSTM_ALPHA>::datatype * alpha, double local_time, unsigned char label, vgl_orient_box_3d<double> box, float p_threshold)
{
  std::vector<vgl_point_3d<double> > corners = box.corners();
  vgl_box_3d<double> aabb;
  for(std::vector<vgl_point_3d<double> >::const_iterator pt_iter = corners.begin(); pt_iter != corners.end(); pt_iter++)
    aabb.add(*pt_iter);


  boxm2_array_3d<uchar16>&  trees = blk->trees();

  //iterate through each tree
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
     for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
       //load current block/tree
       uchar16 tree = trees(x, y, z);
       boct_bit_tree bit_tree((unsigned char*) tree.data_block());

       //first check if the tree box is contained in the box,
       vgl_point_3d<double> tree_min_pt(blk_data.local_origin_.x() + x*blk_data.sub_block_dim_.x(),
                                         blk_data.local_origin_.y() + y*blk_data.sub_block_dim_.y(),
                                         blk_data.local_origin_.z() + z*blk_data.sub_block_dim_.z());
       vgl_box_3d<double> tree_box(tree_min_pt,blk_data.sub_block_dim_.x(),
                                                blk_data.sub_block_dim_.y(),
                                                blk_data.sub_block_dim_.z(),
                                                vgl_box_3d<double>::min_pos);



       if(!vgl_intersection<double>(tree_box,aabb ).is_empty())
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
           vgl_point_3d<double> cellCenter_global(   float(cellCenter.x()*blk_data.sub_block_dim_.x() + blk_data.local_origin_.x()),
                                                      float(cellCenter.y()*blk_data.sub_block_dim_.y() + blk_data.local_origin_.y()),
                                                      float(cellCenter.z()*blk_data.sub_block_dim_.z() + blk_data.local_origin_.z()));

           //check if box contains cell center
           if(box.contains(cellCenter_global) )
           {
             //finally found the cell!
             int data_offset = bit_tree.get_data_index(currBitIndex); //mdata index
             //traverse the time tree
             vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,local_time);
             bstm_time_tree time_tree(time_treebits.data_block(),blk_data.max_level_t_);
             int bit_index_t = time_tree.traverse(local_time - blk_t->tree_index(local_time) );
             int data_offset_t = time_tree.get_data_index(bit_index_t);

             float prob = 1.0f - (float)std::exp(-alpha[data_offset_t] * side_len * blk_data.sub_block_dim_.x());
             //set the label if prob is high enough
             if(prob >= p_threshold)
               labels[data_offset_t] = label;
           }
        }
      }
     }
    }
  }
}

inline void bstm_label_ball(bstm_block* blk, bstm_block_metadata blk_data, bstm_time_block* blk_t, bstm_data_traits<BSTM_LABEL>::datatype *labels,
                    bstm_data_traits<BSTM_ALPHA>::datatype * alpha, double local_time, unsigned char label, vgl_sphere_3d<double> sphere, float p_threshold)
{
  boxm2_array_3d<uchar16>&  trees = blk->trees();

  //iterate through each tree
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
     for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
       //load current block/tree
       uchar16 tree = trees(x, y, z);
       boct_bit_tree bit_tree((unsigned char*) tree.data_block());

       //first check if the tree box is contained in the box,
       vgl_point_3d<double> tree_min_pt(blk_data.local_origin_.x() + x*blk_data.sub_block_dim_.x(),
                                         blk_data.local_origin_.y() + y*blk_data.sub_block_dim_.y(),
                                         blk_data.local_origin_.z() + z*blk_data.sub_block_dim_.z());
       vgl_box_3d<double> tree_box(tree_min_pt,blk_data.sub_block_dim_.x(),
                                                blk_data.sub_block_dim_.y(),
                                                blk_data.sub_block_dim_.z(),
                                                vgl_box_3d<double>::min_pos);

       vgl_sphere_3d<double> tree_sphere(tree_box.centroid(), tree_box.width()/2);

       if(bvgl_volume_of_intersection(tree_sphere, sphere) )
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
           vgl_point_3d<double> cellCenter_global(   float(cellCenter.x()*blk_data.sub_block_dim_.x() + blk_data.local_origin_.x()),
                                                      float(cellCenter.y()*blk_data.sub_block_dim_.y() + blk_data.local_origin_.y()),
                                                      float(cellCenter.z()*blk_data.sub_block_dim_.z() + blk_data.local_origin_.z()));

           //check if box contains cell center
           if(sphere.contains(cellCenter_global) )
           {
             //finally found the cell!
             int data_offset = bit_tree.get_data_index(currBitIndex); //mdata index
             //traverse the time tree
             vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,local_time);
             bstm_time_tree time_tree(time_treebits.data_block(),blk_data.max_level_t_);
             int bit_index_t = time_tree.traverse(local_time - blk_t->tree_index(local_time) );
             int data_offset_t = time_tree.get_data_index(bit_index_t);

             float prob = 1.0f - (float)std::exp(-alpha[data_offset_t] * side_len * blk_data.sub_block_dim_.x());
             //set the label if prob is high enough
             if(prob > p_threshold)
               labels[data_offset_t] = label;
           }
        }
      }
     }
    }
  }
}

#endif //bstm_label_bb_function_h_
