#include <boxm2/basic/boxm_block_vis_graph_iterator.txx>
#include <boct/boct_tree.txx>
#include <boxm2/sample/boxm_edge_sample.h>

typedef boct_tree<short, boxm_edge_sample<float> > tree_type;
BOXM2_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(tree_type);
