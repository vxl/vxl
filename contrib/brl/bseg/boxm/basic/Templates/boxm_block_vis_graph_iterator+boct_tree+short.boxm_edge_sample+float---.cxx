#include <boxm/basic/boxm_block_vis_graph_iterator.hxx>
#include <boct/boct_tree.hxx>
#include <boxm/sample/boxm_edge_sample.h>

typedef boct_tree<short, boxm_edge_sample<float> > tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(tree_type);
