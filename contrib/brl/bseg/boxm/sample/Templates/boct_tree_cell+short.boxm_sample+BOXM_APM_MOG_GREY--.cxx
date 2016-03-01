#include <bsta/io/bsta_io_mixture_fixed.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>
#include <boct/boct_tree_cell.hxx>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>

typedef boxm_sample<BOXM_APM_MOG_GREY> boxm_sample_mog_grey;
BOCT_TREE_CELL_INSTANTIATE(short,boxm_sample_mog_grey );

typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype apm_datatype;
BOCT_TREE_CELL_INSTANTIATE(short,apm_datatype );
