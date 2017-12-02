#include <boxm/basic/boxm_block_vis_graph_iterator.hxx>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boct/boct_tree.hxx>
#include <vgl/io/vgl_io_point_3d.h>

typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > tree_type;
BOXM_BLOCK_VIS_GRAPH_ITERATOR_INSTANTIATE(tree_type);
