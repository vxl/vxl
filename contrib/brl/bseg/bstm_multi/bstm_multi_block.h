#ifndef bstm_multi_block_h_
#define bstm_multi_block_h_

//:
// \file bstm_multi_block.h
// \brief A block in a space time scene that represents a region in
// space-time through several heirarchical levels of BSTM space-time
// trees.

#include <vbl/vbl_ref_count.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_scene_parser.h>

class bstm_multi_block : public vbl_ref_count {
public:
  typedef bstm_multi_block_metadata metadata;
  typedef bstm_multi_scene_parser parser;

public:
  // TODO should we use char buffer, or vcl_vector of multiple buffers?
  bstm_multi_block(bstm_block_id id, metadata data, char *buffer);
};

#endif // bstm_multi_block_h_
