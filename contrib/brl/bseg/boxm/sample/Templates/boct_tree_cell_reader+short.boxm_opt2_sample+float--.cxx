#include <boct/boct_tree_cell_reader.hxx>
#include <boxm/sample/boxm_opt2_sample.h>

typedef boxm_opt2_sample<float> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);
