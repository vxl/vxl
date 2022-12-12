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


#define EIGEN_MATRIXBASE_PLUGIN "vnl_vector_eigen_augment.h"
#include <Eigen/Core>

template <class T>
class vnl_vector;
template <class T>
class vnl_matrix;

//----------------------------------------------------------------------

#define v vnl_vector<T>
#define m vnl_matrix<T>
template <class T>
VNL_EXPORT T
dot_product(v const &, v const &);
template <class T>
VNL_EXPORT T
inner_product(v const &, v const &);
template <class T>
VNL_EXPORT T
bracket(v const &, m const &, v const &);
template <class T>
VNL_EXPORT T
cos_angle(v const &, v const &);
template <class T>
VNL_EXPORT double
angle(v const &, v const &);
template <class T>
VNL_EXPORT m
outer_product(v const &, v const &);
template <class T>
VNL_EXPORT v
element_product(v const &, v const &);
template <class T>
VNL_EXPORT v
element_quotient(v const &, v const &);
template <class T>
VNL_EXPORT T
vnl_vector_ssd(v const &, v const &);
template <class T>
VNL_EXPORT void
swap(v &, v &) noexcept;
#undef v
#undef m

//----------------------------------------------------------------------

//abstract vnl_interface_baseclass to ensure common overrides
//template <typename T>
//class VNL_EXPORT vnl_interface
//{
//public:
//
//};

template <class T>
using eigen_vnl_vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;



//: Mathematical vector class, templated by type of element.
// The vnl_vector<T> class implements one-dimensional arithmetic
// vectors to be used with the vnl_matrix<T> class. vnl_vector<T>
// has size fixed by constructor time or changed by assignment
// operator.
// For faster, non-mallocing vectors with size known at compile
// time, use vnl_vector_fixed* or vnl_T_n (e.g. vnl_double_3).
//
// NOTE: Vectors are indexed from zero!  Thus valid elements are [0,size()-1].

template <class T>
class VNL_EXPORT vnl_vector : public eigen_vnl_vector<T> //, public vnl_interface<T>
{
public:
  using Superclass = eigen_vnl_vector<T>;

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

  //: Put value at given position in vector.
  inline void
  put(size_t i, T const & v);

  //: Get value at element i
  inline T
  get(size_t i) const;

  //: Set all values to v
  // baseclass vnl_vector& fill(T const& v);

  //: Sets elements to ptr[i]
  //  Note: ptr[i] must be valid for i=0..size()-1
  vnl_vector &
  copy_in(T const * ptr);

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


  //: Add scalar value to all elements

  vnl_vector<T> &
  operator+=(T value)
  {
    this->Superclass::array() += value;
    return *this;
  }

  //: Subtract scalar value from all elements
  vnl_vector<T> &
  operator-=(T value)
  {
    this->Superclass::array() -= value;
    return *this;
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

  //: Unary plus operator
  // Return new vector = (*this)
  vnl_vector<T>
  operator+() const
  {
    return *this;
  }


  using Superclass::operator<<;

  vnl_vector<T>
  operator-() const
  {
    return this->Superclass::operator-();
  }

  //
  vnl_vector<T>
  operator+(T v) const
  {
    return Superclass(this->array() + v);
  }
  vnl_vector<T>
  operator-(T v) const
  {
    return Superclass(this->array() - v);
  }
  vnl_vector<T>
  operator*(T v) const
  {
    return this->Superclass::operator*(v);
  }
  vnl_vector<T>
  operator/(T v) const
  {
    return this->Superclass::operator/(v);
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

  //--------------------------------------------------------------------------------

  //: Access the contiguous block storing the elements in the vector. O(1).
  //  data_block()[0] is the first element of the vector
  T const *
  data_block() const
  {
    return this->data();
  }

  //: Access the contiguous block storing the elements in the vector. O(1).
  //  data_block()[0] is the first element of the vector
  T *
  data_block()
  {
    return this->data();
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

  // norms etc
  typedef typename vnl_c_vector<T>::abs_t abs_t;

  //: Return sum of squares of elements
  abs_t
  squared_magnitude() const
  {
    return vnl_c_vector<T>::two_nrm2(this->data(), this->size());
  }

  //: Return magnitude (length) of vector
  abs_t
  magnitude() const
  {
    return two_norm();
  }

  //: Return sum of absolute values of the elements
  abs_t
  one_norm() const
  {
    return vnl_c_vector<T>::one_norm(this->data(), this->size());
  }

  //: Return sqrt of sum of squares of values of elements
  abs_t
  two_norm() const
  {
    return vnl_c_vector<T>::two_norm(this->data(), this->size());
  }

  //: Return largest absolute element value
  abs_t
  inf_norm() const
  {
    return vnl_c_vector<T>::inf_norm(this->data(), this->size());
  }

  //: Normalise by dividing through by the magnitude
  vnl_vector<T> &
  normalize()
  {
    this->Superclass::normalize();
    return *this;
  }

  // These next 6 functions are should really be helper functions since they aren't
  // really proper functions on a vector in a philosophical sense.

  //: Root Mean Squares of values
  abs_t rms() const { return vnl_c_vector<T>::rms_norm(begin(), this->size()); }

  //: Smallest value
  T
  min_value() const
  {
    return vnl_c_vector<T>::min_value(this->data(), this->size());
  }

  //: Largest value
  T
  max_value() const
  {
    return vnl_c_vector<T>::max_value(this->data(), this->size());
  }

  //: Location of smallest value
  size_t
  arg_min() const
  {
    return vnl_c_vector<T>::arg_min(this->data(), this->size());
  }

  //: Location of largest value
  size_t
  arg_max() const
  {
    return vnl_c_vector<T>::arg_max(this->data(), this->size());
  }

  //: Mean of values in vector
  // baseclass T mean() const { return this->sum() / static_cast<T>(this->size()); }

  //: Sum of values in a vector
  // baseclass T sum() const { return std::accumulate(this->cbegin(), this->cend(), 0.0); }

  //: Reverse the order of the elements
  //  Element i swaps with element size()-1-i
  vnl_vector<T> &
  flip();

  //: Reverse the order of the elements from index b to 1-e, inclusive.
  //  When b = 0 and e = size(), this is equivalent to flip();
  vnl_vector<T> &
  flip(const size_t & b, const size_t & e);

  //: Roll the vector forward by the specified shift.
  //  The shift is cyclical, such that the elements which
  //  are displaced from the end reappear at the beginning.
  //  Negative shifts and shifts >= the length of the array are supported.
  //  A new vector is returned; the underlying data is unchanged.
  vnl_vector<T>
  roll(const int & shift) const;

  //: Roll the vector forward by the specified shift.
  //  The shift is cyclical, such that the elements which
  //  are displaced from the end reappear at the beginning.
  //  Negative shifts and shifts >= the length of the array are supported.
  //
  vnl_vector &
  roll_inplace(const int & shift); // NOT SUPPORTED!

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

  //: Return true iff the size is zero.
  bool
  empty() const
  {
    return !this->size();
  }

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


// Definitions of inline functions


//: Gets the element at specified index and return its value. O(1).
// Range check is performed.

template <class T>
inline T
vnl_vector<T>::get(size_t i) const
{
#if VNL_CONFIG_CHECK_BOUNDS
  if (i >= this->size())              // If invalid index specified
    vnl_error_vector_index("get", i); // Raise exception
#endif
  return (*this)[i];
}

//: Puts the value at specified index. O(1).
// Range check is performed.

template <class T>
inline void
vnl_vector<T>::put(size_t i, T const & v)
{
#if VNL_CONFIG_CHECK_BOUNDS
  if (i >= this->size())              // If invalid index specified
    vnl_error_vector_index("put", i); // Raise exception
#endif
  (*this)[i] = v; // Assign data value
}

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
