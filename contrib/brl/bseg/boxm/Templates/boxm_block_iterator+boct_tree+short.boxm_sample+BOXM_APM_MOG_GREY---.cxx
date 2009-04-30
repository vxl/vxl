#include <boxm/boxm_scene.txx>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >  tree_type;
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
