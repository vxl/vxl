// This is core/vpgl/vpgl_affine_tri_focal_tensor.hxx
#ifndef vpgl_affine_tri_focal_tensor_hxx_
#define vpgl_affine_tri_focal_tensor_hxx_
#include "vpgl_affine_tri_focal_tensor.h"
#include <vgl/vgl_tolerance.h>
#include <vnl/vnl_det.h>

template<class Type>
void vpgl_affine_tri_focal_tensor<Type>::set(const vpgl_affine_camera<Type>& c1,const vpgl_affine_camera<Type>& c2, const vpgl_affine_camera<Type>& c3){
  vpgl_proj_camera<Type> p1, p2, p3;
  if (!proj(c1, p1) || !proj(c2, p2) || !proj(c3, p3)) {
    throw std::invalid_argument("vpgl_affine_tri_focal_tensor: affine->projective failed");
  }
  vpgl_tri_focal_tensor<Type>::set(p1, p2, p3);
}

template<class Type>
bool affine(vpgl_proj_camera<Type> const& pcam, vpgl_affine_camera<Type>& acam ){
  Type tol = Type(2)*vgl_tolerance<Type>::position;
  vnl_matrix_fixed<Type, 3, 4> M = pcam.get_matrix();
  // swap cols 3 and 4
  vnl_vector_fixed<Type, 3> col2 = M.get_column(2);
  vnl_vector_fixed<Type, 3> col3 = M.get_column(3);
  M.set_column(2, col3); M.set_column(3, col2);
  // check if affine
  vnl_vector_fixed<Type, 4> r2 = M.get_row(2);
  bool v23_largest = true;
  Type v23 = fabs(r2[3]);
  for(size_t i = 0; i<3; ++i)
    if(fabs(r2[i])> v23)
      v23_largest = false;
  if(!v23_largest)
    return false;
  for(size_t i = 0; i<3; ++i)
    if(fabs(r2[i]/v23) > tol)
      return false;
  M /= r2[3];
  acam.set_matrix(M);
  return true;
}
template<class Type>
bool proj(vpgl_affine_camera<Type> const& acam, vpgl_proj_camera<Type>& pcam){
  Type tol = vgl_tolerance<Type>::position;
  vnl_matrix_fixed<Type, 3, 4> A = acam.get_matrix();
  // swap cols 2 and 3
  vnl_vector_fixed<Type, 3> col2 = A.get_column(2);
  vnl_vector_fixed<Type, 3> col3 = A.get_column(3);
  A.set_column(2, col3); A.set_column(3, col2);
  vnl_matrix_fixed<Type, 3, 3> M;
  M.set_column(0, A.get_column(0));
  M.set_column(1, A.get_column(1));
  M.set_column(2, A.get_column(2));
  Type D = fabs(vnl_det(M));
  if(D<tol)
    return false;
  pcam.set_matrix(A);
  return true;
}
template<class Type>
bool affine(vpgl_fundamental_matrix<Type> const& F, vpgl_affine_fundamental_matrix<Type>& aF )
{
  Type tol = Type(2)*vgl_tolerance<Type>::position;
  vnl_matrix_fixed<Type, 3, 3> M = F.get_matrix();
  Type max = M.absolute_value_max();
  if(max < tol)
    return false;
  M/=max;
  for(size_t r = 0; r<2;++r)
    for(size_t c = 0; c<2; ++c)
      if(fabs(M[r][c]) > tol)
        return false;
  aF.set_matrix(M);
  return true;
}

template<class Type>
std::ostream& operator << (std::ostream& ostr, const vpgl_affine_tri_focal_tensor<Type>& aT){
  const vpgl_tri_focal_tensor<Type>* T = reinterpret_cast<const vpgl_tri_focal_tensor<Type>* >(&aT);
  ostr << *T;
  return ostr;
}
template<class Type>
std::istream& operator >> (std::istream& istr, vpgl_affine_tri_focal_tensor<Type>& aT){
  vpgl_tri_focal_tensor<Type> T;
  istr >> T;
  aT = vpgl_affine_tri_focal_tensor<Type>(T);
  return istr;
}

// Code for easy instantiation.
#undef vpgl_AFFINE_TRI_FOCAL_TENSOR_INSTANTIATE
#define vpgl_AFFINE_TRI_FOCAL_TENSOR_INSTANTIATE(Type) \
template class vpgl_affine_tri_focal_tensor<Type >; \
template std::ostream& operator<<(std::ostream&, const vpgl_affine_tri_focal_tensor<Type>&); \
template std::istream& operator>>(std::istream&, vpgl_affine_tri_focal_tensor<Type>&); \
template bool affine(vpgl_proj_camera<Type> const&, vpgl_affine_camera<Type>&); \
template bool proj(vpgl_affine_camera<Type> const&, vpgl_proj_camera<Type>&) ; \
template bool affine(vpgl_fundamental_matrix<Type> const&, vpgl_affine_fundamental_matrix<Type>&)
#endif // vpgl_affine_tri_focal_tensor_hxx_
