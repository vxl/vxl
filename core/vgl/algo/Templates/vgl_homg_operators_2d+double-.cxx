// Instantiation of vgl_homg_operators_2d<double>
#include <vgl/algo/vgl_homg_operators_2d.txx>
template class vgl_homg_operators_2d<double>;
template vgl_homg_point_2d<double> operator*(vnl_matrix_fixed<double,3,3> const& m, vgl_homg_point_2d<double> const& p);
template vgl_homg_line_2d<double> operator*(vnl_matrix_fixed<double,3,3> const& m, vgl_homg_line_2d<double> const& p);
