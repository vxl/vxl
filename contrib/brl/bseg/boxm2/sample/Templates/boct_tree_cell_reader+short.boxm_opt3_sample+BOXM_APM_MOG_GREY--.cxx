#include <boct/boct_tree_cell_reader.txx>
#include <boxm2/sample/boxm_opt3_sample.h>
#include <boxm2/boxm_apm_traits.h>

typedef boxm_opt3_sample<BOXM_APM_MOG_GREY> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);
