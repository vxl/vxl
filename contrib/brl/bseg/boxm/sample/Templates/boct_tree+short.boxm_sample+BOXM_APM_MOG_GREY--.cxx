#include <boct/boct_tree.hxx>
#include <bsta/io/bsta_io_mixture_fixed.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>

// tree with alpha and appearance combined
BOCT_TREE_INSTANTIATE(short, boxm_sample<BOXM_APM_MOG_GREY>);

// tree with appearance
typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype apm_datatype;
BOCT_TREE_INSTANTIATE(short, apm_datatype);
