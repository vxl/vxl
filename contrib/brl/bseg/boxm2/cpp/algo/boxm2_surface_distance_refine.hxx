#include <map>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>

template <class D>
void boxm2_surface_distance_refine(D const& dist_functor, boxm2_scene_sptr scene, std::vector<std::string> const& prefixes,
                                   double distance_thresh, bool zero_model, float occupied_prob){
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  // iterate over the scene blocks
  for (auto & block : blocks)
    {
      boxm2_block_id id = block.first;
      boxm2_block_metadata mdata = block.second;
      boxm2_block_sptr blk = boxm2_cache::instance()->get_block(scene,id);
      // multiply by 0.99 since split is triggered by alpha > max_alpha, not >=
      boxm2_refine_block_multi_data_function(scene, blk, prefixes, static_cast<float>(0.99*occupied_prob));
      // now retrieve the refined databases
      int num_cells = blk->num_cells();
      boxm2_data_base* alpha_base  = boxm2_cache::instance()->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
          alpha_base->enable_write();
            boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),num_cells *sizeof(float),alpha_base->block_id());
      //set alphas to zero for the next round
      if (zero_model) {
        for (float & alpha_it : alpha_data->data()) {
          alpha_it = 0.0f;
        }
      }
      // iterate over the trees in each block
      const boxm2_array_3d<vnl_vector_fixed<unsigned char, 16> > &trees = blk->trees();
      vgl_point_3d<double> orig = blk->local_origin();
      vgl_vector_3d<double> dims = blk->sub_block_dim();
      vgl_vector_3d<unsigned int> nums = blk->sub_block_num();
      for(unsigned iz = 0; iz<nums.z(); ++iz){
        double z = orig.z() + iz*dims.z();
        for(unsigned iy = 0; iy<nums.y(); ++iy){
          double y = orig.y() + iy*dims.y();
          for(unsigned ix = 0; ix<nums.x(); ++ix){
            double x = orig.x() + ix*dims.x();
            vgl_point_3d<double> subblock_origin(x, y, z);
            //load current block/tree
            vnl_vector_fixed<unsigned char, 16>  tree = trees(ix, iy, iz);
            boct_bit_tree bit_tree((unsigned char*) tree.data_block(), mdata.max_level_);
            //iterate through leaves of the tree
            std::vector<int> leafBits = bit_tree.get_leaf_bits();
            for (int currBitIndex : leafBits) {
              int data_offset = bit_tree.get_data_index(currBitIndex); //data index
              vgl_point_3d<double> cell_pos = bit_tree.cell_center(currBitIndex);
              vgl_vector_3d<double> cell_offset(cell_pos.x()*dims.x(), cell_pos.y()*dims.y(), cell_pos.z()*dims.z());
              vgl_point_3d<double> pos = subblock_origin + cell_offset;
              double side_len = bit_tree.cell_len(currBitIndex);
              // distance tolerance decreases with cell length
              double tol = distance_thresh*side_len*dims.x();
              bool near = dist_functor(pos)<=tol;
              if(near){//if a tree leaf is near the surface set alpha
                double alpha = -std::log(1.0 - occupied_prob)/ side_len;
                alpha_data->data()[data_offset] = static_cast<float>(alpha);
              }
            }
          }
        }
      }
//      boxm2_cache::instance()->replace_data_base(scene, id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), alpha_base);
        }
}
#undef BOXM2_SURFACE_DISTANCE_REFINE_INSTANTIATE
#define BOXM2_SURFACE_DISTANCE_REFINE_INSTANTIATE(D) \
  template void boxm2_surface_distance_refine(D const&, boxm2_scene_sptr, std::vector<std::string> const&, double , bool , float )
