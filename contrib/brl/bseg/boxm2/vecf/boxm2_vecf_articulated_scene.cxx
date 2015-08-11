#include "boxm2_vecf_articulated_scene.h"
#include <boxm2/io/boxm2_lru_cache.h>
void boxm2_vecf_articulated_scene::clear_target(boxm2_scene_sptr target_scene){
  vcl_string gray_APM_prefix  = boxm2_data_traits<BOXM2_MOG3_GREY>::prefix();
  vcl_string color_APM_prefix = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix();
  vcl_string alpha_prefix     = boxm2_data_traits<BOXM2_ALPHA>::prefix();

  vcl_vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  boxm2_data_base* alpha_db     = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], alpha_prefix);
  boxm2_data_base* gray_app_db  = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], gray_APM_prefix);
  boxm2_data_base* color_app_db = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], color_APM_prefix + "_" + (this->color_apm_id_));
  boxm2_block_metadata m_data = target_scene->get_block_metadata(blocks[0]);
  gray_app_db->set_default_value(gray_APM_prefix,   m_data);
  color_app_db->set_default_value(color_APM_prefix, m_data);
  alpha_db->set_default_value(alpha_prefix, m_data);

}
