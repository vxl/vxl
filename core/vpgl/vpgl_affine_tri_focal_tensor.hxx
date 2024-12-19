// This is core/vpgl/vpgl_affine_tri_focal_tensor.hxx
#ifndef vpgl_affine_tri_focal_tensor_hxx_
#define vpgl_affine_tri_focal_tensor_hxx_
#include "vpgl_affine_tri_focal_tensor.h"
#include <vgl/vgl_tolerance.h>
#include <vnl/vnl_det.h>

// reset internal state
template <class Type>
void
vpgl_affine_tri_focal_tensor<Type>::clear_img_pt_transforms()
{
  vgl_h_matrix_2d<Type> K;
  K.set_identity();
  img_pt_transforms_.resize(3, K);
}

// get scaling transforms from image dimensions
template <class Type>
std::vector<vgl_h_matrix_2d<Type>>
vpgl_affine_tri_focal_tensor<Type>::img_pt_transforms_from_dims(const std::vector<std::pair<size_t, size_t>> & dims)
{
  // check input
  if (dims.size() != 3)
    throw std::invalid_argument("invalid dims size");

  // populate transforms
  std::vector<vgl_h_matrix_2d<Type>> pt_transforms(3);
  for (size_t i = 0; i < 3; ++i)
  {
    vnl_matrix_fixed<Type, 3, 3> K(Type(0));
    K[0][0] = Type(2) / dims[i].first;
    K[1][1] = Type(2) / dims[i].second;
    K[0][2] = -Type(1);
    K[1][2] = -Type(1);
    K[2][2] = Type(1);
    pt_transforms[i] = vgl_h_matrix_2d<Type>(K);
  }

  return pt_transforms;
}

//: setup using 3 affine cameras
template <class Type>
void
vpgl_affine_tri_focal_tensor<Type>::set(const vpgl_affine_camera<Type> & c1,
                                        const vpgl_affine_camera<Type> & c2,
                                        const vpgl_affine_camera<Type> & c3)
{
  auto T = this->tensor_matrix(c1, c2, c3);
  this->set_cams_and_tensor(c1, c2, c3, T);
}

//: setup using three cameras with scaling transforms
template <class Type>
void
vpgl_affine_tri_focal_tensor<Type>::set(const vpgl_affine_camera<Type> & c1,
                                        const vpgl_affine_camera<Type> & c2,
                                        const vpgl_affine_camera<Type> & c3,
                                        std::vector<vgl_h_matrix_2d<Type>> img_pt_transforms)
{
  vpgl_affine_camera<Type> pre_c1 = premultiply_a(c1, img_pt_transforms[0]);
  vpgl_affine_camera<Type> pre_c2 = premultiply_a(c2, img_pt_transforms[1]);
  vpgl_affine_camera<Type> pre_c3 = premultiply_a(c3, img_pt_transforms[2]);
  auto T = this->tensor_matrix(pre_c1, pre_c2, pre_c3);
  this->set_cams_and_tensor(pre_c1, pre_c2, pre_c3, T, img_pt_transforms);
}

//: setup using three cameras with image dimensions
template <class Type>
void
vpgl_affine_tri_focal_tensor<Type>::set(const vpgl_affine_camera<Type> & c1,
                                        const vpgl_affine_camera<Type> & c2,
                                        const vpgl_affine_camera<Type> & c3,
                                        const std::vector<std::pair<size_t, size_t>> & image_dims_ni_nj)
{
  auto img_pt_transforms = this->img_pt_transforms_from_dims(image_dims_ni_nj);
  this->set(c1, c2, c3, img_pt_transforms);
}

//: setup using 3 affine cameras, tensor, optional scaling transforms
// This function assumes no additional calculations are necessary beyond
// an affine->projective camera conversion for storage
template <class Type>
void
vpgl_affine_tri_focal_tensor<Type>::set_cams_and_tensor(const vpgl_affine_camera<Type> & c1,
                                                        const vpgl_affine_camera<Type> & c2,
                                                        const vpgl_affine_camera<Type> & c3,
                                                        const vbl_array_3d<Type> & T,
                                                        std::vector<vgl_h_matrix_2d<Type>> img_pt_transforms)
{
  // reset internal state
  this->clear();

  // projective cameras
  vpgl_proj_camera<Type> p1, p2, p3;
  if (!proj(c1, p1) || !proj(c2, p2) || !proj(c3, p3))
  {
    throw std::invalid_argument("vpgl_affine_tri_focal_tensor:set "
                                "affine->projective failed");
  }

  // set using parent method
  vpgl_tri_focal_tensor<Type>::set_cams_and_tensor(p1, p2, p3, T);

  // set scaling transforms
  if (!img_pt_transforms.empty())
    img_pt_transforms_ = std::move(img_pt_transforms);
}

//: affine fundamental matrices
template <class Type>
vpgl_affine_fundamental_matrix<Type>
vpgl_affine_tri_focal_tensor<Type>::affine_fmatrix_12()
{
  if (!this->compute_f_matrices())
    throw std::runtime_error("vpgl_tri_focal_tensor::affine_fmatrix_12 "
                             "invalid fundamental matrices");

  vpgl_affine_fundamental_matrix<Type> temp = affine(this->f12_);

  vnl_matrix_fixed<Type, 3, 3> F = temp.get_matrix();
  vnl_matrix_fixed<Type, 3, 3> K1 = img_pt_transforms_[0].get_matrix();
  vnl_matrix_fixed<Type, 3, 3> K2 = img_pt_transforms_[1].get_matrix();
  K2.inplace_transpose();

  vnl_matrix_fixed<Type, 3, 3> ret = K2 * F * K1;
  Type fbn = ret.frobenius_norm();
  if (fbn < vgl_tolerance<Type>::position)
    throw std::runtime_error("vpgl_affine_tri_focal_tensor::affine_fmatrix_12 "
                             "frobenius norm too small");
  ret /= fbn;
  return vpgl_affine_fundamental_matrix<Type>(ret);
}

template <class Type>
vpgl_affine_fundamental_matrix<Type>
vpgl_affine_tri_focal_tensor<Type>::affine_fmatrix_13()
{
  if (!this->compute_f_matrices())
    throw std::runtime_error("vpgl_tri_focal_tensor::affine_fmatrix_13 "
                             "invalid fundamental matrices");

  vpgl_affine_fundamental_matrix<Type> temp = affine(this->f13_);

  vnl_matrix_fixed<Type, 3, 3> F = temp.get_matrix();
  vnl_matrix_fixed<Type, 3, 3> K1 = img_pt_transforms_[0].get_matrix();
  vnl_matrix_fixed<Type, 3, 3> K3 = img_pt_transforms_[2].get_matrix();
  K3.inplace_transpose();

  vnl_matrix_fixed<Type, 3, 3> ret = K3 * F * K1;
  Type fbn = ret.frobenius_norm();
  if (fbn < vgl_tolerance<Type>::position)
    throw std::runtime_error("vpgl_affine_tri_focal_tensor::affine_fmatrix_13 "
                             "frobenius norm too small");
  ret /= fbn;
  return vpgl_affine_fundamental_matrix<Type>(ret);
}

template <class Type>
vpgl_affine_fundamental_matrix<Type>
vpgl_affine_tri_focal_tensor<Type>::affine_fmatrix_23()
{
  if (!this->compute_f_matrix_23())
    throw std::runtime_error("vpgl_tri_focal_tensor::affine_fmatrix_23 "
                             "invalid fundamental matrices");

  vpgl_affine_fundamental_matrix<Type> temp = affine(this->f23_);

  vnl_matrix_fixed<Type, 3, 3> F = temp.get_matrix();
  vnl_matrix_fixed<Type, 3, 3> K2 = img_pt_transforms_[1].get_matrix();
  vnl_matrix_fixed<Type, 3, 3> K3 = img_pt_transforms_[2].get_matrix();
  K3.inplace_transpose();

  vnl_matrix_fixed<Type, 3, 3> ret = K3 * F * K2;
  Type fbn = ret.frobenius_norm();
  if (fbn < vgl_tolerance<Type>::position)
    throw std::runtime_error("vpgl_affine_tri_focal_tensor::affine_fmatrix_23 "
                             "frobenius norm too small");
  ret /= fbn;
  return vpgl_affine_fundamental_matrix<Type>(ret);
}

//: affine cameras
template <class Type>
vpgl_affine_camera<Type>
vpgl_affine_tri_focal_tensor<Type>::affine_camera_1()
{
  if (!this->compute_proj_cameras())
    throw std::runtime_error("vpgl_affine_tri_focal_tensor::affine_camera_1 "
                             "invalid cameras");
  return affine(this->c1_);
}

template <class Type>
vpgl_affine_camera<Type>
vpgl_affine_tri_focal_tensor<Type>::affine_camera_2()
{
  if (!this->compute_proj_cameras())
    throw std::runtime_error("vpgl_affine_tri_focal_tensor::affine_camera_2 "
                             "invalid cameras");
  return affine(this->c2_);
}

template <class Type>
vpgl_affine_camera<Type>
vpgl_affine_tri_focal_tensor<Type>::affine_camera_3()
{
  if (!this->compute_proj_cameras())
    throw std::runtime_error("vpgl_affine_tri_focal_tensor::affine_camera_3 "
                             "invalid cameras");
  return affine(this->c3_);
}


template <class Type>
bool
affine(const vpgl_proj_camera<Type> & pcam, vpgl_affine_camera<Type> & acam)
{
  Type tol = Type(2) * vgl_tolerance<Type>::position;
  vnl_matrix_fixed<Type, 3, 4> M = pcam.get_matrix();
  // swap cols 3 and 4
  vnl_vector_fixed<Type, 3> col2 = M.get_column(2);
  vnl_vector_fixed<Type, 3> col3 = M.get_column(3);
  M.set_column(2, col3);
  M.set_column(3, col2);
  // check if affine
  vnl_vector_fixed<Type, 4> r2 = M.get_row(2);
  bool v23_largest = true;
  Type v23 = fabs(r2[3]);
  for (size_t i = 0; i < 3; ++i)
    if (fabs(r2[i]) > v23)
      v23_largest = false;
  if (!v23_largest)
    return false;
  for (size_t i = 0; i < 3; ++i)
    if (fabs(r2[i] / v23) > tol)
      return false;
  M /= r2[3];
  acam.set_matrix(M);
  return true;
}

template <class Type>
vpgl_affine_camera<Type>
affine(const vpgl_proj_camera<Type> & pcam)
{
  vpgl_affine_camera<Type> acam;
  if (!affine(pcam, acam))
    throw std::runtime_error("Cannot convert from projective to "
                             "affine camera");
  return acam;
}

template <class Type>
bool
proj(const vpgl_affine_camera<Type> & acam, vpgl_proj_camera<Type> & pcam)
{
  Type tol = vgl_tolerance<Type>::position;
  vnl_matrix_fixed<Type, 3, 4> A = acam.get_matrix();
  // swap cols 2 and 3
  vnl_vector_fixed<Type, 3> col2 = A.get_column(2);
  vnl_vector_fixed<Type, 3> col3 = A.get_column(3);
  A.set_column(2, col3);
  A.set_column(3, col2);
  vnl_matrix_fixed<Type, 3, 3> M;
  M.set_column(0, A.get_column(0));
  M.set_column(1, A.get_column(1));
  M.set_column(2, A.get_column(2));
  Type D = fabs(vnl_det(M));
  if (D < tol)
    return false;
  pcam.set_matrix(A);
  return true;
}

template <class Type>
vpgl_proj_camera<Type>
proj(const vpgl_affine_camera<Type> & acam)
{
  vpgl_proj_camera<Type> pcam;
  if (!proj(acam, pcam))
    throw std::runtime_error("Cannot convert from affine to "
                             "projective camera");
  return pcam;
}

template <class Type>
bool
affine(const vpgl_fundamental_matrix<Type> & F, vpgl_affine_fundamental_matrix<Type> & aF)
{
  Type tol = Type(1e-13);
  vnl_matrix_fixed<Type, 3, 3> M = F.get_matrix();
  Type max = M.absolute_value_max();
  if (max < tol)
  {
    std::cerr << "fundamental matrix elements are all (nearly) zero\n" << M << std::endl;
    return false;
  }
  M /= max;

  for (size_t r = 0; r < 2; ++r)
    for (size_t c = 0; c < 2; ++c)
      if (fabs(M[r][c]) > tol)
      {
        std::cerr << "fundamental matrix does not appear to be affine "
                  << "(elements in upper left 2x2 block should have "
                  << "magnitude less than " << tol << ")\n"
                  << M << std::endl;
        return false;
      }

  aF.set_matrix(M);
  return true;
}

template <class Type>
vpgl_affine_fundamental_matrix<Type>
affine(const vpgl_fundamental_matrix<Type> & F)
{
  vpgl_affine_fundamental_matrix<Type> aF;
  if (!affine(F, aF))
    throw std::runtime_error("Cannot convert from projective to "
                             "affine fundamental matrix");
  return aF;
}

template <class Type>
std::ostream &
operator<<(std::ostream & ostr, const vpgl_affine_tri_focal_tensor<Type> & aT)
{
  const vpgl_tri_focal_tensor<Type> * T = reinterpret_cast<const vpgl_tri_focal_tensor<Type> *>(&aT);
  ostr << *T;
  return ostr;
}

template <class Type>
std::istream &
operator>>(std::istream & istr, vpgl_affine_tri_focal_tensor<Type> & aT)
{
  vpgl_tri_focal_tensor<Type> T;
  istr >> T;
  aT = vpgl_affine_tri_focal_tensor<Type>(T);
  return istr;
}

template <class Type>
vbl_array_3d<Type>
vpgl_affine_tri_focal_tensor<Type>::tensor_matrix(const vpgl_affine_camera<Type> & c1,
                                                  const vpgl_affine_camera<Type> & c2,
                                                  const vpgl_affine_camera<Type> & c3)
{
  vbl_array_3d<Type> T(3, 3, 3);
  vnl_matrix_fixed<Type, 3, 4> A = c1.get_matrix();
  vnl_matrix_fixed<Type, 3, 4> B = c2.get_matrix();
  vnl_matrix_fixed<Type, 3, 4> C = c3.get_matrix();
  vnl_matrix_fixed<double, 4, 4> M; // double for numerical precision
  for (size_t i = 0; i < 3; ++i)
    for (size_t q = 0; q < 3; ++q)
      for (size_t r = 0; r < 3; ++r)
      {
        size_t ins = 0;
        for (size_t k = 0; k < 3; ++k)
        {
          if (k == i)
            continue;
          for (size_t s = 0; s < 4; ++s)
          {
            M[ins][s] = A[k][s];
          }
          ins++;
        }
        for (size_t s = 0; s < 4; ++s)
        {
          M[2][s] = B[q][s];
          M[3][s] = C[r][s];
        }
        double sign = pow(-1.0, i);
        T[i][q][r] = Type(sign * vnl_det(M));
      }

  return T;
}

// Code for easy instantiation.
#undef vpgl_AFFINE_TRI_FOCAL_TENSOR_INSTANTIATE
#define vpgl_AFFINE_TRI_FOCAL_TENSOR_INSTANTIATE(Type)                                                 \
  template class vpgl_affine_tri_focal_tensor<Type>;                                                   \
  template std::ostream & operator<<(std::ostream &, const vpgl_affine_tri_focal_tensor<Type> &);      \
  template std::istream & operator>>(std::istream &, vpgl_affine_tri_focal_tensor<Type> &);            \
  template bool affine(vpgl_proj_camera<Type> const &, vpgl_affine_camera<Type> &);                    \
  template vpgl_affine_camera<Type> affine(vpgl_proj_camera<Type> const &);                            \
  template bool proj(vpgl_affine_camera<Type> const &, vpgl_proj_camera<Type> &);                      \
  template vpgl_proj_camera<Type> proj(vpgl_affine_camera<Type> const &);                              \
  template bool affine(vpgl_fundamental_matrix<Type> const &, vpgl_affine_fundamental_matrix<Type> &); \
  template vpgl_affine_fundamental_matrix<Type> affine(vpgl_fundamental_matrix<Type> const &)
#endif // vpgl_affine_tri_focal_tensor_hxx_
