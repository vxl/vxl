// This is core/vpgl/vpgl_proj_camera.hxx
#ifndef vpgl_tri_focal_tensor_hxx_
#define vpgl_tri_focal_tensor_hxx_

#include "vpgl_tri_focal_tensor.h"
#include <vul/vul_printf.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_tolerance.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_cross_product_matrix.h>

static int
epsilon(size_t i, size_t j, size_t k)
{
  if (i == j || j == k || k == i)
    return 0;
  if (i == 0 && j == 1 && k == 2)
    return 1;
  if (i == 1 && j == 2 && k == 0)
    return 1;
  if (i == 2 && j == 0 && k == 1)
    return 1;
  return -1;
}

template <class Type>
void
vpgl_tri_focal_tensor<Type>::init()
{
  // epipoles
  epipoles_valid_ = false;
  e12_.set(Type(0), Type(0), Type(0));
  e13_.set(Type(0), Type(0), Type(0));

  // cameras
  if (!cameras_valid_)
  {
    vnl_matrix_fixed<Type, 3, 4> c_invalid(Type(0));
    c1_.set_matrix(c_invalid);
    c2_.set_matrix(c_invalid);
    c3_.set_matrix(c_invalid);
  }
  // fundamental matrices
  f_matrices_1213_valid_ = false;
  vnl_matrix_fixed<Type, 3, 3> f_invalid(Type(0));
  f12_.set_matrix(f_invalid);
  f13_.set_matrix(f_invalid);

  f_matrix_23_valid_ = false;
  f23_.set_matrix(f_invalid);
}

template <class Type>
void
vpgl_tri_focal_tensor<Type>::normalize()
{
  Type sum_sqrs = Type(0);
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
      {
        Type t = T_(i, j, k);
        sum_sqrs += t * t;
      }
  sum_sqrs /= Type(27);
  Type s = sqrt(sum_sqrs);
  Type ptol = vgl_tolerance<Type>::position;
  if (s < ptol)
  {
    std::cout << " Frobenius norm too low - " << s << " < " << ptol << " can't normalize" << std::endl;
    return;
  }
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
        T_(i, j, k) /= s;
}

template <class Type>
bool
vpgl_tri_focal_tensor<Type>::compute_proj_cameras()
{
  if (cameras_valid_)
    return true;
  if (!epipoles_valid_)
    this->compute_epipoles();
  if (!epipoles_valid_)
    return false;
  c1_ = vpgl_proj_camera<Type>(); // canonical camera
  vnl_vector_fixed<Type, 3> x(Type(1), Type(1), Type(1));
  Type alpha = Type(1), beta = Type(1);
  vnl_vector_fixed<Type, 3> e12(e12_.x(), e12_.y(), e12_.w()), e13(e13_.x(), e13_.y(), e13_.w());
  vnl_matrix_fixed<Type, 3, 3> Te3 = dot3t(e13.as_ref());
  vnl_matrix_fixed<Type, 3, 3> TTe2 = dot2t(e12.as_ref());
  vnl_matrix_fixed<Type, 3, 3> M;
  M.set_identity();
  M -= outer_product(e13, e13);
  vnl_matrix_fixed<Type, 3, 3> B0 = -M * TTe2;
  vnl_matrix_fixed<Type, 3, 3> DIFF = B0 + TTe2 - outer_product(e13, TTe2.transpose() * e13);
  double diffmag = DIFF.fro_norm();
  if (diffmag > Type(1e-12))
    return false;
  vnl_matrix_fixed<Type, 3, 3> A0 = Te3;
  c2_.set_matrix(A0 + outer_product(e12, x), beta * e12);
  c3_.set_matrix(B0 + outer_product(e13, x), beta * e13);
  // check
  if (!within_scale(*this, vpgl_tri_focal_tensor<Type>(c2_, c3_)))
    return false;
  cameras_valid_ = true;
  return true;
}

// set function given 3 projective cameras, calculating the tensor
template <class Type>
void
vpgl_tri_focal_tensor<Type>::set(const vpgl_proj_camera<Type> & c1,
                                 const vpgl_proj_camera<Type> & c2,
                                 const vpgl_proj_camera<Type> & c3)
{
  vnl_matrix_fixed<Type, 3, 3> M2, M3;
  vnl_vector_fixed<Type, 3> p2, p3;
  cameras_valid_ = true;
  bool c1_is_can = c1.is_canonical(static_cast<Type>(1.0e-6)); // may need to be adjusted if noisy data? FIXME- JLM
  if (c1_is_can)
  {
    c2.decompose(M2, p2);
    c3.decompose(M3, p3);
    c1_ = c1;
    c2_ = c2;
    c3_ = c3;
  }
  else
  {
    vgl_h_matrix_3d<Type> hc = get_canonical_h(c1);
    vpgl_proj_camera<Type> c1_can(c1.get_matrix() * hc.get_matrix());
    vpgl_proj_camera<Type> c2_can(c2.get_matrix() * hc.get_matrix());
    vpgl_proj_camera<Type> c3_can(c3.get_matrix() * hc.get_matrix());
    c1_ = c1_can;
    c2_ = c2_can;
    c3_ = c3_can;
    c2_can.decompose(M2, p2);
    c3_can.decompose(M3, p3);
  }
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
        T_(i, j, k) = (M2(j, i) * p3[k] - M3(k, i) * p2[j]);
  this->normalize();
}

// == CONTRACTION WITH VECTORS ==

//: Compute ${\tt M}_{jk} = T_{ijk} v_i$.
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::dot1(const vnl_vector_fixed<Type, 3> & v) const
{
  vnl_matrix_fixed<Type, 3, 3> answer;
  answer.fill(0.0);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        answer(j, k) += v[i] * T_(i, j, k);
  return answer;
}

//: Compute ${\tt M}_{ik} = T_{ijk} v_j$.
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::dot2(const vnl_vector_fixed<Type, 3> & v) const
{
  vnl_matrix_fixed<Type, 3, 3> answer;
  answer.fill(0.0);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        answer(i, k) += v[j] * T_(i, j, k);
  return answer;
}

//: Compute ${\tt M}_{ij} = T_{ijk} v_k$.
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::dot3(const vnl_vector_fixed<Type, 3> & v) const
{
  vnl_matrix_fixed<Type, 3, 3> answer;
  answer.fill(0.0);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        answer(i, j) += v[k] * T_(i, j, k);
  return answer;
}

//: Compute ${\tt M}_{kj} = T_{ijk} v_i$. (The transpose of dot1).
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::dot1t(const vnl_vector_fixed<Type, 3> & v) const
{
  vnl_matrix_fixed<Type, 3, 3> answer;
  answer.fill(0.0);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        answer(k, j) += v[i] * T_(i, j, k);
  return answer;
}

//: Compute ${\tt M}_{ki} = T_{ijk} v_j$.
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::dot2t(const vnl_vector_fixed<Type, 3> & v) const
{
  vnl_matrix_fixed<Type, 3, 3> answer;
  answer.fill(0.0);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        answer(k, i) += v[j] * T_(i, j, k);
  return answer;
}

//: Compute ${\tt M}_{ji} = T_{ijk} v_k$.
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::dot3t(const vnl_vector_fixed<Type, 3> & v) const
{
  vnl_matrix_fixed<Type, 3, 3> answer;
  answer.fill(0.0);
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        answer(j, i) += v[k] * T_(i, j, k);
  return answer;
}

//: Compute ${\tt M}_{rs} = x_i(x'_j epsilon_{jpr}) (x''_k epsilon+{kqs})T_{ipq)$.
template <class Type>
vnl_matrix_fixed<Type, 3, 3>
vpgl_tri_focal_tensor<Type>::point_constraint_3x3(vgl_homg_point_2d<Type> const & point1,
                                                  vgl_homg_point_2d<Type> const & point2,
                                                  vgl_homg_point_2d<Type> const & point3) const
{
  Type z = Type(0);
  vnl_vector_fixed<Type, 3> x(z), xp(z), xpp(z);
  x[0] = point1.x();
  x[1] = point1.y();
  x[2] = point1.w();
  xp[0] = point2.x();
  xp[1] = point2.y();
  xp[2] = point2.w();
  xpp[0] = point3.x();
  xpp[1] = point3.y();
  xpp[2] = point3.w();
  vnl_matrix_fixed<Type, 3, 3> zero(0.0);
  for (size_t r = 0; r < 3; ++r)
    for (size_t s = 0; s < 3; ++s)
    {
      Type sum_rs = Type(0);
      Type n = Type(0);
      for (size_t i = 0; i < 3; i++)
        for (size_t p = 0; p < 3; p++)
          for (size_t q = 0; q < 3; q++)
          {
            Type sum_j = Type(0), sum_k = Type(0);

            for (size_t j = 0; j < 3; ++j)
              sum_j += xp[j] * epsilon(j, p, r);

            for (size_t k = 0; k < 3; ++k)
              sum_k += xpp[k] * epsilon(k, q, s);

            sum_rs += x[i] * sum_j * sum_k * T_(i, p, q);
            n += Type(1);
          }
      zero[r][s] = sum_rs / n;
    }
  return zero;
}

template <class Type>
Type
vpgl_tri_focal_tensor<Type>::point_constraint(vgl_homg_point_2d<Type> const & point1,
                                              vgl_homg_point_2d<Type> const & point2,
                                              vgl_homg_point_2d<Type> const & point3) const
{
  Type z = Type(0);
  vnl_vector_fixed<Type, 3> m(z), mp(z), mpp(z);
  m[0] = point1.x();
  m[1] = point1.y();
  m[2] = point1.w();
  mp[0] = point2.x();
  mp[1] = point2.y();
  mp[2] = point2.w();
  mpp[0] = point3.x();
  mpp[1] = point3.y();
  mpp[2] = point3.w();
  Type total_sum = z;
  Type n_terms = z;
  for (size_t i = 0; i < 3; i++)
  {
    Type sum_jk = z;
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
      {
        sum_jk += mp[j] * mpp[k] * T_(i, 2, 2);
        sum_jk -= mpp[k] * T_(i, j, 2);
        sum_jk -= mp[j] * T_(i, 2, k);
        sum_jk += T_(i, j, k);
        n_terms += Type(1);
      }
    total_sum += m[i] * sum_jk;
  }
  return total_sum / n_terms;
}

template <class Type>
vnl_vector_fixed<Type, 3>
vpgl_tri_focal_tensor<Type>::line_constraint_3(vgl_homg_line_2d<Type> const & line1,
                                               vgl_homg_line_2d<Type> const & line2,
                                               vgl_homg_line_2d<Type> const & line3) const
{
  Type z = Type(0);
  vnl_vector_fixed<Type, 3> l(z), lp(z), lpp(z);
  l[0] = line1.a();
  l[1] = line1.b();
  l[2] = line1.c();
  lp[0] = line2.a();
  lp[1] = line2.b();
  lp[2] = line2.c();
  lpp[0] = line3.a();
  lpp[1] = line3.b();
  lpp[2] = line3.c();
  vnl_vector_fixed<Type, 3> zero_s(0.0);
  for (size_t s = 0; s < 3; ++s)
  {
    Type sum_r = Type(0);
    Type n = Type(0);
    for (size_t r = 0; r < 3; ++r)
    {
      for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 3; j++)
          for (size_t k = 0; k < 3; k++)
          {
            n += Type(1);
            sum_r += l[r] * epsilon(r, i, s) * lp[j] * lpp[k] * T_(i, j, k);
          }
      zero_s[s] = sum_r / n;
    }
  }
  return zero_s;
}

template <class Type>
vgl_h_matrix_2d<Type>
vpgl_tri_focal_tensor<Type>::hmatrix_13(vgl_homg_line_2d<Type> const & line2) const
{
  vnl_vector_fixed<Type, 3> l2(line2.a(), line2.b(), line2.c());
  return vgl_h_matrix_2d<Type>(dot2t(l2));
}

template <class Type>
vgl_h_matrix_2d<Type>
vpgl_tri_focal_tensor<Type>::hmatrix_12(vgl_homg_line_2d<Type> const & line3) const
{
  vnl_vector_fixed<Type, 3> l3(line3.a(), line3.b(), line3.c());
  return vgl_h_matrix_2d<Type>(dot3t(l3)); // do3->dot3t from oxl/mvl/TriTensor
}

//: Contract tensor axis tensor_axis with first component of matrix $M$.
// That is, where $S$ is the result of the operation:
//
// - For tensor_axis = 1, compute $S_{ijk} = T_{pjk} M_{pi}$
// - For tensor_axis = 2, compute $S_{ijk} = T_{ipk} M_{pj}$
// - For tensor_axis = 3, compute $S_{ijk} = T_{ijp} M_{pk}$
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::postmultiply(size_t tensor_axis, const vnl_matrix<Type> & M) const
{
  switch (tensor_axis)
  {
    case 1:
      return postmultiply1(M);
    case 2:
      return postmultiply2(M);
    case 3:
      return postmultiply3(M);
    default:
      throw std::invalid_argument("unexpected tensor_axis");
  }
}

//: Contract tensor axis tensor_axis with second component of matrix $M$.
// That is, where $S$ is the result of the operation:
//
// - For tensor_axis = 1, compute $S_{ijk} = M_{ip} T_{pjk}$
// - For tensor_axis = 2, compute $S_{ijk} = M_{jp} T_{ipk}$
// - For tensor_axis = 3, compute $S_{ijk} = M_{kp} T_{ijp}$
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::premultiply(size_t tensor_axis, const vnl_matrix<Type> & M) const
{
  switch (tensor_axis)
  {
    case 1:
      return premultiply1(M);
    case 2:
      return premultiply2(M);
    case 3:
      return premultiply3(M);
    default:
      throw std::invalid_argument("unexpected tensor_axis");
  }
}

//: Compute $ S_{ijk} = T_{pjk} M_{pi} $.
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::postmultiply1(const vnl_matrix<Type> & M) const
{
  vpgl_tri_focal_tensor<Type> S;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
      {
        double v = 0;
        for (int p = 0; p < 3; ++p)
          v += T_(p, j, k) * M(p, i);
        S(i, j, k) = v;
      }
  return S;
}

//: Compute $ S_{ijk} = T_{ipk} M_{pj} $.
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::postmultiply2(const vnl_matrix<Type> & M) const
{
  vpgl_tri_focal_tensor<Type> S;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
      {
        double v = 0;
        for (int p = 0; p < 3; ++p)
          v += T_(i, p, k) * M(p, j);
        S(i, j, k) = v;
      }
  return S;
}

//: Compute $ S_{ijk} = T_{ijp} M_{pk} $.
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::postmultiply3(const vnl_matrix<Type> & M) const
{
  vpgl_tri_focal_tensor<Type> S;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
      {
        double v = 0;
        for (int p = 0; p < 3; ++p)
          v += T_(i, j, p) * M(p, k);
        S(i, j, k) = v;
      }
  return S;
}

//: Compute $ S_{ijk} = M_{ip} T_{pjk} $.
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::premultiply1(const vnl_matrix<Type> & M) const
{
  vpgl_tri_focal_tensor<Type> S;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
      {
        double v = 0;
        for (int p = 0; p < 3; ++p)
          v += M(i, p) * T_(p, j, k);
        S(i, j, k) = v;
      }
  return S;
}

//: Compute $ S_{ijk} = M_{jp} T_{ipk} $.
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::premultiply2(const vnl_matrix<Type> & M) const
{
  vpgl_tri_focal_tensor<Type> S;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
      {
        double v = 0;
        for (int p = 0; p < 3; ++p)
          v += M(j, p) * T_(i, p, k);
        S(i, j, k) = v;
      }
  return S;
}

//: Compute $ S_{ijk} = M_{kp} T_{ijp} $.
template <class Type>
vpgl_tri_focal_tensor<Type>
vpgl_tri_focal_tensor<Type>::premultiply3(const vnl_matrix<Type> & M) const
{
  vpgl_tri_focal_tensor<Type> S;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
      {
        double v = 0;
        for (int p = 0; p < 3; ++p)
          v += M(k, p) * T_(i, j, p);
        S(i, j, k) = v;
      }
  return S;
}

template <class Type>
vgl_homg_line_2d<Type>
vpgl_tri_focal_tensor<Type>::image1_transfer(vgl_homg_line_2d<Type> const & line2,
                                             vgl_homg_line_2d<Type> const & line3) const
{
  vnl_vector_fixed<Type, 3> l1(0.0, 0.0, 0.0);
  vnl_vector_fixed<Type, 3> l2(line2.a(), line2.b(), line2.c());
  vnl_vector_fixed<Type, 3> l3(line3.a(), line3.b(), line3.c());

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        l1[i] += T_(i, j, k) * l2[j] * l3[k];

  return vgl_homg_line_2d<Type>(l1[0], l1[1], l1[2]);
}

template <class Type>
vgl_homg_line_2d<Type>
vpgl_tri_focal_tensor<Type>::image2_transfer(vgl_homg_line_2d<Type> const & line1,
                                             vgl_homg_line_2d<Type> const & line3) const
{
  vnl_vector_fixed<Type, 3> l1(line1.a(), line1.b(), line1.c());
  vnl_vector_fixed<Type, 3> l3(line3.a(), line3.b(), line3.c());
  vnl_vector_fixed<Type, 3> l = vnl_inverse(dot3(l3)) * l1;
  return vgl_homg_line_2d<Type>(l[0], l[1], l[2]);
}

template <class Type>
vgl_homg_line_2d<Type>
vpgl_tri_focal_tensor<Type>::image3_transfer(vgl_homg_line_2d<Type> const & line1,
                                             vgl_homg_line_2d<Type> const & line2) const
{
  vnl_vector_fixed<Type, 3> l1(line1.a(), line1.b(), line1.c());
  vnl_vector_fixed<Type, 3> l2(line2.a(), line2.b(), line2.c());
  vnl_vector_fixed<Type, 3> l = vnl_inverse(dot2(l2)) * l1;
  return vgl_homg_line_2d<Type>(l[0], l[1], l[2]);
}

template <class Type>
void
vpgl_tri_focal_tensor<Type>::get_constraint_lines_image1(vgl_homg_point_2d<Type> const & p2,
                                                         vgl_homg_point_2d<Type> const & p3,
                                                         std::vector<vgl_homg_line_2d<Type>> & lines) const
{
  // use the same notation as the output of tr_hartley_equation.

  Type x2 = p2.x();
  Type y2 = p2.y();
  Type z2 = p2.w();

  Type x3 = p3.x();
  Type y3 = p3.y();
  Type z3 = p3.w();

  lines.resize(0);

  /* 0 */

  {
    Type lx = x2 * y3 * T_(0, 1, 0) - y2 * y3 * T_(0, 0, 0)
            - x2 * x3 * T_(0, 1, 1) + y2 * x3 * T_(0, 0, 1);

    Type ly = x2 * y3 * T_(1, 1, 0) - y2 * y3 * T_(1, 0, 0)
            - x2 * x3 * T_(1, 1, 1) + y2 * x3 * T_(1, 0, 1);

    Type lz = x2 * y3 * T_(2, 1, 0) - y2 * y3 * T_(2, 0, 0)
            - x2 * x3 * T_(2, 1, 1) + y2 * x3 * T_(2, 0, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 1 */
  {
    Type lx = x2 * z3 * T_(0, 1, 0) - y2 * z3 * T_(0, 0, 0)
            - x2 * x3 * T_(0, 1, 2) + y2 * x3 * T_(0, 0, 2);

    Type ly = x2 * z3 * T_(1, 1, 0) - y2 * z3 * T_(1, 0, 0)
            - x2 * x3 * T_(1, 1, 2) + y2 * x3 * T_(1, 0, 2);

    Type lz = x2 * z3 * T_(2, 1, 0) - y2 * z3 * T_(2, 0, 0)
            - x2 * x3 * T_(2, 1, 2) + y2 * x3 * T_(2, 0, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 2 */
  {
    Type lx = x2 * z3 * T_(0, 1, 1) - y2 * z3 * T_(0, 0, 1)
            - x2 * y3 * T_(0, 1, 2) + y2 * y3 * T_(0, 0, 2);

    Type ly = x2 * z3 * T_(1, 1, 1) - y2 * z3 * T_(1, 0, 1)
            - x2 * y3 * T_(1, 1, 2) + y2 * y3 * T_(1, 0, 2);

    Type lz = x2 * z3 * T_(2, 1, 1) - y2 * z3 * T_(2, 0, 1)
            - x2 * y3 * T_(2, 1, 2) + y2 * y3 * T_(2, 0, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 3 */
  {
    Type lx = x2 * y3 * T_(0, 2, 0) - z2 * y3 * T_(0, 0, 0)
            - x2 * x3 * T_(0, 2, 1) + z2 * x3 * T_(0, 0, 1);

    Type ly = x2 * y3 * T_(1, 2, 0) - z2 * y3 * T_(1, 0, 0)
            - x2 * x3 * T_(1, 2, 1) + z2 * x3 * T_(1, 0, 1);

    Type lz = x2 * y3 * T_(2, 2, 0) - z2 * y3 * T_(2, 0, 0)
            - x2 * x3 * T_(2, 2, 1) + z2 * x3 * T_(2, 0, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 4 */
  {
    Type lx = x2 * z3 * T_(0, 2, 0) - z2 * z3 * T_(0, 0, 0)
            - x2 * x3 * T_(0, 2, 2) + z2 * x3 * T_(0, 0, 2);

    Type ly = x2 * z3 * T_(1, 2, 0) - z2 * z3 * T_(1, 0, 0)
            - x2 * x3 * T_(1, 2, 2) + z2 * x3 * T_(1, 0, 2);

    Type lz = x2 * z3 * T_(2, 2, 0) - z2 * z3 * T_(2, 0, 0)
            - x2 * x3 * T_(2, 2, 2) + z2 * x3 * T_(2, 0, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 5 */
  {
    Type lx = x2 * z3 * T_(0, 2, 1) - z2 * z3 * T_(0, 0, 1)
            - x2 * y3 * T_(0, 2, 2) + z2 * y3 * T_(0, 0, 2);

    Type ly = x2 * z3 * T_(1, 2, 1) - z2 * z3 * T_(1, 0, 1)
            - x2 * y3 * T_(1, 2, 2) + z2 * y3 * T_(1, 0, 2);

    Type lz = x2 * z3 * T_(2, 2, 1) - z2 * z3 * T_(2, 0, 1)
            - x2 * y3 * T_(2, 2, 2) + z2 * y3 * T_(2, 0, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 6 */
  {
    Type lx = y2 * y3 * T_(0, 2, 0) - z2 * y3 * T_(0, 1, 0)
            - y2 * x3 * T_(0, 2, 1) + z2 * x3 * T_(0, 1, 1);

    Type ly = y2 * y3 * T_(1, 2, 0) - z2 * y3 * T_(1, 1, 0)
            - y2 * x3 * T_(1, 2, 1) + z2 * x3 * T_(1, 1, 1);

    Type lz = y2 * y3 * T_(2, 2, 0) - z2 * y3 * T_(2, 1, 0)
            - y2 * x3 * T_(2, 2, 1) + z2 * x3 * T_(2, 1, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 7 */
  {
    Type lx = y2 * z3 * T_(0, 2, 0) - z2 * z3 * T_(0, 1, 0)
            - y2 * x3 * T_(0, 2, 2) + z2 * x3 * T_(0, 1, 2);

    Type ly = y2 * z3 * T_(1, 2, 0) - z2 * z3 * T_(1, 1, 0)
            - y2 * x3 * T_(1, 2, 2) + z2 * x3 * T_(1, 1, 2);

    Type lz = y2 * z3 * T_(2, 2, 0) - z2 * z3 * T_(2, 1, 0)
            - y2 * x3 * T_(2, 2, 2) + z2 * x3 * T_(2, 1, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 8 */
  {
    Type lx = y2 * z3 * T_(0, 2, 1) - z2 * z3 * T_(0, 1, 1)
            - y2 * y3 * T_(0, 2, 2) + z2 * y3 * T_(0, 1, 2);

    Type ly = y2 * z3 * T_(1, 2, 1) - z2 * z3 * T_(1, 1, 1)
            - y2 * y3 * T_(1, 2, 2) + z2 * y3 * T_(1, 1, 2);

    Type lz = y2 * z3 * T_(2, 2, 1) - z2 * z3 * T_(2, 1, 1)
            - y2 * y3 * T_(2, 2, 2) + z2 * y3 * T_(2, 1, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }
}

template <class Type>
void
vpgl_tri_focal_tensor<Type>::get_constraint_lines_image2(vgl_homg_point_2d<Type> const & p1,
                                                         vgl_homg_point_2d<Type> const & p3,
                                                         std::vector<vgl_homg_line_2d<Type>> & lines) const
{
  Type x1 = p1.x();
  Type y1 = p1.y();
  Type z1 = p1.w();

  Type x3 = p3.x();
  Type y3 = p3.y();
  Type z3 = p3.w();

  lines.resize(0);

  /* 0 */
  {
    Type lx = x1 * y3 * T_(0, 1, 0) - x1 * x3 * T_(0, 1, 1)
            + y1 * y3 * T_(1, 1, 0) - y1 * x3 * T_(1, 1, 1)
            + z1 * y3 * T_(2, 1, 0) - z1 * x3 * T_(2, 1, 1);

    Type ly = -x1 * y3 * T_(0, 0, 0) + x1 * x3 * T_(0, 0, 1)
            - y1 * y3 * T_(1, 0, 0) + y1 * x3 * T_(1, 0, 1)
            - z1 * y3 * T_(2, 0, 0) + z1 * x3 * T_(2, 0, 1);

    Type lz = 0;

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 1 */
  {
    Type lx = x1 * z3 * T_(0, 1, 0) - x1 * x3 * T_(0, 1, 2)
            + y1 * z3 * T_(1, 1, 0) - y1 * x3 * T_(1, 1, 2)
            + z1 * z3 * T_(2, 1, 0) - z1 * x3 * T_(2, 1, 2);

    Type ly = -x1 * z3 * T_(0, 0, 0) + x1 * x3 * T_(0, 0, 2)
            - y1 * z3 * T_(1, 0, 0) + y1 * x3 * T_(1, 0, 2)
            - z1 * z3 * T_(2, 0, 0) + z1 * x3 * T_(2, 0, 2);

    Type lz = 0;
    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 2 */
  {
    Type lx = x1 * z3 * T_(0, 1, 1) - x1 * y3 * T_(0, 1, 2)
            + y1 * z3 * T_(1, 1, 1) - y1 * y3 * T_(1, 1, 2)
            + z1 * z3 * T_(2, 1, 1) - z1 * y3 * T_(2, 1, 2);

    Type ly = -x1 * z3 * T_(0, 0, 1) + x1 * y3 * T_(0, 0, 2)
            - y1 * z3 * T_(1, 0, 1) + y1 * y3 * T_(1, 0, 2)
            - z1 * z3 * T_(2, 0, 1) + z1 * y3 * T_(2, 0, 2);

    Type lz = 0;

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 3 */
  {
    Type lx = x1 * y3 * T_(0, 2, 0) - x1 * x3 * T_(0, 2, 1)
            + y1 * y3 * T_(1, 2, 0) - y1 * x3 * T_(1, 2, 1)
            + z1 * y3 * T_(2, 2, 0) - z1 * x3 * T_(2, 2, 1);

    Type ly = 0;

    Type lz = -x1 * y3 * T_(0, 0, 0) + x1 * x3 * T_(0, 0, 1)
            - y1 * y3 * T_(1, 0, 0) + y1 * x3 * T_(1, 0, 1)
            - z1 * y3 * T_(2, 0, 0) + z1 * x3 * T_(2, 0, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 4 */
  {
    Type lx = x1 * z3 * T_(0, 2, 0) - x1 * x3 * T_(0, 2, 2)
            + y1 * z3 * T_(1, 2, 0) - y1 * x3 * T_(1, 2, 2)
            + z1 * z3 * T_(2, 2, 0) - z1 * x3 * T_(2, 2, 2);

    Type ly = 0;

    Type lz = -x1 * z3 * T_(0, 0, 0) + x1 * x3 * T_(0, 0, 2)
            - y1 * z3 * T_(1, 0, 0) + y1 * x3 * T_(1, 0, 2)
            - z1 * z3 * T_(2, 0, 0) + z1 * x3 * T_(2, 0, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 5 */
  {
    Type lx = x1 * z3 * T_(0, 2, 1) - x1 * y3 * T_(0, 2, 2)
            + y1 * z3 * T_(1, 2, 1) - y1 * y3 * T_(1, 2, 2)
            + z1 * z3 * T_(2, 2, 1) - z1 * y3 * T_(2, 2, 2);

    Type ly = 0;

    Type lz = -x1 * z3 * T_(0, 0, 1) + x1 * y3 * T_(0, 0, 2)
            - y1 * z3 * T_(1, 0, 1) + y1 * y3 * T_(1, 0, 2)
            - z1 * z3 * T_(2, 0, 1) + z1 * y3 * T_(2, 0, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 6 */
  {
    Type lx = 0;

    Type ly = x1 * y3 * T_(0, 2, 0) - x1 * x3 * T_(0, 2, 1)
            + y1 * y3 * T_(1, 2, 0) - y1 * x3 * T_(1, 2, 1)
            + z1 * y3 * T_(2, 2, 0) - z1 * x3 * T_(2, 2, 1);

    Type lz = -x1 * y3 * T_(0, 1, 0) + x1 * x3 * T_(0, 1, 1)
            - y1 * y3 * T_(1, 1, 0) + y1 * x3 * T_(1, 1, 1)
            - z1 * y3 * T_(2, 1, 0) + z1 * x3 * T_(2, 1, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 7 */
  {
    Type lx = 0;

    Type ly = x1 * z3 * T_(0, 2, 0) - x1 * x3 * T_(0, 2, 2)
            + y1 * z3 * T_(1, 2, 0) - y1 * x3 * T_(1, 2, 2)
            + z1 * z3 * T_(2, 2, 0) - z1 * x3 * T_(2, 2, 2);

    Type lz = -x1 * z3 * T_(0, 1, 0) + x1 * x3 * T_(0, 1, 2)
            - y1 * z3 * T_(1, 1, 0) + y1 * x3 * T_(1, 1, 2)
            - z1 * z3 * T_(2, 1, 0) + z1 * x3 * T_(2, 1, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 8 */
  {
    Type lx = 0;

    Type ly = x1 * z3 * T_(0, 2, 1) - x1 * y3 * T_(0, 2, 2)
            + y1 * z3 * T_(1, 2, 1) - y1 * y3 * T_(1, 2, 2)
            + z1 * z3 * T_(2, 2, 1) - z1 * y3 * T_(2, 2, 2);

    Type lz = -x1 * z3 * T_(0, 1, 1) + x1 * y3 * T_(0, 1, 2)
            - y1 * z3 * T_(1, 1, 1) + y1 * y3 * T_(1, 1, 2)
            - z1 * z3 * T_(2, 1, 1) + z1 * y3 * T_(2, 1, 2);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }
}

template <class Type>
void
vpgl_tri_focal_tensor<Type>::get_constraint_lines_image3(vgl_homg_point_2d<Type> const & p1,
                                                         vgl_homg_point_2d<Type> const & p2,
                                                         std::vector<vgl_homg_line_2d<Type>> & lines) const
{
  // use the same notation as the output of tr_hartley_equation.
  Type x1 = p1.x();
  Type y1 = p1.y();
  Type z1 = p1.w();

  Type x2 = p2.x();
  Type y2 = p2.y();
  Type z2 = p2.w();

  lines.clear();

  /* 0 */
  {
    Type lx = -x1 * x2 * T_(0, 1, 1) + x1 * y2 * T_(0, 0, 1)
            - y1 * x2 * T_(1, 1, 1) + y1 * y2 * T_(1, 0, 1)
            - z1 * x2 * T_(2, 1, 1) + z1 * y2 * T_(2, 0, 1);

    Type ly = x1 * x2 * T_(0, 1, 0) - x1 * y2 * T_(0, 0, 0)
            + y1 * x2 * T_(1, 1, 0) - y1 * y2 * T_(1, 0, 0)
            + z1 * x2 * T_(2, 1, 0) - z1 * y2 * T_(2, 0, 0);

    Type lz = 0;

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 1 */
  {
    Type lx = -x1 * x2 * T_(0, 1, 2) + x1 * y2 * T_(0, 0, 2)
            - y1 * x2 * T_(1, 1, 2) + y1 * y2 * T_(1, 0, 2)
            - z1 * x2 * T_(2, 1, 2) + z1 * y2 * T_(2, 0, 2);

    Type ly = 0;

    Type lz = x1 * x2 * T_(0, 1, 0) - x1 * y2 * T_(0, 0, 0)
            + y1 * x2 * T_(1, 1, 0) - y1 * y2 * T_(1, 0, 0)
            + z1 * x2 * T_(2, 1, 0) - z1 * y2 * T_(2, 0, 0);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 2 */
  {
    Type lx = 0;

    Type ly = -x1 * x2 * T_(0, 1, 2) + x1 * y2 * T_(0, 0, 2)
            - y1 * x2 * T_(1, 1, 2) + y1 * y2 * T_(1, 0, 2)
            - z1 * x2 * T_(2, 1, 2) + z1 * y2 * T_(2, 0, 2);

    Type lz = x1 * x2 * T_(0, 1, 1) - x1 * y2 * T_(0, 0, 1)
            + y1 * x2 * T_(1, 1, 1) - y1 * y2 * T_(1, 0, 1)
            + z1 * x2 * T_(2, 1, 1) - z1 * y2 * T_(2, 0, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 3 */
  {
    Type lx = -x1 * x2 * T_(0, 2, 1) + x1 * z2 * T_(0, 0, 1)
            - y1 * x2 * T_(1, 2, 1) + y1 * z2 * T_(1, 0, 1)
            - z1 * x2 * T_(2, 2, 1) + z1 * z2 * T_(2, 0, 1);

    Type ly = x1 * x2 * T_(0, 2, 0) - x1 * z2 * T_(0, 0, 0)
            + y1 * x2 * T_(1, 2, 0) - y1 * z2 * T_(1, 0, 0)
            + z1 * x2 * T_(2, 2, 0) - z1 * z2 * T_(2, 0, 0);

    Type lz = 0;

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 4 */
  {
    Type lx = -x1 * x2 * T_(0, 2, 2) + x1 * z2 * T_(0, 0, 2)
            - y1 * x2 * T_(1, 2, 2) + y1 * z2 * T_(1, 0, 2)
            - z1 * x2 * T_(2, 2, 2) + z1 * z2 * T_(2, 0, 2);

    Type ly = 0;

    Type lz = x1 * x2 * T_(0, 2, 0) - x1 * z2 * T_(0, 0, 0)
            + y1 * x2 * T_(1, 2, 0) - y1 * z2 * T_(1, 0, 0)
            +  z1 * x2 * T_(2, 2, 0) - z1 * z2 * T_(2, 0, 0);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 5 */
  {
    Type lx = 0;

    Type ly = -x1 * x2 * T_(0, 2, 2) + x1 * z2 * T_(0, 0, 2)
            - y1 * x2 * T_(1, 2, 2) + y1 * z2 * T_(1, 0, 2)
            - z1 * x2 * T_(2, 2, 2) + z1 * z2 * T_(2, 0, 2);

    Type lz = x1 * x2 * T_(0, 2, 1) - x1 * z2 * T_(0, 0, 1)
            + y1 * x2 * T_(1, 2, 1) - y1 * z2 * T_(1, 0, 1)
            + z1 * x2 * T_(2, 2, 1) - z1 * z2 * T_(2, 0, 1);
    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 6 */
  {
    Type lx = -x1 * y2 * T_(0, 2, 1) + x1 * z2 * T_(0, 1, 1)
            - y1 * y2 * T_(1, 2, 1) + y1 * z2 * T_(1, 1, 1)
            - z1 * y2 * T_(2, 2, 1) + z1 * z2 * T_(2, 1, 1);

    Type ly = x1 * y2 * T_(0, 2, 0) - x1 * z2 * T_(0, 1, 0)
            + y1 * y2 * T_(1, 2, 0) - y1 * z2 * T_(1, 1, 0)
            + z1 * y2 * T_(2, 2, 0) - z1 * z2 * T_(2, 1, 0);

    Type lz = 0;
    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 7 */
  {
    Type lx = -x1 * y2 * T_(0, 2, 2) + x1 * z2 * T_(0, 1, 2)
            - y1 * y2 * T_(1, 2, 2) + y1 * z2 * T_(1, 1, 2)
            - z1 * y2 * T_(2, 2, 2) + z1 * z2 * T_(2, 1, 2);

    Type ly = 0;

    Type lz = x1 * y2 * T_(0, 2, 0) - x1 * z2 * T_(0, 1, 0)
            + y1 * y2 * T_(1, 2, 0) - y1 * z2 * T_(1, 1, 0)
            + z1 * y2 * T_(2, 2, 0) - z1 * z2 * T_(2, 1, 0);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }

  /* 8 */
  {
    Type lx = 0;

    Type ly = -x1 * y2 * T_(0, 2, 2) + x1 * z2 * T_(0, 1, 2)
            - y1 * y2 * T_(1, 2, 2) + y1 * z2 * T_(1, 1, 2)
            - z1 * y2 * T_(2, 2, 2) + z1 * z2 * T_(2, 1, 2);

    Type lz = x1 * y2 * T_(0, 2, 1) - x1 * z2 * T_(0, 1, 1)
            + y1 * y2 * T_(1, 2, 1) - y1 * z2 * T_(1, 1, 1)
            + z1 * y2 * T_(2, 2, 1) - z1 * z2 * T_(2, 1, 1);

    if (!(lx == Type(0) && ly == Type(0) && lz == Type(0)))
      lines.push_back(vgl_homg_line_2d<Type>(lx, ly, lz));
  }
}

template <class Type>
vgl_homg_point_2d<Type>
vpgl_tri_focal_tensor<Type>::image1_transfer(vgl_homg_point_2d<Type> const & point2,
                                             vgl_homg_point_2d<Type> const & point3) const
{
  std::vector<vgl_homg_line_2d<Type>> constraint_lines(9);
  get_constraint_lines_image1(point2, point3, constraint_lines);
  return vgl_homg_operators_2d<Type>::lines_to_point(constraint_lines);
}

template <class Type>
vgl_homg_point_2d<Type>
vpgl_tri_focal_tensor<Type>::image2_transfer(vgl_homg_point_2d<Type> const & point1,
                                             vgl_homg_point_2d<Type> const & point3) const
{
  std::vector<vgl_homg_line_2d<Type>> constraint_lines(9);
  get_constraint_lines_image2(point1, point3, constraint_lines);
  return vgl_homg_operators_2d<Type>::lines_to_point(constraint_lines);
}

template <class Type>
vgl_homg_point_2d<Type>
vpgl_tri_focal_tensor<Type>::image3_transfer(vgl_homg_point_2d<Type> const & point1,
                                             vgl_homg_point_2d<Type> const & point2) const
{
  std::vector<vgl_homg_line_2d<Type>> constraint_lines(9);
  get_constraint_lines_image3(point1, point2, constraint_lines);
  return vgl_homg_operators_2d<Type>::lines_to_point(constraint_lines);
}

template <class Type>
bool
vpgl_tri_focal_tensor<Type>::compute_epipoles()
{
  if (epipoles_valid_)
    return true;

  Type tol = vgl_tolerance<Type>::position;
  vnl_matrix_fixed<Type, 3, 3> T1 = dot1(vnl_vector_fixed<Type, 3>(1, 0, 0).as_ref());
  vnl_matrix_fixed<Type, 3, 3> T2 = dot1(vnl_vector_fixed<Type, 3>(0, 1, 0).as_ref());
  vnl_matrix_fixed<Type, 3, 3> T3 = dot1(vnl_vector_fixed<Type, 3>(0, 0, 1).as_ref());

  vnl_svd<Type> svd1(T1.as_ref());

  vnl_vector_fixed<Type, 3> u1 = svd1.nullvector();
  vnl_vector_fixed<Type, 3> v1 = svd1.left_nullvector();

  vnl_svd<Type> svd2(T2.as_ref());
  vnl_vector_fixed<Type, 3> u2 = svd2.nullvector();
  vnl_vector_fixed<Type, 3> v2 = svd2.left_nullvector();

  vnl_svd<Type> svd3(T3.as_ref());
  vnl_vector_fixed<Type, 3> u3 = svd3.nullvector();
  vnl_vector_fixed<Type, 3> v3 = svd3.left_nullvector();

  vnl_matrix_fixed<Type, 3, 3> V;
  V(0, 0) = v1[0];
  V(0, 1) = v2[0];
  V(0, 2) = v3[0];
  V(1, 0) = v1[1];
  V(1, 1) = v2[1];
  V(1, 2) = v3[1];
  V(2, 0) = v1[2];
  V(2, 1) = v2[2];
  V(2, 2) = v3[2];

  vnl_svd<Type> svdv(V.as_ref());
  vnl_vector<Type> left_nvvec = svdv.left_nullvector();
  e12_.set(left_nvvec[0], left_nvvec[1], left_nvvec[2]);

  if (fabs(e12_.x()) < tol && fabs(e12_.y()) < tol && fabs(e12_.w()) < tol)
  {
    std::cout << "null e12 - fatal" << std::endl;
    return false;
  }

  vnl_matrix_fixed<Type, 3, 3> U;
  U(0, 0) = u1[0];
  U(0, 1) = u2[0];
  U(0, 2) = u3[0];
  U(1, 0) = u1[1];
  U(1, 1) = u2[1];
  U(1, 2) = u3[1];
  U(2, 0) = u1[2];
  U(2, 1) = u2[2];
  U(2, 2) = u3[2];

  vnl_svd<Type> svdu(U.as_ref());
  vnl_vector<Type> left_nuvec = svdu.left_nullvector();
  e13_.set(left_nuvec[0], left_nuvec[1], left_nuvec[2]);
  if (fabs(e13_.x()) < tol && fabs(e13_.y()) < tol && fabs(e13_.w()) < tol)
  {
    std::cout << "null e13 - fatal" << std::endl;
    return false;
  }
  epipoles_valid_ = true;
  return true;
}

template <class Type>
bool
vpgl_tri_focal_tensor<Type>::compute_f_matrices()
{
  if (f_matrices_1213_valid_)
    return true;
  if (!epipoles_valid_)
    compute_epipoles();
  if (!epipoles_valid_)
  {
    std::cout << "Can't compute f matrices - epipoles not valid" << std::endl;
    return false;
  }

  vnl_vector_fixed<Type, 3> ev12(e12_.x(), e12_.y(), e12_.w()), ev13(e13_.x(), e13_.y(), e13_.w());

  // bit of a pain since cross product matrix isn't defined for a generic type
  // so must convert to double
  vnl_vector_fixed<double, 3> ev12_d(ev12[0], ev12[1], ev12[2]), ev13_d(ev13[0], ev13[1], ev13[2]);

  vnl_matrix_fixed<double, 3, 3> e12x = vnl_cross_product_matrix(ev12_d);
  vnl_matrix_fixed<double, 3, 3> e13x = vnl_cross_product_matrix(ev13_d);

  vnl_matrix_fixed<Type, 3, 3> temp12, temp13, F12_t, F13_t;
  vnl_matrix_fixed<double, 3, 3> temp12_d, temp13_d, cp12, cp13;
  temp12 = dot3(ev13);
  temp13 = dot2(ev12);
  // transpose required
  for (size_t r = 0; r < 3; ++r)
    for (size_t c = 0; c < 3; ++c)
    {
      temp12_d[r][c] = temp12[c][r]; // transpose
      temp13_d[r][c] = temp13[c][r]; // transpose
    }
  cp12 = e12x * temp12_d;
  cp13 = e13x * temp13_d;
  for (size_t r = 0; r < 3; ++r)
    for (size_t c = 0; c < 3; ++c)
    {
      F12_t[r][c] = cp12[r][c];
      F13_t[r][c] = cp13[r][c];
    }
  f12_.set_matrix(F12_t);
  f13_.set_matrix(F13_t);
  f_matrices_1213_valid_ = true;
  return true;
}

template <class Type>
bool
vpgl_tri_focal_tensor<Type>::compute_f_matrix_23()
{
  if (f_matrix_23_valid_)
    return true;
  if (!cameras_valid_)
    return false;
  vpgl_fundamental_matrix<Type> f23(c2_, c3_);
  f23_ = f23;
  f_matrix_23_valid_ = true;
  return true;
}

//-----------------------------------------------------------------------------
//: Read from ASCII std::istream
template <class Type>
std::istream &
operator>>(std::istream & s, vpgl_tri_focal_tensor<Type> & T)
{
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        s >> T(i, j, k);
  return s;
}

//-----------------------------------------------------------------------------
//: Print in ASCII to std::ostream
template <class Type>
std::ostream &
operator<<(std::ostream & s, const vpgl_tri_focal_tensor<Type> & T)
{
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      for (int k = 0; k < 3; ++k)
        vul_printf(s, "%20.16e ", T(i, j, k));
      s << std::endl;
    }
    s << std::endl;
  }
  return s;
}

template <class Type>
bool
within_scale(const vpgl_tri_focal_tensor<Type> & T1, const vpgl_tri_focal_tensor<Type> & T2)
{
  Type max_abs = Type(0);
  size_t max_i = 0, max_j = 0, max_k = 0;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        if (fabs(T1(i, j, k)) > max_abs)
        {
          max_abs = fabs(T1(i, j, k));
          max_i = i;
          max_j = j;
          max_k = k;
        }
  Type scale1 = Type(1) / max_abs;
  Type scale2 = Type(1) / fabs(T2(max_i, max_j, max_k));
  Type rms = Type(0);
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      for (size_t k = 0; k < 3; ++k)
      {
        Type d = T1(i, j, k) * scale1 - T2(i, j, k) * scale2;
        rms += d * d;
      }
  rms /= Type(27);

  if (rms > Type(1e-15))
    return false;

  return true;
}

// Code for easy instantiation.
#undef vpgl_TRI_FOCAL_TENSOR_INSTANTIATE
#define vpgl_TRI_FOCAL_TENSOR_INSTANTIATE(Type)                                                                        \
  template class vpgl_tri_focal_tensor<Type>;                                                                          \
  template std::ostream & operator<<(std::ostream &, const vpgl_tri_focal_tensor<Type> &);                             \
  template std::istream & operator>>(std::istream &, vpgl_tri_focal_tensor<Type> &);                                   \
  template bool within_scale(const vpgl_tri_focal_tensor<Type> &, const vpgl_tri_focal_tensor<Type> &);
#endif // vpgl_tri_focal_tensor_hxx_
