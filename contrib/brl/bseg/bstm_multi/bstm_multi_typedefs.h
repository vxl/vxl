#ifndef bstm_multi_typedefs_h_
#define bstm_multi_typedefs_h_

//: \file bstm_multi_typedefs.h
// \brief Make typedefs for templates accessible, e.g. for REGISTER_DATATYPES in
// process config code. There's probably a better way to do this, but I didn't
// want to have bstm_multi_block depend on space_time_scene or space_time_cache.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <vbl/vbl_smart_ptr.h>

#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/io/block_cache.h>
#include <bstm_multi/space_time_scene.h>

typedef space_time_scene<bstm_multi_block> bstm_multi_scene;
typedef vbl_smart_ptr<bstm_multi_scene> bstm_multi_scene_sptr;
typedef block_cache<bstm_multi_scene, bstm_multi_block> bstm_multi_cache;
typedef vbl_smart_ptr<bstm_multi_cache> bstm_multi_cache_sptr;

#endif // bstm_multi_typedefs_h_
