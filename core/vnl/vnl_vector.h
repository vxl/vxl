// This is core/vnl/vnl_vector.h
#ifndef vnl_vector_h_
#define vnl_vector_h_
//:
// \file
// \author Andrew W. Fitzgibbon
//
// \verbatim
// Modifications
// Comments re-written by Tim Cootes, for his sins.
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Mar.2009 - Peter Vanroose - added arg_min() and arg_max()
//   Oct.2010 - Peter Vanroose - mutators and setters now return *this
// \endverbatim
#include <iosfwd>
#include "vnl_error.h"

#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "vnl_c_vector.h"
#include <vnl/vnl_config.h>
#include "vnl_error.h"
#include "vnl/vnl_export.h"
#ifndef NDEBUG
#  if VNL_CONFIG_CHECK_BOUNDS
#    include <cassert>
#  endif
#else
#  undef VNL_CONFIG_CHECK_BOUNDS
#  define VNL_CONFIG_CHECK_BOUNDS 0
#  undef ERROR_CHECKING
#endif
#include "vnl_sse.h"
#include "vnl_math.h"
#include <algorithm>
#include <numeric>

#include <vector>
// #define EIGEN_MATRIXBASE_PLUGIN "vnl_vector_eigen_augment.h"
#include <Eigen/Core>

//template <class T>
//class vnl_matrix;

//----------------------------------------------------------------------

//#define v vnl_vector<T>
//#define m vnl_matrix<T>
//template <class T>
//VNL_EXPORT T
//dot_product(v const &, v const &);
//template <class T>
//VNL_EXPORT T
//inner_product(v const &, v const &);
//template <class T>
//VNL_EXPORT T
//bracket(v const &, m const &, v const &);
//template <class T>
//VNL_EXPORT T
//cos_angle(v const &, v const &);
//template <class T>
//VNL_EXPORT double
//angle(v const &, v const &);
//template <class T>
//VNL_EXPORT m
//outer_product(v const &, v const &);
//template <class T>
//VNL_EXPORT v
//element_product(v const &, v const &);
//template <class T>
//VNL_EXPORT v
//element_quotient(v const &, v const &);
//template <class T>
//VNL_EXPORT T
//vnl_vector_ssd(v const &, v const &);
//template <class T>
//VNL_EXPORT void
//swap(v &, v &) noexcept;
//#undef v
//#undef m

//----------------------------------------------------------------------

//abstract vnl_interface_baseclass to ensure common overrides
//template <typename T>
//class VNL_EXPORT vnl_interface
//{
//public:
//
//};



//: Mathematical vector class, templated by type of element.
// The vnl_vector<T> class implements one-dimensional arithmetic
// vectors to be used with the vnl_matrix<T> class. vnl_vector<T>
// has size fixed by constructor time or changed by assignment
// operator.
// For faster, non-mallocing vectors with size known at compile
// time, use vnl_vector_fixed* or vnl_T_n (e.g. vnl_double_3).
//
// NOTE: Vectors are indexed from zero!  Thus valid elements are [0,size()-1].


#if 1

template <class T>
using vnl_vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;

#else
template <class T>
class VNL_EXPORT vnl_vector : public eigen_vnl_vector_same<T> //, public vnl_interface<T>
{
public:
  using Superclass = eigen_vnl_vector_same<T>;

public:
  friend class vnl_matrix<T>;


  vnl_vector(void):Superclass() {}

  // This constructor allows you to construct MyVectorType from Eigen expressions
  template<typename OtherDerived>
  vnl_vector(const Eigen::MatrixBase<OtherDerived>& other)
    : Superclass(other)
  { }

  // This method allows you to assign Eigen expressions to MyVectorType
  template<typename OtherDerived>
  vnl_vector<T>& operator=(const Eigen::MatrixBase <OtherDerived>& other)
  {
    this->Superclass::operator=(other);
    return *this;
  }

  //: Creates a vector containing n uninitialized elements.
  explicit vnl_vector(size_t len)
    : Superclass(len)
  {}
  vnl_vector(const Superclass & rhs)
    : Superclass(rhs)
  {}
  vnl_vector(Superclass && rhs)
    : Superclass(rhs)
  {}


  //: Creates a vector containing n elements, all set to v0.
  vnl_vector(size_t len, T const & v0);

  //: Creates a vector containing len elements, with the first n
  // elements taken from the array values[]. O(n).
  vnl_vector(size_t len, size_t n, T const values[]);

  //: Creates a vector containing len elements, initialized with values from
  // a data block.
  vnl_vector(T const * data_block, size_t n);

  //: Copy constructor.
  vnl_vector(vnl_vector<T> const &) = default;

  // NOTE: move-assignment must be allowed to throw an exception, because we need to maintain
  //       backwards compatibility and the move-construction & move-aasignment
  //       operators fall back to the copy-assignment operator behavior in
  //       cases when the memory is externally managed.
  //: Move-constructor.
  vnl_vector(vnl_vector<T> &&) noexcept = default;
  //: Move-assignment operator
  vnl_vector<T> &
  operator=(vnl_vector<T> && rhs) = default;

  //: Set all elements to value v
  vnl_vector<T> &
  operator=(T const & v)
  {
    this->setConstant(v);
    return *this;
  }

  //: Copy operator
  vnl_vector<T> &
  operator=(vnl_vector<T> const & rhs)
  {
    if(this != & rhs)
    {
      this->Superclass::operator=(rhs);
    }
    return *this;
  }



  //: Destructor
  /** This destructor *must* be virtual to ensure that the vnl_vector_ref subclass destructor
   * is called and memory is not accidently de-allocated. */
  virtual ~vnl_vector() = default;


  //: Copy elements to ptr[i]
  //  Note: ptr[i] must be valid for i=0..size()-1
  void
  copy_out(T *) const; // from vector to array[].

  //: Sets elements to ptr[i]
  //  Note: ptr[i] must be valid for i=0..size()-1
  vnl_vector &
  set(T const * ptr)
  {
    return copy_in(ptr);
  }

  //: Return reference to the element at specified index.
  // There are assert style boundary checks - #define NDEBUG to turn them off.
  T &
  operator()(size_t i)
  {
#if VNL_CONFIG_CHECK_BOUNDS
    assert(i < size()); // Check the index is valid.
#endif
    return this->Superclass::operator()(i);
  }
  //: Return reference to the element at specified index. No range checking.
  // There are assert style boundary checks - #define NDEBUG to turn them off.
  T const &
  operator()(size_t i) const
  {
#if VNL_CONFIG_CHECK_BOUNDS
    assert(i < size()); // Check the index is valid
#endif
    return this->Superclass::operator()(i);
  }

  //: Return reference to the element at specified index. No range checking.
  T &
  operator[](size_t i)
  {
    return this->Superclass::operator()(i);
  }
  //: Return reference to the element at specified index. No range checking.
  T const &
  operator[](size_t i) const
  {
    return this->Superclass::operator()(i);
  }

  //: Multiply all elements by scalar
  vnl_vector<T> &
  operator*=(T v)
  {
    this->Superclass::operator*=(v);
    return *this;
  }

  //: Divide all elements by scalar
  vnl_vector<T> &
  operator/=(T v)
  {
    this->Superclass::operator/=(v);
    return *this;
  }

  //: Add rhs to this and return *this
  vnl_vector<T> &
  operator+=(vnl_vector<T> const & rhs)
  {
    this->Superclass::operator+=(rhs);
    return *this;
  }

  //:  Mutates lhs vector with its subtraction with rhs vector. O(n).
  vnl_vector<T> &
  operator-=(vnl_vector<T> const & rhs)
  {
    this->Superclass::operator-=(rhs);
    return *this;
  }


  //: *this = M*(*this) where M is a suitable matrix.
  //  this is treated as a column vector
  vnl_vector<T> &
  pre_multiply(vnl_matrix<T> const & M);

  //: *this = (*this)*M where M is a suitable matrix.
  //  this is treated as a row vector
  vnl_vector<T> &
  post_multiply(vnl_matrix<T> const & M);

  //: *this = (*this)*M where M is a suitable matrix.
  //  this is treated as a row vector
  vnl_vector<T> &
  operator*=(vnl_matrix<T> const & m)
  {
    return this->post_multiply(m);
  }



  vnl_vector<T>
  operator+(vnl_vector<T> const & v) const
  {
    return this->Superclass::operator+(v);
  }

  vnl_vector<T>
  operator-(vnl_vector<T> const & v) const
  {
    return this->Superclass::operator-(v);
  }

  vnl_vector<T>
  operator*(vnl_matrix<T> const & M) const
  {
    return this->transpose().operator*(M).transpose().matrix();
  }



  //: Type defs for iterators
  typedef T element_type;
  typedef size_t size_type;

  //: Type defs for iterators
  typedef T * iterator;
  //: Iterator pointing to start of data
  iterator
  begin()
  {
    return this->data();
  }

  //: Iterator pointing to element beyond end of data
  iterator
  end()
  {
    return this->data() + this->size();
  }

  //: Const iterator type
  typedef T const * const_iterator;
  //: Iterator pointing to start of data
  const_iterator
  begin() const
  {
    return this->data();
  }
  const_iterator
  cbegin() const
  {
    return this->data() ? this->data() : nullptr;
  }

  //: Iterator pointing to element beyond end of data
  const_iterator
  end() const
  {
    return this->data() + this->size();
  }
  const_iterator
  cend() const
  {
    return this->data() ? this->data() + this->size() : nullptr;
  }

  //: Analogous to std::vector::front().
  T &
  front()
  {
    return *(this->data());
  }
  //: Analogous to std::vector::back().
  T &
  back()
  {
    return *(this->data() + this->size() - 1);
  }

  //: Analogous to std::vector::front() (const overload).
  const T &
  front() const
  {
    return *(this->data());
  }
  //: Analogous to std::vector::back() (const overload).
  const T &
  back() const
  {
    return *(this->data() + this->size() - 1);
  }

  //: Return a reference to this.
  // Useful in code which would prefer not to know if its argument
  // is a vector, vector_ref or a vector_fixed.  Note that it doesn't
  // return a vector_ref, so it's only useful in templates or macros.
  vnl_vector<T> const &
  as_ref() const
  {
    return *this;
  }

  //: Return a reference to this.
  vnl_vector<T> &
  as_ref()
  {
    return *this;
  }

  //: Applies function to elements
  vnl_vector<T> apply(T (*f)(T)) const;
  //: Applies function to elements
  vnl_vector<T> apply(T (*f)(T const &)) const;

  //: Returns a subvector specified by the start index and length. O(n).
  vnl_vector<T>
  extract(size_t len, size_t start = 0) const;

  //: Replaces elements with index beginning at start, by values of v. O(n).
  vnl_vector<T> &
  update(vnl_vector<T> const &, size_t start = 0);


  //: Mean of values in vector
  // baseclass T mean() const { return this->sum() / static_cast<T>(this->size()); }

  //: Sum of values in a vector
  // baseclass T sum() const { return std::accumulate(this->cbegin(), this->cend(), 0.0); }

  //: Set this to that and that to this
  void
  swap(vnl_vector<T> & that) noexcept;

  //: Check that size()==sz if not, abort();
  // This function does or tests nothing if NDEBUG is defined
  void
  assert_size(size_t VXL_USED_IN_DEBUG(sz)) const
  {
#ifndef NDEBUG
    assert_size_internal(sz);
#endif
  }

  //: Check that this is finite if not, abort();
  // This function does or tests nothing if NDEBUG is defined
  void
  assert_finite() const
  {
#ifndef NDEBUG
    assert_finite_internal();
#endif
  }

  //: Return true if it's finite
  bool
  is_finite() const;

  //: Return true iff all the entries are zero.
  bool
  is_zero() const;

  //: Return true if *this == v
  bool
  operator_eq(vnl_vector<T> const & v) const;

  //: Equality test
  bool
  operator==(vnl_vector<T> const & that) const
  {
    return this->operator_eq(that);
  }

  //: Inequality test
  bool
  operator!=(vnl_vector<T> const & that) const
  {
    return !this->operator_eq(that);
  }

  //: Resize to n elements.
  // This is a destructive resize, in that the old data is lost if size() != \a n before the call.
  // If size() is already \a n, this is a null operation.
  bool
  set_size(size_t n);

  //: Make the vector as if it had been default-constructed.
  void
  clear();

  //: Read from text stream
  bool
  read_ascii(std::istream & s);

  //: Read from text stream
  static vnl_vector<T>
  read(std::istream & s);

  //: Support external memory
  void
  set_data(T * datain, size_t sz, bool LetArrayManageMemory)
  {
    (void) LetArrayManageMemory;
    // Only True is allowed
    assert(LetArrayManageMemory == true);
    this->resize(sz);
    this->copy_in(datain);
  }
  void
  set_data_same_size(T * datain, bool LetArrayManageMemory)
  {
    set_data(datain, this->size(), LetArrayManageMemory);
  }

protected:
  void
  assert_size_internal(size_t sz) const;
  void
  assert_finite_internal() const;
  void
  destroy();

private:
  const bool m_LetArrayManageMemory{ true };
};

//:
// \file
// \author VDN
// \date   Feb 21, 1992
// \brief new lite version adapted from Matrix.h
//
// The parameterized vnl_vector<T> class implements 1D arithmetic vectors of a
// user specified type. The only constraint placed on the type is that
// it must overload the following operators: +, -, *, and /. Thus, it will
// be possible to have a vnl_vector over std::complex<T>.  The vnl_vector<T>
// class is static in size, that is once a vnl_vector<T> of a particular
// size has been declared, elements cannot be added or removed. Using the
// set_size() method causes the vector to resize, but the contents will be
// lost.
//
// Each vector contains  a protected  data section  that has a  T* slot that
// points to the  physical memory allocated  for the one  dimensional array. In
// addition, an integer  specifies   the number  of  elements  for the
// vector.  These values  are provided in the  constructors.
//
// Several constructors are provided. See .h file for descriptions.
//
// Methods   are  provided   for destructive   scalar   and vector  addition,
// multiplication, check for equality  and inequality, fill, reduce, and access
// and set individual elements.  Finally, both  the  input and output operators
// are overloaded to allow for formatted input and output of vector elements.
//
// vnl_vector is a special type of matrix, and is implemented for space and time
// efficiency. When vnl_vector is pre_multiplied by/with matrix, m*v, vnl_vector is
// implicitly a column matrix. When vnl_vector is post_multiplied by/with matrix
// v*m, vnl_vector is implicitly a row matrix.
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include "vnl_vector.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include "vnl_math.h"
#include "vnl_matrix.h"
#include "vnl_numeric_traits.h"

#include "vnl_c_vector.h"

//: Creates a vector of specified length, and initialize all elements with value. O(n).

template<class T>
vnl_vector<T>::vnl_vector (size_t len, T const& value)
  : Eigen::Matrix<T, Eigen::Dynamic, 1>(len)
{
  this->setConstant(value);
}

//: Creates a vector of specified length and initialize first n elements with values. O(n).

template<class T>
vnl_vector<T>::vnl_vector (size_t len, size_t n, T const values[]): Eigen::Matrix<T, Eigen::Dynamic, 1>(len)
{
  // If user specified values, initialize first n elements with values
  // n.b Assignment is used over universal initialization to avoid a
  // gcc 4.8.5 ICE.
  const size_t copy_num = std::min(len,n);
  std::copy(values, values + copy_num, this->data());
}




//: Creates a vector from a block array of data, stored row-wise.
// Values in datablck are copied. O(n).

template<class T>
vnl_vector<T>::vnl_vector (T const* datablck, size_t len): Eigen::Matrix<T, Eigen::Dynamic, 1>(len)
{
  std::copy( datablck, datablck + len, this->data() );
}

//: Frees up the array inside vector. O(1).
template<class T>
void vnl_vector<T>::destroy()
{
  this->resize(0);
}

template<class T>
void vnl_vector<T>::clear()
{
  this->resize(0);
}


//: Sets elements of a vector to those in an array. O(n).

template<class T>
vnl_vector<T>&
vnl_vector<T>::copy_in (T const *ptr)
{
  std::copy( ptr, ptr + this->size(), this->data() );
  return *this;
}

//: Sets elements of an array to those in vector. O(n).

template<class T>
void vnl_vector<T>::copy_out (T *ptr) const
{
  std::copy( this->cbegin(), this->cend(), ptr );
}

//: Copies rhs vector into lhs vector. O(n).
// Changes the dimension of lhs vector if necessary.



//: Pre-multiplies vector with matrix and stores result back in vector.
// v = m * v. O(m*n). Vector is assumed a column matrix.

template<class T>
vnl_vector<T>& vnl_vector<T>::pre_multiply (vnl_matrix<T> const& m)
{
  vnl_vector<T> temp =  m * (*this);
  *this = temp;
  return *this;                                 // Return vector reference
}

//: Post-multiplies vector with matrix and stores result back in vector.
// v = v * m. O(m*n). Vector is assumed a row matrix.

template<class T>
vnl_vector<T>& vnl_vector<T>::post_multiply (vnl_matrix<T> const& m)
{
  auto temp = ((*this) * m).eval();
  *this = temp;
  return *this;                                 // Return vector reference
}


//:
template <class T>
vnl_vector<T> vnl_vector<T>::apply(T (*f)(T const&)) const
{
  vnl_vector<T> ret(this->size());
  vnl_c_vector<T>::apply(this->data(), this->size(), f, ret.data());
  return ret;
}

//: Return the vector made by applying "f" to each element.
template <class T>
vnl_vector<T> vnl_vector<T>::apply(T (*f)(T)) const
{
  vnl_vector<T> ret(this->size());
  vnl_c_vector<T>::apply(this->data(), this->size(), f, ret.data());
  return ret;
}

//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------

// Disable warning caused when T is complex<float>.  The static_cast
// to real_t constructs a complex<float> from a double.
#if defined(_MSC_VER)
# pragma warning (push)
# pragma warning (disable: 4244) /* conversion with loss of data */
#endif

template <class T>
bool vnl_vector<T>::is_finite() const
{
  for (size_t i = 0; i < this->size();++i)
    if (!vnl_math::isfinite( (*this)[i] ))
      return false;

  return true;
}

template <class T>
bool vnl_vector<T>::is_zero() const
{
  T const zero(0);
  for (size_t i = 0; i < this->size();++i)
    if ( !( (*this)[i] == zero) )
      return false;

  return true;
}

template <class T>
void vnl_vector<T>::assert_finite_internal() const
{
  if (this->is_finite())
    return;

  std::cerr << __FILE__ ": *** NAN FEVER **\n" << *this;
  std::abort();
}

template <class T>
void vnl_vector<T>::assert_size_internal(size_t sz) const
{
  if (this->size() != sz) {
    std::cerr << __FILE__ ": Size is " << this->size() << ". Should be " << sz << '\n';
    std::abort();
  }
}



template<class T>
bool vnl_vector<T>::operator_eq (vnl_vector<T> const& rhs) const
{
  if (this == &rhs)                               // same object => equal.
    return true;

  if (this->size() != rhs.size())                 // Size different ?
    return false;                                 // Then not equal.
  for (size_t i = 0; i < this->size(); i++)           // For each index
    if (this->Superclass::operator()(i) != rhs(i))          // Element different ?
      return false;                               // Then not equal.

  return true;                                    // Else same; return true.
}
#endif

//--------------------------------------------------------------------------------

//: Overloads the output operator to print a vector. O(n).

template<class T>
std::ostream& operator<< (std::ostream& s, vnl_vector<T> const& v)
{
  for (size_t i = 0; i+1 < v.size(); ++i)   // For each index in vector
    s << v[i] << ' ';                              // Output data element
  if (v.size() > 0)  s << v[v.size()-1];
  return s;
}

//: Read a vnl_vector from an ascii std::istream.
// If the vector has nonzero size on input, read that many values.
// Otherwise, read to EOF.
template <class T>
std::istream& operator>>(std::istream& s, vnl_vector<T>& M)
{
  M.read_ascii(s); return s;
}


//: Returns new vector whose elements are the products v1[i]*v2[i]. O(n).

template<class T>
vnl_vector<T> element_product (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("element_product", v1.size(), v2.size());
#endif

  vnl_vector<T> result(v1.size());

  vnl_sse<T>::element_product(&(*v1.cbegin()), &(*v2.cbegin()), &(*result.begin()), v1.size());

  return result;
}

//: Returns new vector whose elements are the quotients v1[i]/v2[i]. O(n).

template<class T>
vnl_vector<T> element_quotient (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("element_quotient", v1.size(), v2.size());
#endif
  vnl_vector<T> result(v1.size());
  for (size_t i = 0; i < v1.size(); i++)
    result[i] = v1[i] / v2[i];
  return result;
}



// Definitions of inline functions


#if 0
//: multiply matrix and (column) vector. O(m*n).
// \relatesalso vnl_vector
// \relatesalso vnl_matrix
template <class T>
inline vnl_vector<T>
operator*(vnl_matrix<T> const & M, vnl_vector<T> const & v)
{
  vnl_vector<T> result(M.rows());
#ifndef NDEBUG
  if (M.cols() != v.size())
    vnl_error_vector_dimension("vnl_vector<>::operator*(M, v)", M.cols(), v.size());
#endif
  vnl_sse<T>::matrix_x_vector(M.data(), v.data(), result.data(), M.rows(), M.cols());
  return result;
}
#endif

//
////: add scalar and vector. O(n).
//// \relatesalso vnl_vector
template <class T>
inline vnl_vector<T>
operator+(T s, vnl_vector<T> const & v)
{
  return v.operator+(s);
}

////: subtract vector from scalar. O(n).
//// \relatesalso vnl_vector
template <class T>
inline vnl_vector<T>
operator-(T s, vnl_vector<T> const & v)
{
    vnl_vector<T> result(v.size());
    for(size_t i=0; i< result.size(); ++i)
    {
    result[i] = s - v[i];
    }
    return result;
}



//: Returns the dot product of two nd-vectors, or [v1]*[v2]^T. O(n).

template<class T>
T dot_product (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
    if (v1.size() != v2.size())
    vnl_error_vector_dimension ("dot_product", v1.size(), v2.size());
#endif
    return vnl_c_vector<T>::dot_product(&(*v1.cbegin()),
                                        &(*v2.cbegin()),
                                        v1.size());
}

//: Hermitian inner product. O(n)

template<class T>
T inner_product (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
    if (v1.size() != v2.size())
    vnl_error_vector_dimension ("inner_product", v1.size(), v2.size());
#endif
    //return std::inner_product(v1.cbegin(), v1.cend(),v2.cbegin(), 0);
    return vnl_c_vector<T>::inner_product(&(*v1.cbegin()),
                                          &(*v2.cbegin()),
                                          v1.size());
}


// fsm : cos_angle should return a T, or a double-precision extension
// of T. "double" is wrong since it won't work if T is complex.
template <class T>
T cos_angle(vnl_vector<T> const& a, vnl_vector<T> const& b)
{
    typedef typename vnl_numeric_traits<T>::real_t real_t;
    typedef typename vnl_numeric_traits<T>::abs_t abs_t;
    typedef typename vnl_numeric_traits<abs_t>::real_t abs_r;

    real_t ab = inner_product(a,b);
    real_t a_b = static_cast<real_t>(
      std::sqrt( abs_r(a.squared_magnitude() * b.squared_magnitude()) ));
    return T( ab / a_b);
}

#if defined(_MSC_VER)
# pragma warning (pop)
#endif

//: Returns the smallest angle between two non-zero n-dimensional vectors. O(n).

template<class T>
double angle (vnl_vector<T> const& a, vnl_vector<T> const& b)
{
    typedef typename vnl_numeric_traits<T>::abs_t abs_t;
    typedef typename vnl_numeric_traits<abs_t>::real_t abs_r;
    const abs_r c = abs_r( cos_angle(a, b) );
    // IMS: sometimes cos_angle returns 1+eps, which can mess up std::acos.
    if (c >= 1.0) return 0;
    if (c <= -1.0) return vnl_math::pi;
    return std::acos( c );
}



//: Returns the 'matrix element' <u|A|v> = u^t * A * v. O(mn).

//template<class T>
//T bracket(vnl_vector<T> const &u, vnl_matrix<T> const &A, vnl_vector<T> const &v)
//{
//#ifndef NDEBUG
//    if (u.size() != A.rows())
//    vnl_error_vector_dimension("bracket",u.size(),A.rows());
//    if (A.columns() != v.size())
//    vnl_error_vector_dimension("bracket",A.columns(),v.size());
//#endif
//    T brak(0);
//    for (size_t i=0; i<u.size(); ++i)
//    for (size_t j=0; j<v.size(); ++j)
//      brak += u[i]*A(i,j)*v[j];
//    return brak;
//}

//: Returns the nxn outer product of two nd-vectors, or [v1]^T*[v2]. O(n).

//template<class T>
//vnl_matrix<T> outer_product (vnl_vector<T> const& v1,
//              vnl_vector<T> const& v2) {
//    vnl_matrix<T> out(v1.size(), v2.size());
//    for (size_t i = 0; i < out.rows(); i++)             // v1.column() * v2.row()
//    for (size_t j = 0; j < out.columns(); j++)
//      out[i][j] = v1[i] * v2[j];
//    return out;
//}



//
////: multiply scalar and vector. O(n).
//// \relatesalso vnl_vector
template <class T>
inline vnl_vector<T>
operator*(T s, vnl_vector<T> const & v)
{
  return v * s;
}

//: Interchange the two vectors
// \relatesalso vnl_vector
template <class T>
inline void
swap(vnl_vector<T> & a, vnl_vector<T> & b) noexcept
{
  a.swap(b);
}

//: Euclidean Distance between two vectors.
// Sum of Differences squared.
// \relatesalso vnl_vector
template <class T>
inline T
vnl_vector_ssd(vnl_vector<T> const & v1, vnl_vector<T> const & v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension("vnl_vector_ssd", v1.size(), v2.size());
#endif
  return vnl_c_vector<T>::euclid_dist_sq(v1.data(), v2.data(), v1.size());
}

// Non-vector functions which are nevertheless very useful.

//: Write vector to a std::ostream
// \relatesalso vnl_vector
template <class T>
VNL_EXPORT std::ostream &
operator<<(std::ostream &, vnl_vector<T> const &);
//: Read vector from a std::istream
// \relatesalso vnl_vector
template <class T>
VNL_EXPORT std::istream &
operator>>(std::istream &, vnl_vector<T> &);


#endif // vnl_vector_h_
