#include <boct/boct_tree_cell_reader.hxx>
#include <boxm/sample/boxm_opt3_sample.h>
#include <boxm/boxm_apm_traits.h>

typedef boxm_opt3_sample<BOXM_APM_MOG_GREY> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);
