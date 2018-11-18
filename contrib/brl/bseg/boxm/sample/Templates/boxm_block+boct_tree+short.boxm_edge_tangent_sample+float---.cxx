#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>

typedef boct_tree<short,boxm_edge_tangent_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);
