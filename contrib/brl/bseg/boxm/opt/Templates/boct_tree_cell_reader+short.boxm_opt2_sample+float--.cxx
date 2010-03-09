#include <boct/boct_tree_cell_reader.txx>
#include <boxm/opt/boxm_opt2_sample.h>

typedef boxm_opt2_sample<float> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);
