// Instantiation of vgl_homg_operators_3d<double>
#include <vgl/algo/vgl_homg_operators_3d.txx>
template class vgl_homg_operators_3d<double>;
template vgl_homg_point_2d<double> operator*(vnl_matrix_fixed<double,3,4> const& m, vgl_homg_point_3d<double> const& p);
template vgl_homg_line_2d<double> operator*(vnl_matrix_fixed<double,3,4> const& m, vgl_homg_plane_3d<double> const& l);
