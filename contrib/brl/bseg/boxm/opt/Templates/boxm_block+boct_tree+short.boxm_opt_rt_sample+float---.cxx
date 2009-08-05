#include <boxm/boxm_block.txx>
#include <boct/boct_tree.h>
#include "boxm/opt/boxm_opt_rt_sample.h"

typedef boct_tree<short,boxm_opt_rt_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);
