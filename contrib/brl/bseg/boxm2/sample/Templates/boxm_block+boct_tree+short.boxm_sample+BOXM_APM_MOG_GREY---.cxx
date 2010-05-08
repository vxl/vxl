#include <boxm2/boxm_block.txx>
#include <boxm2/boxm_apm_traits.h>
#include <boxm2/sample/boxm_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >  tree_type;
BOXM2_BLOCK_INSTANTIATE(tree_type);

typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype apm_datatype;
typedef boct_tree<short,apm_datatype >  apm_tree_type;

BOXM2_BLOCK_INSTANTIATE(apm_tree_type);
