#include <boct/boct_tree.hxx>
#include <boct/boct_tree_cell.hxx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>

typedef vnl_vector_fixed<double,10> vector;

BOCT_TREE_INSTANTIATE(short, vector );
BOCT_TREE_CELL_INSTANTIATE(short, vector);
