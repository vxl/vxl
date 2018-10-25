#include <bstm/cpp/algo/bstm_analyze_coherency_function.h>
#include <bstm/cpp/algo/bstm_data_similarity_traits.h>

#include <vgl/vgl_intersection.h>

bstm_analyze_coherency_function::bstm_analyze_coherency_function(bstm_block* blk, bstm_block_metadata blk_data, bstm_time_block* blk_t, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype *apps,
                                       const bstm_data_traits<BSTM_ALPHA>::datatype * alphas, double init_local_time, double end_local_time, vgl_box_3d<double> box, float  /*p_threshold*/,
                                       std::ofstream &output_file)

{
  boxm2_array_3d<uchar16>&  trees = blk->trees();

  double time_inc = blk_data.sub_block_num_t_ / blk_data.sub_block_dim_t_ ;
  std::cout << "time inc: " << time_inc << std::endl;
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

           //compute cell boxe
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


             vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,init_local_time);
             bstm_time_tree time_tree(time_treebits.data_block(),blk_data.max_level_t_);
             int bit_index_t = time_tree.traverse(init_local_time - blk_t->tree_index(init_local_time) );
             int data_offset_t = time_tree.get_data_index(bit_index_t);
             float original_prob = 1.0f - (float)std::exp(-alphas[data_offset_t] * side_len * blk_data.sub_block_dim_.x());
             bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype original_app = apps[data_offset_t];

             for(double t = init_local_time; t <= end_local_time; t += time_inc)
             {
               //traverse the time tree
               vnl_vector_fixed<unsigned char,8> time_treebits = blk_t->get_cell_tt(data_offset,t);
               bstm_time_tree time_tree(time_treebits.data_block(),blk_data.max_level_t_);
               int bit_index_t = time_tree.traverse(t - blk_t->tree_index(t) );
               int data_offset_t = time_tree.get_data_index(bit_index_t);
               float prob = 1.0f - (float)std::exp(-alphas[data_offset_t] * side_len * blk_data.sub_block_dim_.x());
               bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype app = apps[data_offset_t];
               output_file << bstm_similarity_traits<BSTM_MOG6_VIEW_COMPACT, BOXM2_MOG6_VIEW_COMPACT>::kl_div_surf (original_prob,prob) << " ";
             }
             output_file << std::endl;
           }
        }
      }
     }
    }
  }
}
