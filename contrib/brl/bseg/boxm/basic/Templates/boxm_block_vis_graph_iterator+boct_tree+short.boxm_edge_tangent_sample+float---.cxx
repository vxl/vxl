#include <boxm/basic/boxm_block_vis_graph_iterator.hxx>
#include <boct/boct_tree.hxx>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <boxm/sample/boxm_inf_line_sample.h>

typedef boct_tree<short, boxm_edge_tangent_sample<float> > tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(tree_type);

typedef boct_tree<short, boxm_inf_line_sample<float> > aux_tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(aux_tree_type);

typedef boct_tree<short, float > aux_tree_type2;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(aux_tree_type2);
