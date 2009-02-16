// This is core/vpdl/vpdl_base_traits.h
#ifndef vpdl_base_traits_h_
#define vpdl_base_traits_h_
//:
// \file
// \author Matthew Leotta
// \brief specialized template base class traits for distributions
// \date February 5, 2009
//
// These template specializations are a base class to vpdl_distribution.
// They provide traits that differ with template arguments to allow for
// a single distribution base class without specializations.
//
// \verbatim
// Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>


//: The traits for different distributions template arguments.
// The vector and matrix data types vary with both T and n.
// - For n > 1 the data types are vnl_vector_fixed<T,n> and vnl_matrix_fixed<T,n,n>
// - For n == 1 the data types are T and T
// - For n == 0 the data types are vnl_vector<T> and vnl_matrix<T>
// 
// The n == 0 indicates that the dimension is dynamic and set at run time.
template<class T, unsigned int n=0>
class vpdl_base_traits
{
public:
  //: The compile time dimension of the distribution
  enum { fixed_dim = n };
  //: the data type used for vectors (e.g. the mean) 
  typedef vnl_vector_fixed<T,n> vector;
  //: the data type used for matrices (e.g. covariance)
  typedef vnl_matrix_fixed<T,n,n> matrix;
  
  //: Constructor - does nothing
  vpdl_base_traits(unsigned int dim=n) {}
  
  //: return the dimension of the space
  unsigned int dimension() const { return n; }  
  //: set the variable dimension
  // the dimension is not variable, so do nothing and hope to be optimized away
  void set_dimension(unsigned int) {}
  
  // functions for uniform array access to all vector and matrix types
  //==================================================================
  
  //: Access vector size
  static inline unsigned int v_size(const vector& v) { return n; }
  //: Access matrix size
  static inline unsigned int m_size(const matrix& m) { return n; }
  
  //: resize a vector - do nothing, fixed size
  static inline void set_size(vector& v, unsigned int s) {}
  //: resize a matrix - do nothing, fixed size
  static inline void set_size(matrix& v, unsigned int s) {}
  
  //: initialize a vector by filling it with val
  static inline void v_init(vector& v, unsigned int s, const T& val)
  { v.fill(val); }
  
  //: index into a vector
  static inline T& index(vector& v, unsigned int i) 
  { return v[i]; }
  //: index into a vector
  static inline const T& index(const vector& v, unsigned int i) 
  { return v[i]; }
  
  //: index into a matrix
  static inline T& index(matrix& m, unsigned int i, unsigned int j) 
  { return m(i,j); }
  //: index into a matrix
  static inline const T& index(const matrix& m, unsigned int i, unsigned int j) 
  { return m(i,j); }
}; 


//: The traits specialization for scalars
// It might be usefull to create light-weight wrappers around T for internal use.
// These could allow operator[] and operator() to simulate array access
// for these scalars and reduce the need for later specialization.
template<class T>
class vpdl_base_traits<T,1>
{
public:
  //: The compile time dimension of the distribution
  enum { fixed_dim = 1 };
  //: the data type used for vectors (e.g. the mean) 
  typedef T vector;
  //: the data type used for matrices (e.g. covariance)
  typedef T matrix;
  
  //: Constructor - does nothing
  vpdl_base_traits(unsigned int dim=1) {}
  
  //: return the dimension of the space
  unsigned int dimension() const { return 1; }  
  //: set the variable dimension
  // the dimension is not variable, so do nothing and hope to be optimized away
  void set_dimension(unsigned int) {}
  
  // functions for uniform array access to all vector and matrix types
  //==================================================================
  
  //: Access vector size
  static inline unsigned int v_size(const vector& v) { return 1; }
  //: Access matrix size
  static inline unsigned int m_size(const matrix& m) { return 1; }
  
  //: resize a vector or matrix - do nothing, fixed size
  static inline void set_size(vector& v, unsigned int s) {}
  
  //: initialize a vector by filling it with val
  static inline void v_init(vector& v, unsigned int s, const T& val)
  { v = val; }
  
  //: index into a vector
  static inline T& index(vector& v, unsigned int i) 
  { return v; }
  //: index into a vector
  static inline const T& index(const vector& v, unsigned int i) 
  { return v; }
  
  //: index into a matrix
  static inline T& index(matrix& m, unsigned int i, unsigned int j) 
  { return m; }
  //: index into a matrix
  static inline const T& index(const matrix& m, unsigned int i, unsigned int j) 
  { return m; }
}; 


//: The traits specialization for dynamic size vectors
template<class T>
class vpdl_base_traits<T,0>
{
public:
  //: The compile time dimension of the distribution
  enum { fixed_dim = 0 };
  //: the data type used for vectors (e.g. the mean) 
  typedef vnl_vector<T> vector;
  //: the data type used for matrices (e.g. covariance)
  typedef vnl_matrix<T> matrix;
  
  //: Constructor - optionally sets the dimension
  vpdl_base_traits(unsigned int dim=0) : var_dim_(dim) {}
  
  //: return the dimension of the space
  unsigned int dimension() const { return var_dim_; }
  //: set the variable dimension
  void set_dimension(unsigned int dim) { var_dim_ = dim; }
  
  // functions for uniform array access to all vector and matrix types
  //==================================================================
  
  //: Access vector size
  static inline unsigned int v_size(const vector& v) { return v.size(); }
  //: Access matrix size
  static inline unsigned int m_size(const matrix& m) 
  { 
    assert(m.cols() == m.rows()); 
    return m.cols(); 
  }
  
  //: resize a vector
  static inline void set_size(vector& v, unsigned int s) 
  { v.set_size(s); }
  //: resize a matrix
  static inline void set_size(matrix& m, unsigned int s) 
  { m.set_size(s,s); }
  
  //: initialize a vector by filling it with val
  static inline void v_init(vector& v, unsigned int s, const T& val)
  { 
    v.set_size(s); 
    v.fill(val); 
  }
   
  //: index into a vector
  static inline T& index(vector& v, unsigned int i) 
  { return v[i]; }
  //: index into a vector
  static inline const T& index(const vector& v, unsigned int i) 
  { return v[i]; }
  
  //: index into a matrix
  static inline T& index(matrix& m, unsigned int i, unsigned int j) 
  { return m(i,j); }
  //: index into a matrix
  static inline const T& index(const matrix& m, unsigned int i, unsigned int j) 
  { return m(i,j); }
  
private:
  //: the variable dimension of the space, set at run-time
  unsigned int var_dim_;
}; 

#endif // vpdl_base_traits_h_
