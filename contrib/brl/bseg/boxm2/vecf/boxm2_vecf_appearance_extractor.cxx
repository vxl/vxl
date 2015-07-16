#include "boxm2_vecf_appearance_extractor.h"
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include "boxm2_vecf_composite_head_parameters.h"
void boxm2_vecf_appearance_extractor::extract_head_appearance(){
  boxm2_scene_sptr base_model = source_model_.scene();
  // for each block of the target scene
  boxm2_vecf_composite_head_parameters const& head_params = source_model_.get_params();
  vcl_vector<boxm2_block_id> target_blocks = target_scene_->get_block_ids();
  vcl_vector<boxm2_block_id> source_blocks = base_model->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin();
       sblk != source_blocks.end(); ++sblk) {

    boxm2_block *source_blk = boxm2_cache::instance()->get_block(base_model, *sblk);
    boxm2_data_base* source_app = boxm2_cache::instance()->get_data_base(base_model, *sblk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    boxm2_data_base* source_alpha = boxm2_cache::instance()->get_data_base(base_model, *sblk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data_base* source_color = boxm2_cache::instance()->get_data_base(base_model, *sblk, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(source_model_.color_apm_id_));

    source_app->enable_write();
    boxm2_vecf_orbit_scene & lorbit = source_model_.left_orbit_;
    //3d array of trees
    typedef boxm2_block::uchar16 uchar16;
    const boxm2_array_3d<uchar16>& trees = source_blk->trees();
    boxm2_data_traits<BOXM2_MOG3_GREY>::datatype *source_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(source_app->data_buffer());
    boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype *source_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(source_color->data_buffer());
    boxm2_data_traits<BOXM2_ALPHA>::datatype *source_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(source_alpha->data_buffer());
    // for each block of the base model
    for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin(); tblk != target_blocks.end(); ++tblk) {
      boxm2_block *target_blk = boxm2_cache::instance()->get_block(base_model, *tblk);
      boxm2_data_base* target_app = boxm2_cache::instance()->get_data_base(base_model, *tblk , boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
      boxm2_data_base* target_color = boxm2_cache::instance()->get_data_base(target_scene_, *tblk, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(source_model_.color_apm_id_));
      boxm2_data_base* target_alpha = boxm2_cache::instance()->get_data_base(base_model, *tblk,boxm2_data_traits<BOXM2_ALPHA>::prefix());

      boxm2_data_traits<BOXM2_MOG3_GREY>::datatype *target_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app->data_buffer());
      boxm2_data_traits<BOXM2_ALPHA>::datatype *target_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha->data_buffer());
      boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype *target_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(target_color->data_buffer());

      //iterate through each block, filtering the root level first
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
          for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
            //load current tree
            uchar16 tree = trees(x, y, z);
            boct_bit_tree bit_tree((unsigned char*) tree.data_block(), source_blk->max_level());

            //FOR ALL LEAVES IN CURRENT TREE
            vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
            vcl_vector<int>::iterator iter;
            for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
              int bit_idx = (*iter);
              int data_idx = bit_tree.get_data_index(bit_idx);

              vgl_point_3d<double> cell_center_norm = bit_tree.cell_center( bit_idx );
              double source_side_len = 1.0/(1 << bit_tree.depth_at(bit_idx));
              // convert to global coordinates
              vgl_vector_3d<double> subblk_dims = source_blk->sub_block_dim();
              vgl_vector_3d<double> subblk_offset(x*subblk_dims.x(),
                                                  y*subblk_dims.y(),
                                                  z*subblk_dims.z());

              vgl_vector_3d<double> cell_offset(cell_center_norm.x()*subblk_dims.x(),
                                                cell_center_norm.y()*subblk_dims.y(),
                                                cell_center_norm.z()*subblk_dims.z());

              vgl_point_3d<double> cell_center = source_blk->local_origin() + subblk_offset + cell_offset;

              // apply (inverse) scaling
              vgl_point_3d<double> fwd_scaled_cell_center(cell_center.x() * head_params.head_scale_.x(),
                                                          cell_center.y() * head_params.head_scale_.y(),
                                                          cell_center.z() * head_params.head_scale_.z());

              // retreive the correct cell from the source data
              vgl_point_3d<double> local_tree_coords, target_cell_center;
              double side_len;

              if ( target_blk->contains( fwd_scaled_cell_center, local_tree_coords, target_cell_center, side_len )) {

                unsigned target_data_idx;
                target_blk->data_index( fwd_scaled_cell_center, target_data_idx);
                float alpha = target_alpha_data[target_data_idx];
                float src_prob = 1 - vcl_exp( - source_alpha_data[data_idx] * source_side_len);
                float prob = 1 - vcl_exp(-alpha*side_len);
                const float prob_thresh = 0.00;
                if (src_prob >= prob_thresh) {
                  // get data can copy from source to target
                  source_app_data[data_idx] = target_app_data[target_data_idx];
                  source_color_data[data_idx] =  target_color_data[target_data_idx];
                }
              }
            }
          }
        }
      }
    } // for each source block
  } // for each target block
}


void boxm2_vecf_appearance_extractor::extract_orbit_appearance(){


}
