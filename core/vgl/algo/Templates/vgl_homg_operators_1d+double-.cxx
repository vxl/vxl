// Instantiation of vgl_homg_operators_1d<double>
#include <vgl/algo/vgl_homg_operators_1d.txx>
template class vgl_homg_operators_1d<double>;
template vgl_homg_point_1d<double> operator*(vnl_matrix_fixed<double,2,2> const& m, vgl_homg_point_1d<double> const& p);
