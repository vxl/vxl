#include <boct/boct_tree_cell_reader.hxx>
#include <boxm/sample/boxm_edge_sample.h>

typedef boxm_edge_sample<float> sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,sample);

typedef boxm_aux_edge_sample<float> aux_sample;
BOCT_TREE_CELL_READER_INSTANTIATE(short,aux_sample);
