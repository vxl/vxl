#ifndef bstm_multi_bstm_block_to_bstm_multi_block_function_h_
#define bstm_multi_bstm_block_to_bstm_multi_block_function_h_

#include <vcl_map.h>
#include <vcl_string.h>

#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>

#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

bool bstm_block_to_bstm_multi_block(
    bstm_multi_block *blk,
    const vcl_map<vcl_string, block_data_base *> &datas,
    bstm_block *bstm_blk,
    bstm_time_block *bstm_blk_t,
    const vcl_map<vcl_string, bstm_data_base *> &bstm_datas,
    double p_threshold,
    double app_threshold);

#endif // bstm_multi_bstm_to_bstm_multi_scene_function_h_
