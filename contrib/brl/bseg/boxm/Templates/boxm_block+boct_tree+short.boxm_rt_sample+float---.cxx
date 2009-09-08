#include <boxm/boxm_block.txx>
#include <boct/boct_tree.h>
#include "boxm/boxm_rt_sample.h"

typedef boct_tree<short,boxm_rt_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);
