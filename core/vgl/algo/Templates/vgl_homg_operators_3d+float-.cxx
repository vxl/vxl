// Instantiation of vgl_homg_operators_3d<float>
#include <vgl/algo/vgl_homg_operators_3d.txx>
template class vgl_homg_operators_3d<float>;
template vgl_homg_point_2d<float> operator*(vnl_matrix_fixed<float,3,4> const& m, vgl_homg_point_3d<float> const& p);
template vgl_homg_line_2d<float> operator*(vnl_matrix_fixed<float,3,4> const& m, vgl_homg_plane_3d<float> const& l);
