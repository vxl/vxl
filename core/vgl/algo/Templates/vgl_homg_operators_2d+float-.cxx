// Instantiation of vgl_homg_operators_2d<float>
#include <vgl/algo/vgl_homg_operators_2d.txx>
template class vgl_homg_operators_2d<float>;
template vgl_homg_point_2d<float> operator*(vnl_matrix_fixed<float,3,3> const& m, vgl_homg_point_2d<float> const& p);
template vgl_homg_line_2d<float> operator*(vnl_matrix_fixed<float,3,3> const& m, vgl_homg_line_2d<float> const& p);
