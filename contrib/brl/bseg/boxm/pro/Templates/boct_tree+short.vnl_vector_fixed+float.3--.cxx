#include <boct/boct_tree.txx>
#include <boct/boct_tree_cell.txx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>

typedef vnl_vector_fixed<float,3> vector;
 

BOCT_TREE_INSTANTIATE(short, vector );
BOCT_TREE_CELL_INSTANTIATE(short, vector);
