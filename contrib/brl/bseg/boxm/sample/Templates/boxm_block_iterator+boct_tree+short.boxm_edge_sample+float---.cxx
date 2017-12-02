#include <boxm/boxm_scene.hxx>
#include <boxm/sample/boxm_edge_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,boxm_edge_sample<float> >  tree_type;
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);

typedef boct_tree<short,boxm_aux_edge_sample<float> >  aux_tree_type;
BOXM_BLOCK_ITERATOR_INSTANTIATE(aux_tree_type);
