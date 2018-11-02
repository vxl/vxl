// This is core/vpdl/vpdt/vpdt_eigen_sym_matrix.h
#ifndef vpdt_eigen_sym_matrix_h_
#define vpdt_eigen_sym_matrix_h_
//:
// \file
// \author Matthew Leotta
// \date March 5, 2009
// \brief A symmetric matrix represented in eigenvalue decomposition
//
// \verbatim
// Modifications
//   <None yet>
// \endverbatim

#include <limits>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_field_default.h>
#include <vpdl/vpdt/vpdt_access.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//: wrapper for the vnl eigensystem function for fixed size data
template<class T, unsigned int n>
inline void vnl_symmetric_eigensystem_compute(const vnl_matrix_fixed<T,n,n>& A,
                                              vnl_matrix_fixed<T,n,n>& V,
                                              vnl_vector_fixed<T,n>& D)
{
  vnl_matrix_ref<T> Vr(n,n,V.data_block());
  vnl_vector_ref<T> Dr(n,D.data_block());
  vnl_symmetric_eigensystem_compute(A.as_ref(),Vr,Dr);
}


//: A symmetric matrix represented in eigenvalue decomposition
template<class T, unsigned int n=0>
class vpdt_eigen_sym_matrix
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;

  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdt_eigen_sym_matrix(unsigned int var_dim = n)
  {
    vpdt_set_size(eigen_vec_,var_dim);
    vpdt_set_size(eigen_val_,var_dim);
    eigen_vec_.set_identity();
    eigen_val_.fill(T(0));
  }

  //: Constructor - from eigenvectors and eigenvalues
  vpdt_eigen_sym_matrix(const matrix& evec, const vector& eval)
  : eigen_vec_(evec), eigen_val_(eval)
  {
    assert(are_evec_orthonormal());
  }

  //: Constructor - from symmetric matrix
  vpdt_eigen_sym_matrix(const matrix& m)
  {
    set_matrix(m);
  }

  //: Return the dimension
  unsigned int dimension() const { return eigen_val_.size(); }

  //: Access to the eigenvectors
  const matrix& eigenvectors() const { return eigen_vec_; }

  //: Access to the eigenvalues
  const vector& eigenvalues() const { return eigen_val_; }

  //: Set the eigenvectors
  void set_eigenvectors(const matrix& m)
  {
    eigen_vec_ = m;
    assert(are_evec_orthonormal());
  }

  //: set the eigenvalues
  void set_eigenvalues(const vector& v) { eigen_val_ = v; }

  //: Set the size (if variable) and reset to default
  void set_size(unsigned int dim)
  {
    vpdt_set_size(eigen_vec_,dim);
    vpdt_set_size(eigen_val_,dim);
    eigen_vec_.set_identity();
    eigen_val_.fill(T(0));
  }

  //: set the eigenvectors and eigen values by decomposing m
  void set_matrix(const matrix& m)
  {
    const unsigned int dim = vpdt_size(m);
    vpdt_set_size(eigen_vec_,dim);
    vpdt_set_size(eigen_val_,dim);
    vnl_symmetric_eigensystem_compute(m, eigen_vec_, eigen_val_);
  }

  //: multiply the matrix by a scalar
  vpdt_eigen_sym_matrix<T,n>& operator*=(const T& val)
  {
    const unsigned int dim = eigen_val_.size();
    for (unsigned int i=0; i<dim; ++i)
      eigen_val_[i] *= val;
    return *this;
  }

  //: Reform the matrix
  // m = eigen_vec_ * diag(eigen_val_) * eigen_vec_.transpose()
  void form_matrix(matrix& m) const
  {
    const unsigned int dim = eigen_val_.size();
    vpdt_set_size(m,dim);
    m.fill(T(0));

    for (unsigned int i = 0; i < dim; ++i)
      for (unsigned int k = 0; k < dim; ++k){
        T tmp =  eigen_vec_[i][k] * eigen_val_[k];
        for (unsigned int j = i; j < dim; ++j)
          m[i][j] += tmp * eigen_vec_[j][k];
      }

    // fill in other side of diagonal
    for (unsigned int i = 0; i < dim; ++i)
      for (unsigned int j = i+1; j < dim; ++j)
        m[j][i] = m[i][j];
  }

  //: compute the matrix inverse
  // m = eigen_vec_ * inverse(diag(eigen_val_)) * eigen_vec_.transpose()
  void form_inverse(matrix& m) const
  {
    const unsigned int dim = eigen_val_.size();
    vpdt_set_size(m,dim);
    m.fill(T(0));

    for (unsigned int k = 0; k < dim; ++k){
      if (eigen_val_[k] <= T(0))
        continue;
      for (unsigned int i = 0; i < dim; ++i){
        T tmp =  eigen_vec_[i][k] / eigen_val_[k];
        for (unsigned int j = i; j < dim; ++j)
          m[i][j] += tmp * eigen_vec_[j][k];
      }
    }

    // fill in other side of diagonal
    for (unsigned int i = 0; i < dim; ++i)
      for (unsigned int j = i+1; j < dim; ++j)
        m[j][i] = m[i][j];
  }

  //: evaluate y = M * x
  void product(const vector& x, vector& y) const
  {
    const unsigned int dim = eigen_val_.size();
    vpdt_set_size(y,dim);
    vpdt_fill(y,T(0));
    for (unsigned int i = 0; i < dim; ++i){
      T t_i = T(0);
      for (unsigned int j = 0; j < dim; ++j){
        t_i += eigen_vec_[j][i] * x[j];
      }
      t_i *= eigen_val_[i];
      for (unsigned int j = 0; j < dim; ++j){
        y[j] += eigen_vec_[j][i] * t_i;
      }
    }
  }

  //: evaluate y = M^-1 * x
  void inverse_product(const vector& x, vector& y) const
  {
    const unsigned int dim = eigen_val_.size();
    vpdt_set_size(y,dim);
    vpdt_fill(y,T(0));
    for (unsigned int i = 0; i < dim; ++i){
      T t_i = T(0);
      for (unsigned int j = 0; j < dim; ++j){
        t_i += eigen_vec_[j][i] * x[j];
      }
      t_i /= eigen_val_[i];
      for (unsigned int j = 0; j < dim; ++j){
        y[j] += eigen_vec_[j][i] * t_i;
      }
    }
  }

  //: evaluate the Quadratic form x^t * M * x
  T quad_form(const vector& x) const
  {
    const unsigned int dim = eigen_val_.size();
    T val = T(0);
    for (unsigned int i = 0; i < dim; ++i){
      T tmp = T(0);
      for (unsigned int j = 0; j < dim; ++j){
        tmp += eigen_vec_[j][i] * x[j];
      }
      val += tmp*tmp*eigen_val_[i];
    }
    return val;
  }

  //: evaluate the inverse Quadratic form x^t * M^-1 * x
  T inverse_quad_form(const vector& x) const
  {
    const unsigned int dim = eigen_val_.size();
    T val = T(0);
    for (unsigned int i = 0; i < dim; ++i){
      if (eigen_val_[i] <= T(0))
        return std::numeric_limits<T>::infinity();
      T tmp = T(0);
      for (unsigned int j = 0; j < dim; ++j){
        tmp += eigen_vec_[j][i] * x[j];
      }
      val += tmp*tmp/eigen_val_[i];
    }
    return val;
  }

  //: compute the determinant
  T determinant() const
  {
    const unsigned int dim = eigen_val_.size();
    T det = T(1);
    for (unsigned int i=0; i<dim; ++i)
      det *= eigen_val_[i];
    return det;
  }

 private:
  //: the matrix of eigenvectors
  matrix eigen_vec_;
  //: the vector of eigenvalues
  vector eigen_val_;

  //: return true if the eigenvectors are (approximately) orthonormal
  bool are_evec_orthonormal() const
  {
    // FIXME: implement this
    return false;
  }
};


//: A symmetric matrix represented in eigenvalue decomposition
// This partial specialization for the scalar case is no longer needed
// If you get an error related to this, you should be using a scalar instead.
template<class T>
class vpdt_eigen_sym_matrix<T,1> {};


//==============================================================================
// These type generators produce a vpdt_eigen_sym_matrix for a field type

//: generate the vpdt_eigen_sys_matrix type from a field type
template <class F, class Disambiguate= void>
struct vpdt_eigen_sym_matrix_gen;

//: generate the vpdt_eigen_sys_matrix type from a field type
template <class F>
struct vpdt_eigen_sym_matrix_gen<F,typename vpdt_field_traits<F>::type_is_vector>
{
  typedef vpdt_eigen_sym_matrix<typename vpdt_field_traits<F>::scalar_type,
                                vpdt_field_traits<F>::dimension> type;
};

//: generate the vpdt_eigen_sys_matrix type from a field type
template <class F>
struct vpdt_eigen_sym_matrix_gen<F,typename vpdt_field_traits<F>::type_is_scalar>
{
  typedef typename vpdt_field_traits<F>::matrix_type type;
};


//==============================================================================
// universal access functions (See vpdt_access.h)

//: Set the size of a vpdt_eigen_sym_matrix
template <class T>
inline void vpdt_set_size(vpdt_eigen_sym_matrix<T,0>& m, unsigned int s)
{
  m.set_size(s);
}


//: Fill a vpdt_eigen_sym_matrix
template <class T, unsigned int n>
inline void vpdt_fill(vpdt_eigen_sym_matrix<T,n>& m, const T& val)
{
  typename vpdt_eigen_sym_matrix<T,n>::vector v;
  vpdt_set_size(v,m.dimension());
  vpdt_fill(v,val);
  m.set_eigenvalues(v);
}


#endif // vpdt_eigen_sym_matrix_h_
