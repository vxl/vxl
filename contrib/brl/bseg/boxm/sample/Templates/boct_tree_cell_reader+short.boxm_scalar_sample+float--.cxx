#include <boct/boct_tree_cell_reader.hxx>
#include <boxm/sample/boxm_scalar_sample.h>

typedef boxm_scalar_sample<float> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);
