#include <vnl/vnl_matrix_fixed.h>
template class vnl_matrix_fixed<double,2,2>;
#if defined(VCL_GCC_27)
template ostream& operator<<(ostream&,vnl_matrix_fixed<double,2,2> const&);
#endif
