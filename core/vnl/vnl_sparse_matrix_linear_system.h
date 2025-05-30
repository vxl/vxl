// This is core/vnl/vnl_sparse_matrix_linear_system.h
#ifndef vnl_sparse_matrix_linear_system_h_
#define vnl_sparse_matrix_linear_system_h_
//:
//  \file
//  \brief vnl_sparse_matrix -> vnl_linear_system adaptor
//
//  An adaptor that converts a vnl_sparse_matrix<T> to a vnl_linear_system
//
//  \author David Capel, capes@robots
//  \date   July 2000
//
// \verbatim
//  Modifications
//  LSB (Manchester) 19/3/01 Documentation tidied
// \endverbatim
//
//-----------------------------------------------------------------------------

#include "vnl_linear_system.h"
#include "vnl_sparse_matrix.h"
#include "vnl/vnl_export.h"

//: vnl_sparse_matrix -> vnl_linear_system adaptor
//  An adaptor that converts a vnl_sparse_matrix<T> to a vnl_linear_system
template <class T>
class VNL_EXPORT vnl_sparse_matrix_linear_system : public vnl_linear_system
{
public:
  //::Constructor from vnl_sparse_matrix<double> for system Ax = b
  // Keeps a reference to the original sparse matrix A and vector b so DO NOT DELETE THEM!!
  vnl_sparse_matrix_linear_system(const vnl_sparse_matrix<T> & A, const vnl_vector<T> & b)
    : vnl_linear_system(A.columns(), A.rows())
    , A_(A)
    , b_(b)
    , jacobi_precond_()
  {}

  //:  Implementations of the vnl_linear_system virtuals.
  void
  multiply(const vnl_vector<double> & x, vnl_vector<double> & b) const override;
  //:  Implementations of the vnl_linear_system virtuals.
  void
  transpose_multiply(const vnl_vector<double> & b, vnl_vector<double> & x) const override;
  //:  Implementations of the vnl_linear_system virtuals.
  void
  get_rhs(vnl_vector<double> & b) const override;
  //:  Implementations of the vnl_linear_system virtuals.
  void
  apply_preconditioner(const vnl_vector<double> & x, vnl_vector<double> & px) const override;

protected:
  const vnl_sparse_matrix<T> & A_;
  const vnl_vector<T> & b_;
  vnl_vector<double> jacobi_precond_;
};

template <>
VNL_EXPORT void
vnl_sparse_matrix_linear_system<double>::get_rhs(vnl_vector<double> & b) const;
template <>
VNL_EXPORT void
vnl_sparse_matrix_linear_system<double>::transpose_multiply(const vnl_vector<double> & b, vnl_vector<double> & x) const;
template <>
VNL_EXPORT void
vnl_sparse_matrix_linear_system<float>::get_rhs(vnl_vector<double> & b) const;
template <>
VNL_EXPORT void
vnl_sparse_matrix_linear_system<float>::transpose_multiply(const vnl_vector<double> & b, vnl_vector<double> & x) const;
template <>
VNL_EXPORT void
vnl_sparse_matrix_linear_system<double>::multiply(const vnl_vector<double> & x, vnl_vector<double> & b) const;
template <>
VNL_EXPORT void
vnl_sparse_matrix_linear_system<float>::multiply(const vnl_vector<double> & x, vnl_vector<double> & b) const;

#endif // vnl_sparse_matrix_linear_system_h_
