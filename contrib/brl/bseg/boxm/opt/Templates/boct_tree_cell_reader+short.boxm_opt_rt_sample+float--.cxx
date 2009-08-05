#include <boct/boct_tree_cell_reader.txx>
//#include "boxm/boxm_apm_traits.h"
#include <boxm/opt/boxm_opt_rt_sample.h>

typedef boxm_opt_rt_sample<float> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);
