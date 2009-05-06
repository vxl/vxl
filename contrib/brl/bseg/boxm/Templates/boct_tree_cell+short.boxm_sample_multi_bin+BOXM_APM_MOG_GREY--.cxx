#include <boct/boct_tree_cell.txx>
#include "boxm/boxm_apm_traits.h"
#include "boxm/boxm_sample_multi_bin.h"

typedef boxm_sample_multi_bin<BOXM_APM_MOG_GREY> boxm_sample_mog_grey;
BOCT_TREE_CELL_INSTANTIATE(short,boxm_sample_mog_grey );
