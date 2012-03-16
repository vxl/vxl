#include "brip_gain_offset_solver.h"
//:
// \file
#include <vil/vil_image_view.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

//: compute the number of valid corresponding pixels in the case of masks
void brip_gain_offset_solver::compute_valid_pix()
{
  unsigned m_msk_ni = model_mask_.ni(), m_msk_nj = model_mask_.nj();
  unsigned t_msk_ni = test_mask_.ni(), t_msk_nj = test_mask_.nj();
  m_mask_ = m_msk_ni*m_msk_nj>0;
  t_mask_ = t_msk_ni*t_msk_nj>0;

  if (m_mask_ && (m_msk_ni!=ni_||m_msk_nj!=nj_)) {
    n_valid_pix_ = 0;
    return;
  }
  if (t_mask_ && (t_msk_ni!=ni_||t_msk_nj!=nj_)) {
    n_valid_pix_ = 0;
    return;
  }
  // four cases
  // both masks empty
  if (!t_mask_ && !m_mask_) {
    n_valid_pix_ = ni_*nj_;
    return;
  }
  if (!t_mask_ && m_mask_) {
    n_valid_pix_ = 0;
    for (unsigned j = 0; j<m_msk_nj; ++j)
      for (unsigned i = 0; i<m_msk_ni; ++i)
        if (model_mask_(i,j))
          n_valid_pix_++;
    return;
  }
  if (t_mask_ && !m_mask_) {
    n_valid_pix_ = 0;
    for (unsigned j = 0; j<t_msk_nj; ++j)
      for (unsigned i = 0; i<t_msk_ni; ++i)
        if (test_mask_(i,j))
          n_valid_pix_++;
    return;
  }
  if (t_mask_ && m_mask_) {
    n_valid_pix_ = 0;
    for (unsigned j = 0; j<t_msk_nj; ++j)
      for (unsigned i = 0; i<t_msk_ni; ++i)
        if (test_mask_(i,j)&&model_mask_(i,j))
          n_valid_pix_++;
    return;
  }
}

brip_gain_offset_solver::
brip_gain_offset_solver(vil_image_view<float> const& model_image,
                        vil_image_view<float> const& test_image)
: ni_(model_image.ni()), nj_(model_image.nj()),
  model_image_(model_image), test_image_(test_image),
  gain_(1.0f), offset_(0.0f),
  t_mask_(false), m_mask_(false), n_valid_pix_(0)
{}

brip_gain_offset_solver::
brip_gain_offset_solver(vil_image_view<float> const& model_image,
                        vil_image_view<float> const& test_image,
                        vil_image_view<unsigned char> const& model_mask,
                        vil_image_view<unsigned char> const& test_mask)
: ni_(model_image.ni()), nj_(model_image.nj()),
  model_image_(model_image), test_image_(test_image),
  gain_(1.0f), offset_(0.0f),
  model_mask_(model_mask), test_mask_(test_mask),
  t_mask_(false), m_mask_(false), n_valid_pix_(0)
{}

//:
// Im = model_image intensity, It = test_image intensity
//
// each pixel defines an equation
//
//  gain*It(i,j) + off  = Im(i,j)
//
//  Define the matrix A, and vector b
// \verbatim
//     [It(0,0)        1]     [Im(0,0)      ]
//     [It(1,0)        1]     [Im(1,0)      ]
// A = [     ...        ]  b =[  ...        ]
//     [It(ni-1, nj-1) 1]     [Im(ni-1,nj-1)]
// \endverbatim
//  Use SVD to solve A [gain] = b
//                     [off]
//
bool brip_gain_offset_solver::solve()
{
  if (test_image_.ni() !=ni_ ||
      test_image_.nj() !=nj_ ||
      model_image_.ni()!=ni_ ||
      model_image_.nj()!=nj_)
    return false;
  this->compute_valid_pix();
  if (n_valid_pix_ == 0)
    return false;
  vnl_matrix<double> A(n_valid_pix_, 2);
  vnl_matrix<double> b(n_valid_pix_, 1);
  unsigned r = 0;
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      bool t_valid = !t_mask_ || test_mask_(i,j);
      bool m_valid = !m_mask_ || model_mask_(i,j);
      if (t_valid&&m_valid) {
      A[r][0]=test_image_(i,j);    A[r][1]=1.0;
      b[r][0]=model_image_(i,j);
      ++r;
      }
    }
  vnl_svd<double> svd(A);
  vnl_matrix<double> x = svd.solve(b);
  gain_ = static_cast<float>(x[0][0]);
  offset_ = static_cast<float>(x[1][0]);
  return true;
}

vil_image_view<float> brip_gain_offset_solver::mapped_test_image()
{
  vil_image_view<float> temp(ni_, nj_);
  temp.fill(0.0f);
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      bool t_valid = !t_mask_ || test_mask_(i,j);
      bool m_valid = !m_mask_ || model_mask_(i,j);
      if (t_valid&&m_valid) {
        float v = test_image_(i,j);
        temp(i,j) = gain_*v + offset_;
      }
    }
  return temp;
}
