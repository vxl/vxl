#include <vnl/vnl_matrix_fixed.h>
template class vnl_matrix_fixed<double,3,3>;

#if defined(VCL_GCC_27)
template ostream& operator<<(ostream&,vnl_matrix_fixed<double,3,3> const&);
template vnl_matrix<double> operator*(const double &s, const vnl_matrix_fixed<double,3,3> &M);
#endif
