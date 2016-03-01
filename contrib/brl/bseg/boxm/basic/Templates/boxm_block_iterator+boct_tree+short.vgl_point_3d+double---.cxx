#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <vgl/vgl_point_3d.h>

typedef boct_tree<short,vgl_point_3d<double> >  tree_type;
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
