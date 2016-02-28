#include <boct/boct_tree.hxx>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>

BOCT_TREE_INSTANTIATE(short, boxm_sample<BOXM_APM_SIMPLE_GREY>);

// tree with appearance
typedef boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::apm_datatype apm_datatype;
BOCT_TREE_INSTANTIATE(short, apm_datatype);
