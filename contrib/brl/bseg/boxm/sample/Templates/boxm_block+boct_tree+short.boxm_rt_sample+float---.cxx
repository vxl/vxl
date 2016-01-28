#include <boxm/boxm_block.txx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_rt_sample.h>

typedef boct_tree<short, boxm_rt_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);

#include <boxm/boxm_scene.txx>
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree);
