#include <boxm2/boxm_scene.txx>
#include <boxm2/boxm_apm_traits.h>
#include <boxm2/sample/boxm_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,boxm_sample<BOXM_APM_MOB_GREY> >  tree_type;
BOXM2_BLOCK_ITERATOR_INSTANTIATE(tree_type);
