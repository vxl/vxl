#include <boxm/boxm_block.txx>
#include <boct/boct_tree.h>
#include <boxm/boxm_edge_tangent_sample.h>
#include <boxm/boxm_inf_line_sample.h>

typedef boct_tree<short,boxm_edge_tangent_sample<float> > aux_tree;
BOXM_BLOCK_INSTANTIATE(aux_tree);

typedef boct_tree<short,boxm_inf_line_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);

typedef boct_tree<short,float> tree2;
BOXM_BLOCK_INSTANTIATE(tree2);

#include <boxm/boxm_scene.txx>
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree2);
BOXM_BLOCK_ITERATOR_INSTANTIATE(aux_tree);

