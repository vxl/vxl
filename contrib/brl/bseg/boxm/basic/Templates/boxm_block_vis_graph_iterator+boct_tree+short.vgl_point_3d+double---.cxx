#include <boxm/basic/boxm_block_vis_graph_iterator.hxx>
#include <boct/boct_tree.hxx>
#include <vgl/io/vgl_io_point_3d.h>

typedef boct_tree<short, vgl_point_3d<double> > tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(tree_type);
