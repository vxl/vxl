#include <boxm/boxm_scene.hxx>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,boxm_sample<BOXM_APM_MOB_GREY> >  tree_type;
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
