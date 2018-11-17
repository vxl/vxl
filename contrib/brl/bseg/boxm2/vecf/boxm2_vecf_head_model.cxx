#include "boxm2_vecf_head_model.h"
#include <algorithm>
#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <iostream>
#include <limits>
#include <utility>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_sphere_3d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

boxm2_vecf_head_model::boxm2_vecf_head_model(std::string const& scene_file,std::string color_apm_ident):
  boxm2_vecf_articulated_scene(scene_file,std::move(color_apm_ident)),
  scale_(1.0, 1.0, 1.0)
{

}

void boxm2_vecf_head_model::set_scale(vgl_vector_3d<double> scale) {
  if( (scale_-scale).length() >0.0001 ){
    scale_ = scale;
    intrinsic_change_ = true;
  }

}
bool boxm2_vecf_head_model::get_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                                     boxm2_data_base** alpha_data,
                                     boxm2_data_base** app_data,
                                     boxm2_data_base** nobs_data)
{
  *alpha_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                       boxm2_data_traits<BOXM2_ALPHA>::prefix());

  *app_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                     boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());

  *nobs_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                      boxm2_data_traits<BOXM2_NUM_OBS>::prefix());

  return true;
}

  bool get_color_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                      std::string app_id, boxm2_data_base **color_data){
    *color_data = boxm2_cache::instance()->get_data_base(scene, blk_id, std::move(app_id));
    if (color_data)
      return true;
    return false;
  }

void boxm2_vecf_head_model::map_to_target(boxm2_scene_sptr target_scene)
{
  intrinsic_change_ = false;
  // for each block of the target scene
  std::vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();
  std::vector<boxm2_block_id> source_blocks = base_model_->get_block_ids();

  for (auto & target_block : target_blocks) {

    boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene, target_block);

    boxm2_data_base *target_alpha;
    boxm2_data_base *target_app;
    boxm2_data_base *target_nobs;
    boxm2_data_base *target_color = boxm2_cache::instance()->get_data_base(target_scene, target_block, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    if (!get_data(target_scene, target_block, &target_alpha, &target_app, &target_nobs)) {
      std::cerr << "ERROR: boxm2_vecf_head_model::map_to_target(): error getting target block data block=" << target_block.to_string() << std::endl;
      return;
    }
    target_alpha->enable_write();
    target_app->enable_write();
    target_nobs->enable_write();

    //3d array of trees
    typedef boxm2_block::uchar16 uchar16;
    const boxm2_array_3d<uchar16>& trees = target_blk->trees();
    auto *target_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha->data_buffer());
    auto *target_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app->data_buffer());
    auto *target_nobs_data = reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(target_nobs->data_buffer());
    auto *target_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(target_color->data_buffer());
    // for each block of the base model
    for (auto & source_block : source_blocks) {

      boxm2_block *source_blk = boxm2_cache::instance()->get_block(base_model_, source_block);
      const boxm2_array_3d<uchar16>& source_trees = source_blk->trees();
      boxm2_data_base* source_alpha;
      boxm2_data_base* source_app;
      boxm2_data_base* source_nobs;

      boxm2_data_base* source_color = boxm2_cache::instance()->get_data_base(base_model_, source_block, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
      if (!get_data(base_model_, source_block, &source_alpha, &source_app, &source_nobs)) {
        std::cerr << "ERROR: boxm2_vecf_head_model::map_to_source(): error getting source block data block=" << source_block.to_string() << std::endl;
        return ;
      }
      auto *source_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(source_alpha->data_buffer());
      auto *source_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(source_app->data_buffer());
      auto *source_nobs_data = reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(source_nobs->data_buffer());
      auto *source_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(source_color->data_buffer());

      //iterate through each block, filtering the root level first
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
          for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
            //load current tree
            uchar16 tree = trees(x, y, z);
            boct_bit_tree bit_tree((unsigned char*) tree.data_block(), target_blk->max_level());

            //FOR ALL LEAVES IN CURRENT TREE
            std::vector<int> leafBits = bit_tree.get_leaf_bits();
            std::vector<int>::iterator iter;
            for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
              int bit_idx = (*iter);
              int data_idx = bit_tree.get_data_index(bit_idx);

              vgl_point_3d<double> cell_center_norm = bit_tree.cell_center( bit_idx );
              // convert to global coordinates
              vgl_vector_3d<double> subblk_dims = target_blk->sub_block_dim();
              vgl_vector_3d<double> subblk_offset(x*subblk_dims.x(),
                                                  y*subblk_dims.y(),
                                                  z*subblk_dims.z());

              vgl_vector_3d<double> cell_offset(cell_center_norm.x()*subblk_dims.x(),
                                                cell_center_norm.y()*subblk_dims.y(),
                                                cell_center_norm.z()*subblk_dims.z());

              vgl_point_3d<double> cell_center = target_blk->local_origin() + subblk_offset + cell_offset;

              // apply (inverse) scaling
              vgl_point_3d<double> inv_scaled_cell_center(cell_center.x() / scale_.x(),
                                                          cell_center.y() / scale_.y(),
                                                          cell_center.z() / scale_.z());

              // retreive the correct cell from the source data
              vgl_point_3d<double> local_tree_coords, source_cell_center;
              double side_len;
              if ( source_blk->contains( inv_scaled_cell_center, local_tree_coords, source_cell_center, side_len )) {
                unsigned source_data_idx;
                source_blk->data_index( inv_scaled_cell_center, source_data_idx);
                float alpha = source_alpha_data[source_data_idx];
                double prob = 1 - std::exp(-alpha*side_len);
                constexpr double prob_thresh = 0.0;

                  // get data can copy from source to target
                target_alpha_data[data_idx] = source_alpha_data[source_data_idx];
                target_app_data[data_idx]   = source_app_data[source_data_idx];
                target_nobs_data[data_idx]  = source_nobs_data[source_data_idx];
                target_color_data[data_idx] = source_color_data[source_data_idx];

                double alphas[8];
                double params[8];
                vgl_point_3d<double> abs_neighbors[8];

                vgl_vector_3d<double> source_scene_origin = vgl_vector_3d<double>(source_blk->local_origin().x(),source_blk->local_origin().y(),source_blk->local_origin().z());
                vgl_vector_3d<double> source_center_rel = vgl_vector_3d<double>(source_cell_center.x() * source_blk->sub_block_dim().x() ,
                                                                                source_cell_center.y() * source_blk->sub_block_dim().y() ,
                                                                                source_cell_center.z() * source_blk->sub_block_dim().z() );
                vgl_vector_3d<double> abs_cell_center_vec =  source_scene_origin + source_center_rel;
                vgl_point_3d<double>  abs_source_cell_center = vgl_point_3d<double>(abs_cell_center_vec.x(),abs_cell_center_vec.y(),abs_cell_center_vec.z());


                vgl_vector_3d<double> dP = inv_scaled_cell_center - abs_source_cell_center;
                //                std::cout<<inv_scaled_cell_center<<" "<<abs_cell_center_vec<<" "<<dP<<" "<<side_len /source_blk->sub_block_dim().x() <<std::endl;
                double cell_len_rw  = side_len;
                if(dP.x() >= 0){ //source point is to the right of the cell center along x
                  abs_neighbors[0].x() = abs_neighbors[1].x() =abs_neighbors[4].x() = abs_neighbors[5].x() = abs_source_cell_center.x(); // x-left neighbor is the cell center
                  abs_neighbors[2].x() = abs_neighbors[3].x() =abs_neighbors[6].x() = abs_neighbors[7].x() = abs_source_cell_center.x() +cell_len_rw; // x-right neighbor  is the cell to the right of cell center
                }else{ //source point is to the left of the cell center along x
                  abs_neighbors[0].x() = abs_neighbors[1].x() =abs_neighbors[4].x() = abs_neighbors[5].x() = abs_source_cell_center.x() - cell_len_rw; // x-left neighbor is the cell to the left of cell center
                  abs_neighbors[2].x() = abs_neighbors[3].x() =abs_neighbors[6].x() = abs_neighbors[7].x() = abs_source_cell_center.x() ; // x-right neighbor is the cell center
                }
                if(dP.y() >= 0){ //source point is to the right of the cell center along y
                  abs_neighbors[0].y() = abs_neighbors[2].y() =abs_neighbors[4].y() = abs_neighbors[6].y() = abs_source_cell_center.y(); // y-left neighbor is the cell center
                  abs_neighbors[1].y() = abs_neighbors[3].y() =abs_neighbors[5].y() = abs_neighbors[7].y() = abs_source_cell_center.y() +cell_len_rw; // y-right neighbor  is the cell to the right of cell center
                }else{ //source point is to the left of the cell center along y
                  abs_neighbors[0].y() = abs_neighbors[2].y() =abs_neighbors[4].y() = abs_neighbors[6].y() = abs_source_cell_center.y() -cell_len_rw; // y-left neighbor is the cell to the left of cell center
                  abs_neighbors[1].y() = abs_neighbors[3].y() =abs_neighbors[5].y() = abs_neighbors[7].y() = abs_source_cell_center.y(); // y-right neighbor the cell center
                }
                if(dP.z() >= 0){ //source point is above the cell center along z
                  abs_neighbors[0].z() = abs_neighbors[1].z() =abs_neighbors[2].z() = abs_neighbors[3].z() = abs_source_cell_center.z(); // z-bottom neighbor is the cell center
                  abs_neighbors[4].z() = abs_neighbors[5].z() =abs_neighbors[6].z() = abs_neighbors[7].z() = abs_source_cell_center.z() +cell_len_rw; // z-top neighbor  is the cell on top of cell center
                }else{ //source point is below the cell center along z
                  abs_neighbors[0].z() = abs_neighbors[1].z() =abs_neighbors[2].z() = abs_neighbors[3].z() = abs_source_cell_center.z() -cell_len_rw; // z-bottom neighbor is the cell below cell center
                  abs_neighbors[4].z() = abs_neighbors[5].z() =abs_neighbors[6].z() = abs_neighbors[7].z() = abs_source_cell_center.z() ; // z-top neighbor  is the  cell center
                }

                if (abs_neighbors[0].x() == abs_neighbors[2].x() || abs_neighbors[0].y() == abs_neighbors[1].y() || abs_neighbors[0].z() == abs_neighbors[4].z() )
                  continue;
                int nb_count = 0; double sum = 0;
                for (unsigned i=0; i<8; i++){

                  vgl_vector_3d<double> nbCenter = (abs_neighbors[i] - source_blk->local_origin()) / source_blk->sub_block_dim().x();

                  bool in_bounds_x = (nbCenter.x() > 0) && (nbCenter.x() < source_blk->sub_block_num().x());
                  bool in_bounds_y = (nbCenter.y() > 0) && (nbCenter.y() < source_blk->sub_block_num().y());
                  bool in_bounds_z = (nbCenter.z() > 0) && (nbCenter.z() < source_blk->sub_block_num().z());

                  bool in_bounds  = in_bounds_x && in_bounds_y && in_bounds_z;

                  if (in_bounds){
                    uchar16 nb_tree = source_trees((unsigned char) floor(nbCenter.x()),(unsigned char) floor(nbCenter.y()),(unsigned char) floor(nbCenter.z()));
                    boct_bit_tree neighbor_tree((unsigned char*) nb_tree.data_block(), source_blk->max_level());

                    //get neighbor local center, traverse to it
                    vgl_vector_3d<double> locCenter = nbCenter - vgl_vector_3d<double>(floor(nbCenter.x()),floor(nbCenter.y()),floor(nbCenter.z()));
                    int neighborBitIndex = neighbor_tree.traverse(vgl_point_3d<double>(locCenter.x(),locCenter.y(),locCenter.z()));
                    int nb_data_index = neighbor_tree.get_data_index(neighborBitIndex);
                    alphas[i] = source_alpha_data[nb_data_index] ;
                    params[i] = (double)(source_app_data[nb_data_index][0])/255;
                    nb_count++;
                  }else{
                    alphas[i] =0.0; params[i]=0;
                  }
                }
                double interped_alpha = interp_generic_double(abs_neighbors,alphas,inv_scaled_cell_center);

                double intensity = interp_generic_double(abs_neighbors,params,inv_scaled_cell_center);
                target_alpha_data[data_idx] =  (float) interped_alpha;
                target_app_data[data_idx][0] = (unsigned char)(intensity * 255);
              }
            }
          }
        }
      }
    } // for each source block
  } // for each target block
}

void boxm2_vecf_head_model::clear_target(boxm2_scene_sptr target_scene)
{
  // for each block of the target scene
  std::vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();

  for (auto & target_block : target_blocks) {

    boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene, target_block);

    boxm2_data_base *target_alpha;
    boxm2_data_base *target_app;
    boxm2_data_base *target_nobs;
    boxm2_data_base * target_color = boxm2_cache::instance()->get_data_base(target_scene, target_block, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    if (!get_data(target_scene, target_block, &target_alpha, &target_app, &target_nobs)) {
      std::cerr << "ERROR: boxm2_vecf_head_model::map_to_target(): error getting target block data block=" << target_block.to_string() << std::endl;
      return ;
    }
    target_alpha->enable_write();
    target_app->enable_write();
    target_nobs->enable_write();

    //3d array of trees
    typedef boxm2_block::uchar16 uchar16;
    const boxm2_array_3d<uchar16>& trees = target_blk->trees();
    auto *target_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha->data_buffer());
    auto *target_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app->data_buffer());
    auto *target_nobs_data = reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(target_nobs->data_buffer());
    auto *target_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(target_color->data_buffer());
    //iterate through each block, filtering the root level first
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
        for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
          //load current tree
          uchar16 tree = trees(x, y, z);
          boct_bit_tree bit_tree((unsigned char*) tree.data_block(), target_blk->max_level());

          //FOR ALL LEAVES IN CURRENT TREE
          std::vector<int> leafBits = bit_tree.get_leaf_bits();
          std::vector<int>::iterator iter;
          for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
            int bit_idx = (*iter);
            int data_idx = bit_tree.get_data_index(bit_idx);

            // get data can copy from source to target
            target_alpha_data[data_idx] = 0.0f;
            target_app_data[data_idx] = boxm2_data_traits<BOXM2_MOG3_GREY>::datatype();
            target_nobs_data[data_idx] = boxm2_data_traits<BOXM2_NUM_OBS>::datatype();
            target_color_data[data_idx] =boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype();
          }
        }
      }
    } // for each source block
  } // for each target block
}
