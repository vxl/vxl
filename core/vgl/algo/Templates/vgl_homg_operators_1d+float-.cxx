// Instantiation of vgl_homg_operators_1d<float>
#include <vgl/algo/vgl_homg_operators_1d.txx>
template class vgl_homg_operators_1d<float>;
template vgl_homg_point_1d<float> operator*(vnl_matrix_fixed<float,2,2> const& m, vgl_homg_point_1d<float> const& p);
