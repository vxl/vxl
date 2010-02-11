#include <boxm/boxm_block_vis_graph_iterator.txx>
#include <boct/boct_tree.txx>
#include <boxm/boxm_edge_tangent_sample.h>

typedef boct_tree<short, boxm_edge_tangent_sample<float> > tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(tree_type);

typedef boct_tree<short, boxm_inf_line_sample<float> > aux_tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(aux_tree_type);
