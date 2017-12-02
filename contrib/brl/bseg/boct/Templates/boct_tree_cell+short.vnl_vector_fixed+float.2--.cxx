#include <boct/boct_tree_cell.hxx>
#include <vnl/io/vnl_io_vector_fixed.h>
typedef vnl_vector_fixed<float, 2> v_type;
BOCT_TREE_CELL_INSTANTIATE(short, v_type);
