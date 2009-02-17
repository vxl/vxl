// This is core/vpdl/vpdl_eigen_sym_matrix.h
#ifndef vpdl_eigen_sym_matrix_h_
#define vpdl_eigen_sym_matrix_h_
//:
// \file
// \author Matthew Leotta
// \date February 16, 2009
// \brief A symmetric matrix represented in eigenvalue decomposition 
//
// \verbatim
// Modifications
//   None
// \endverbatim


#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vpdl/vpdl_base_traits.h>
#include <vcl_limits.h>

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
class vpdl_eigen_sym_matrix
{
public:
  //: the data type used for vectors
  typedef typename vpdl_base_traits<T,n>::vector vector;
  //: the data type used for matrices
  typedef typename vpdl_base_traits<T,n>::matrix matrix;
  
  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_eigen_sym_matrix(unsigned int var_dim = n) 
  {
    vpdl_base_traits<T,n>::set_size(eigen_vec_,var_dim);
    vpdl_base_traits<T,n>::set_size(eigen_val_,var_dim);
    eigen_vec_.set_identity();
    eigen_val_.fill(T(0));
  }
  
  //: Constructor - from eigenvectors and eigenvalues
  vpdl_eigen_sym_matrix(const matrix& evec, const vector& eval) 
  : eigen_vec_(evec), eigen_val_(eval)
  {
    assert(are_evec_orthonormal());
  }
  
  //: Constructor - from symmetric matrix
  vpdl_eigen_sym_matrix(const matrix& m) 
  {
    set_matrix(m);
  }
  
  //: Access to the eigenvectors
  const matrix& eigenvectors() const { return eigen_vec_; }
  
  //: Access to the eigenvalues
  const vector& eigenvalues() const { return eigen_val_; }
  
  //: set the eigenvectors and eigen values by decomposing m
  void set_matrix(const matrix& m)
  {
    const unsigned int dim = vpdl_base_traits<T,n>::m_size(m);
    vpdl_base_traits<T,n>::set_size(eigen_vec_,dim);
    vpdl_base_traits<T,n>::set_size(eigen_val_,dim);
    vnl_symmetric_eigensystem_compute(m, eigen_vec_, eigen_val_);
  }
  
  //: Reform the matrix
  // m = eigen_vec_ * diag(eigen_val_) * eigen_vec_.transpose()
  void form_matrix(matrix& m) const
  {
    const unsigned int dim = eigen_val_.size();
    vpdl_base_traits<T,n>::set_size(m,dim);
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
    vpdl_base_traits<T,n>::set_size(m,dim);
    m.fill(T(0));
    
    
    for (unsigned int k = 0; k < dim; ++k){
      if(eigen_val_[k] <= T(0))
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
  
  //: evaluate the Quadratic form x^t * M * x 
  T quad_form(const vector& x) const
  {
    const unsigned int dim = eigen_val_.size();
    T val = T(0);
    for(unsigned int i = 0; i < dim; ++i){
      T tmp = T(0);
      for(unsigned int j = 0; j < dim; ++j){
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
    for(unsigned int i = 0; i < dim; ++i){
      if(eigen_val_[i] <= T(0))
        return vcl_numeric_limits<T>::infinity();
      T tmp = T(0);
      for(unsigned int j = 0; j < dim; ++j){
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
    for(unsigned int i=0; i<dim; ++i)
      det *= eigen_val_[i];
    return det;
  }
  

private:
  //: the matrix of eigenvectors
  matrix eigen_vec_;
  //: the vector of eigenvalues
  vector eigen_val_;
  
  //: return true if the eigenvectors are (approximately) orthonormal
  bool are_evec_orthonormal() const;
}; 


//: A symmetric matrix represented in eigenvalue decomposition 
// partial specialization for the scalar case
template<class T>
class vpdl_eigen_sym_matrix<T,1>
{
public:
  //: the data type used for vectors 
  typedef typename vpdl_base_traits<T,1>::vector vector;
  //: the data type used for matrices
  typedef typename vpdl_base_traits<T,1>::matrix matrix;
  
  //: Constructor
  // Optionally initialize the dimension for when n==0.
  // Otherwise var_dim is ignored
  vpdl_eigen_sym_matrix(unsigned int var_dim = 1) : var_(T(0)) {}
  
  //: Constructor - from eigenvectors and eigenvalues
  vpdl_eigen_sym_matrix(const matrix& evec, const vector& eval) 
  : var_(eval)
  {
    assert(evec == T(1));
  }
  
  //: Constructor - from symmetric matrix
  vpdl_eigen_sym_matrix(const matrix& m) : var_(m) {}
  
  //: set the eigenvectors and eigen values by decomposing m
  void set_matrix(matrix& m) { var_ = m; }
  
  //: Access to the eigenvectors
  const matrix& eigenvectors() const { return 1; }
  
  //: Access to the eigenvalues
  const vector& eigenvalues() const { return var_; }
  
  //: Reform the matrix
  void form_matrix(matrix& m) const { m = var_; }
  
  //: compute the matrix inverse
  void form_inverse(matrix& m) const { m = 1/var_; }
  
  //: evaluate the Quadratic form x^t * M * x 
  T quad_form(const vector& x) const { return x*x*var_; }
  
  //: evaluate the inverse Quadratic form x^t * M^-1 * x 
  T inverse_quad_form(const vector& x) const { return x*x/var_; }
  
  //: compute the determinant
  T determinant() const { return var_; }
  
  
private:
  //: the scalar variance
  T var_;
}; 


#endif // vpdl_eigen_sym_matrix_h_
