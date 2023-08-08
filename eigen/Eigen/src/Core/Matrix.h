// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2006-2010 Benoit Jacob <jacob.benoit.1@gmail.com>
// Copyright (C) 2008-2009 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef EIGEN_MATRIX_H
#define EIGEN_MATRIX_H

#include "./InternalHeaderCheck.h"

namespace Eigen {

namespace internal {
template <typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
struct traits<Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>> {
 private:
  constexpr static int size = internal::size_at_compile_time(Rows_, Cols_);
  typedef typename find_best_packet<Scalar_, size>::type PacketScalar;
  enum {
    row_major_bit = Options_ & RowMajor ? RowMajorBit : 0,
    is_dynamic_size_storage = MaxRows_ == Dynamic || MaxCols_ == Dynamic,
    max_size = is_dynamic_size_storage ? Dynamic : MaxRows_ * MaxCols_,
    default_alignment = compute_default_alignment<Scalar_, max_size>::value,
    actual_alignment = ((Options_ & DontAlign) == 0) ? default_alignment : 0,
    required_alignment = unpacket_traits<PacketScalar>::alignment,
    packet_access_bit = (packet_traits<Scalar_>::Vectorizable &&
                         (EIGEN_UNALIGNED_VECTORIZE || (actual_alignment >= required_alignment)))
                            ? PacketAccessBit
                            : 0
  };

 public:
  typedef Scalar_ Scalar;
  typedef Dense StorageKind;
  typedef Eigen::Index StorageIndex;
  typedef MatrixXpr XprKind;
  enum {
    RowsAtCompileTime = Rows_,
    ColsAtCompileTime = Cols_,
    MaxRowsAtCompileTime = MaxRows_,
    MaxColsAtCompileTime = MaxCols_,
    Flags = compute_matrix_flags(Options_),
    Options = Options_,
    InnerStrideAtCompileTime = 1,
    OuterStrideAtCompileTime = (Options & RowMajor) ? ColsAtCompileTime : RowsAtCompileTime,

    // FIXME, the following flag in only used to define NeedsToAlign in PlainObjectBase
    EvaluatorFlags = LinearAccessBit | DirectAccessBit | packet_access_bit | row_major_bit,
    Alignment = actual_alignment
  };
};
}  // namespace internal

/** \class Matrix
 * \ingroup Core_Module
 *
 * \brief The matrix class, also used for vectors and row-vectors
 *
 * The %Matrix class is the work-horse for all \em dense (\ref dense "note") matrices and vectors within Eigen.
 * Vectors are matrices with one column, and row-vectors are matrices with one row.
 *
 * The %Matrix class encompasses \em both fixed-size and dynamic-size objects (\ref fixedsize "note").
 *
 * The first three template parameters are required:
 * \tparam Scalar_ Numeric type, e.g. float, double, int or std::complex<float>.
 *                 User defined scalar types are supported as well (see \ref user_defined_scalars "here").
 * \tparam Rows_ Number of rows, or \b Dynamic
 * \tparam Cols_ Number of columns, or \b Dynamic
 *
 * The remaining template parameters are optional -- in most cases you don't have to worry about them.
 * \tparam Options_ A combination of either \b #RowMajor or \b #ColMajor, and of either
 *                 \b #AutoAlign or \b #DontAlign.
 *                 The former controls \ref TopicStorageOrders "storage order", and defaults to column-major. The latter
 * controls alignment, which is required for vectorization. It defaults to aligning matrices except for fixed sizes that
 * aren't a multiple of the packet size. \tparam MaxRows_ Maximum number of rows. Defaults to \a Rows_ (\ref maxrows
 * "note"). \tparam MaxCols_ Maximum number of columns. Defaults to \a Cols_ (\ref maxrows "note").
 *
 * Eigen provides a number of typedefs covering the usual cases. Here are some examples:
 *
 * \li \c Matrix2d is a 2x2 square matrix of doubles (\c Matrix<double, 2, 2>)
 * \li \c Vector4f is a vector of 4 floats (\c Matrix<float, 4, 1>)
 * \li \c RowVector3i is a row-vector of 3 ints (\c Matrix<int, 1, 3>)
 *
 * \li \c MatrixXf is a dynamic-size matrix of floats (\c Matrix<float, Dynamic, Dynamic>)
 * \li \c VectorXf is a dynamic-size vector of floats (\c Matrix<float, Dynamic, 1>)
 *
 * \li \c Matrix2Xf is a partially fixed-size (dynamic-size) matrix of floats (\c Matrix<float, 2, Dynamic>)
 * \li \c MatrixX3d is a partially dynamic-size (fixed-size) matrix of double (\c Matrix<double, Dynamic, 3>)
 *
 * See \link matrixtypedefs this page \endlink for a complete list of predefined \em %Matrix and \em Vector typedefs.
 *
 * You can access elements of vectors and matrices using normal subscripting:
 *
 * \code
 * Eigen::VectorXd v(10);
 * v[0] = 0.1;
 * v[1] = 0.2;
 * v(0) = 0.3;
 * v(1) = 0.4;
 *
 * Eigen::MatrixXi m(10, 10);
 * m(0, 1) = 1;
 * m(0, 2) = 2;
 * m(0, 3) = 3;
 * \endcode
 *
 * This class can be extended with the help of the plugin mechanism described on the page
 * \ref TopicCustomizing_Plugins by defining the preprocessor symbol \c EIGEN_MATRIX_PLUGIN.
 *
 * <i><b>Some notes:</b></i>
 *
 * <dl>
 * <dt><b>\anchor dense Dense versus sparse:</b></dt>
 * <dd>This %Matrix class handles dense, not sparse matrices and vectors. For sparse matrices and vectors, see the
 * Sparse module.
 *
 * Dense matrices and vectors are plain usual arrays of coefficients. All the coefficients are stored, in an ordinary
 * contiguous array. This is unlike Sparse matrices and vectors where the coefficients are stored as a list of nonzero
 * coefficients.</dd>
 *
 * <dt><b>\anchor fixedsize Fixed-size versus dynamic-size:</b></dt>
 * <dd>Fixed-size means that the numbers of rows and columns are known are compile-time. In this case, Eigen allocates
 * the array of coefficients as a fixed-size array, as a class member. This makes sense for very small matrices,
 * typically up to 4x4, sometimes up to 16x16. Larger matrices should be declared as dynamic-size even if one happens to
 * know their size at compile-time.
 *
 * Dynamic-size means that the numbers of rows or columns are not necessarily known at compile-time. In this case they
 * are runtime variables, and the array of coefficients is allocated dynamically on the heap.
 *
 * Note that \em dense matrices, be they Fixed-size or Dynamic-size, <em>do not</em> expand dynamically in the sense of
 * a std::map. If you want this behavior, see the Sparse module.</dd>
 *
 * <dt><b>\anchor maxrows MaxRows_ and MaxCols_:</b></dt>
 * <dd>In most cases, one just leaves these parameters to the default values.
 * These parameters mean the maximum size of rows and columns that the matrix may have. They are useful in cases
 * when the exact numbers of rows and columns are not known are compile-time, but it is known at compile-time that they
 * cannot exceed a certain value. This happens when taking dynamic-size blocks inside fixed-size matrices: in this case
 * MaxRows_ and MaxCols_ are the dimensions of the original matrix, while Rows_ and Cols_ are Dynamic.</dd>
 * </dl>
 *
 * <i><b>ABI and storage layout</b></i>
 *
 * The table below summarizes the ABI of some possible Matrix instances which is fixed thorough the lifetime of Eigen 3.
 * <table  class="manual">
 * <tr><th>Matrix type</th><th>Equivalent C structure</th></tr>
 * <tr><td>\code Matrix<Scalar,Dynamic,Dynamic> \endcode</td><td>\code
 * struct {
 *   Scalar *data;                  // with (size_t(data)%EIGEN_MAX_ALIGN_BYTES)==0
 *   Eigen::Index rows, cols;
 *  };
 * \endcode</td></tr>
 * <tr class="alt"><td>\code
 * Matrix<Scalar,Dynamic,1>
 * Matrix<Scalar,1,Dynamic> \endcode</td><td>\code
 * struct {
 *   Scalar *data;                  // with (size_t(data)%EIGEN_MAX_ALIGN_BYTES)==0
 *   Eigen::Index size;
 *  };
 * \endcode</td></tr>
 * <tr><td>\code Matrix<Scalar,Rows,Cols> \endcode</td><td>\code
 * struct {
 *   Scalar data[Rows*Cols];        // with (size_t(data)%A(Rows*Cols*sizeof(Scalar)))==0
 *  };
 * \endcode</td></tr>
 * <tr class="alt"><td>\code Matrix<Scalar,Dynamic,Dynamic,0,MaxRows,MaxCols> \endcode</td><td>\code
 * struct {
 *   Scalar data[MaxRows*MaxCols];  // with (size_t(data)%A(MaxRows*MaxCols*sizeof(Scalar)))==0
 *   Eigen::Index rows, cols;
 *  };
 * \endcode</td></tr>
 * </table>
 * Note that in this table Rows, Cols, MaxRows and MaxCols are all positive integers. A(S) is defined to the largest
 * possible power-of-two smaller to EIGEN_MAX_STATIC_ALIGN_BYTES.
 *
 * \see MatrixBase for the majority of the API methods for matrices, \ref TopicClassHierarchy,
 * \ref TopicStorageOrders
 */

template <typename Scalar_, int Rows_, int Cols_, int Options_, int MaxRows_, int MaxCols_>
class Matrix : public PlainObjectBase<Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>> {
 public:
  /** \brief Base class typedef.
   * \sa PlainObjectBase
   */
  typedef PlainObjectBase<Matrix> Base;

  enum { Options = Options_ };

  EIGEN_DENSE_PUBLIC_INTERFACE(Matrix)

  typedef typename Base::PlainObject PlainObject;

  using Base::base;
  using Base::coeffRef;

  /**
   * \brief Assigns matrices to each other.
   *
   * \note This is a special case of the templated operator=. Its purpose is
   * to prevent a default operator= from hiding the templated operator=.
   *
   * \callgraph
   */
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix& operator=(const Matrix& other) { return Base::_set(other); }

  /** \internal
   * \brief Copies the value of the expression \a other into \c *this with automatic resizing.
   *
   * *this might be resized to match the dimensions of \a other. If *this was a null matrix (not already initialized),
   * it will be initialized.
   *
   * Note that copying a row-vector into a vector (and conversely) is allowed.
   * The resizing, if any, is then done in the appropriate way so that row-vectors
   * remain row-vectors and vectors remain vectors.
   */
  template <typename OtherDerived>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix& operator=(const DenseBase<OtherDerived>& other) {
    return Base::_set(other);
  }

  /* Here, doxygen failed to copy the brief information when using \copydoc */

  /**
   * \brief Copies the generic expression \a other into *this.
   * \copydetails DenseBase::operator=(const EigenBase<OtherDerived> &other)
   */
  template <typename OtherDerived>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix& operator=(const EigenBase<OtherDerived>& other) {
    return Base::operator=(other);
  }

  template <typename OtherDerived>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix& operator=(const ReturnByValue<OtherDerived>& func) {
    return Base::operator=(func);
  }

  /** \brief Default constructor.
   *
   * For fixed-size matrices, does nothing.
   *
   * For dynamic-size matrices, creates an empty matrix of size 0. Does not allocate any array. Such a matrix
   * is called a null matrix. This constructor is the unique way to create null matrices: resizing
   * a matrix to 0 is not supported.
   *
   * \sa resize(Index,Index)
   */
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix()
      : Base(){EIGEN_INITIALIZE_COEFFS_IF_THAT_OPTION_IS_ENABLED}

        // FIXME is it still needed
        EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE explicit Matrix(internal::constructor_without_unaligned_array_assert)
      : Base(internal::constructor_without_unaligned_array_assert()){EIGEN_INITIALIZE_COEFFS_IF_THAT_OPTION_IS_ENABLED}

        EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(Matrix && other)
            EIGEN_NOEXCEPT_IF(std::is_nothrow_move_constructible<Scalar>::value)
      : Base(std::move(other)) {}
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix& operator=(Matrix&& other)
      EIGEN_NOEXCEPT_IF(std::is_nothrow_move_assignable<Scalar>::value) {
    Base::operator=(std::move(other));
    return *this;
  }

  /** \copydoc PlainObjectBase(const Scalar&, const Scalar&, const Scalar&,  const Scalar&, const ArgTypes&... args)
   *
   * Example: \include Matrix_variadic_ctor_cxx11.cpp
   * Output: \verbinclude Matrix_variadic_ctor_cxx11.out
   *
   * \sa Matrix(const std::initializer_list<std::initializer_list<Scalar>>&)
   */
  template <typename... ArgTypes>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const Scalar& a0, const Scalar& a1, const Scalar& a2, const Scalar& a3,
                                               const ArgTypes&... args)
      : Base(a0, a1, a2, a3, args...) {}

  /** \brief Constructs a Matrix and initializes it from the coefficients given as initializer-lists grouped by row.
   * \cpp11
   *
   * In the general case, the constructor takes a list of rows, each row being represented as a list of coefficients:
   *
   * Example: \include Matrix_initializer_list_23_cxx11.cpp
   * Output: \verbinclude Matrix_initializer_list_23_cxx11.out
   *
   * Each of the inner initializer lists must contain the exact same number of elements, otherwise an assertion is
   * triggered.
   *
   * In the case of a compile-time column vector, implicit transposition from a single row is allowed.
   * Therefore <code>VectorXd{{1,2,3,4,5}}</code> is legal and the more verbose syntax
   * <code>RowVectorXd{{1},{2},{3},{4},{5}}</code> can be avoided:
   *
   * Example: \include Matrix_initializer_list_vector_cxx11.cpp
   * Output: \verbinclude Matrix_initializer_list_vector_cxx11.out
   *
   * In the case of fixed-sized matrices, the initializer list sizes must exactly match the matrix sizes,
   * and implicit transposition is allowed for compile-time vectors only.
   *
   * \sa Matrix(const Scalar& a0, const Scalar& a1, const Scalar& a2,  const Scalar& a3, const ArgTypes&... args)
   */
  EIGEN_DEVICE_FUNC explicit EIGEN_STRONG_INLINE Matrix(
      const std::initializer_list<std::initializer_list<Scalar>>& list)
      : Base(list) {}

#ifndef EIGEN_PARSED_BY_DOXYGEN

  // This constructor is for both 1x1 matrices and dynamic vectors
  template <typename Scalar>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE explicit Matrix(const Scalar& x) {
    Base::template _init1<Scalar>(x);
  }

  template <typename T0, typename T1>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const T0& x, const T1& y) {
    Base::template _init2<T0, T1>(x, y);
  }

#else
  /** \brief Constructs a fixed-sized matrix initialized with coefficients starting at \a data */
  EIGEN_DEVICE_FUNC explicit Matrix(const Scalar* data);

  /** \brief Constructs a vector or row-vector with given dimension. \only_for_vectors
   *
   * This is useful for dynamic-size vectors. For fixed-size vectors,
   * it is redundant to pass these parameters, so one should use the default constructor
   * Matrix() instead.
   *
   * \warning This constructor is disabled for fixed-size \c 1x1 matrices. For instance,
   * calling Matrix<double,1,1>(1) will call the initialization constructor: Matrix(const Scalar&).
   * For fixed-size \c 1x1 matrices it is therefore recommended to use the default
   * constructor Matrix() instead, especially when using one of the non standard
   * \c EIGEN_INITIALIZE_MATRICES_BY_{ZERO,\c NAN} macros (see \ref TopicPreprocessorDirectives).
   */
  EIGEN_STRONG_INLINE explicit Matrix(Index dim);
  /** \brief Constructs an initialized 1x1 matrix with the given coefficient
   * \sa Matrix(const Scalar&, const Scalar&, const Scalar&,  const Scalar&, const ArgTypes&...) */
  Matrix(const Scalar& x);
  /** \brief Constructs an uninitialized matrix with \a rows rows and \a cols columns.
   *
   * This is useful for dynamic-size matrices. For fixed-size matrices,
   * it is redundant to pass these parameters, so one should use the default constructor
   * Matrix() instead.
   *
   * \warning This constructor is disabled for fixed-size \c 1x2 and \c 2x1 vectors. For instance,
   * calling Matrix2f(2,1) will call the initialization constructor: Matrix(const Scalar& x, const Scalar& y).
   * For fixed-size \c 1x2 or \c 2x1 vectors it is therefore recommended to use the default
   * constructor Matrix() instead, especially when using one of the non standard
   * \c EIGEN_INITIALIZE_MATRICES_BY_{ZERO,\c NAN} macros (see \ref TopicPreprocessorDirectives).
   */
  EIGEN_DEVICE_FUNC Matrix(Index rows, Index cols);

  /** \brief Constructs an initialized 2D vector with given coefficients
   * \sa Matrix(const Scalar&, const Scalar&, const Scalar&,  const Scalar&, const ArgTypes&...) */
  Matrix(const Scalar& x, const Scalar& y);
#endif  // end EIGEN_PARSED_BY_DOXYGEN

  /** \brief Constructs an initialized 3D vector with given coefficients
   * \sa Matrix(const Scalar&, const Scalar&, const Scalar&,  const Scalar&, const ArgTypes&...)
   */
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const Scalar& x, const Scalar& y, const Scalar& z) {
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 3)
    m_storage.data()[0] = x;
    m_storage.data()[1] = y;
    m_storage.data()[2] = z;
  }
  /** \brief Constructs an initialized 4D vector with given coefficients
   * \sa Matrix(const Scalar&, const Scalar&, const Scalar&,  const Scalar&, const ArgTypes&...)
   */
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w) {
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 4)
    m_storage.data()[0] = x;
    m_storage.data()[1] = y;
    m_storage.data()[2] = z;
    m_storage.data()[3] = w;
  }

  /** \brief Copy constructor */
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const Matrix& other) : Base(other) {}

  /** \brief Copy constructor for generic expressions.
   * \sa MatrixBase::operator=(const EigenBase<OtherDerived>&)
   */
  template <typename OtherDerived>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const EigenBase<OtherDerived>& other) : Base(other.derived()) {}

  EIGEN_DEVICE_FUNC EIGEN_CONSTEXPR inline Index innerStride() const EIGEN_NOEXCEPT { return 1; }
  EIGEN_DEVICE_FUNC EIGEN_CONSTEXPR inline Index outerStride() const EIGEN_NOEXCEPT { return this->innerSize(); }

  /////////// Geometry module ///////////

  template <typename OtherDerived>
  EIGEN_DEVICE_FUNC explicit Matrix(const RotationBase<OtherDerived, ColsAtCompileTime>& r);
  template <typename OtherDerived>
  EIGEN_DEVICE_FUNC Matrix& operator=(const RotationBase<OtherDerived, ColsAtCompileTime>& r);

// allow to extend Matrix outside Eigen
#ifdef EIGEN_MATRIX_PLUGIN
#include EIGEN_MATRIX_PLUGIN
#endif

  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // Maybe to PlainObject
  //------------------------------------------------------------

  using SelfMatrix = Matrix<Scalar_, Rows_, Cols_, Options_, MaxRows_, MaxCols_>;

  // NOT ALLOWED FOR MATRIX, ONLY VECTOR
  //  using typename Base::iterator;
//  using typename Base::const_iterator;
//  using Base::begin;
//  using Base::cbegin;
//  using Base::end;
//  using Base::cend;

  template <class Scalar>
  using eigen_vnl_vector = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
  using eigen_vnl_vector_same = eigen_vnl_vector<Scalar>;

  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix(const Eigen::Index r, const Eigen::Index c, Scalar* const input)
      EIGEN_NOEXCEPT_IF(std::is_nothrow_move_constructible<Scalar>::value)
      : Matrix(r, c) {
    std::copy(input, input + r * c, this->begin());
  }

  // TODO Probably should remove this from API
  template <typename IntegerType>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE explicit Matrix(const Scalar* input, const IntegerType r)
      EIGEN_NOEXCEPT_IF(std::is_nothrow_move_constructible<Scalar>::value)
      : Matrix(r, 1) {
    std::copy(input, input + r, this->begin());
  }

  template <typename IntegerType>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE explicit Matrix(Scalar* input, const IntegerType r)
      EIGEN_NOEXCEPT_IF(std::is_nothrow_move_constructible<Scalar>::value)
      : Matrix(r, 1) {
    std::copy(input, input + r, this->begin());
  }

  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix& operator=(const Scalar value) {
    *this = Matrix::Constant(this->rows(), this->cols(), value);
    return *this;
  }

//// TODO: Remove this transpose override performance problem needed for VNL api compliance
//  Matrix transpose() {
//    return this->Base::Base::transpose();
//  }

  // Read a vnl_vector from an ascii std::istream.
  // If the vector has nonzero size on input, read that many values.
  // Otherwise, read to EOF.
  EIGEN_STRONG_INLINE bool read_ascii(std::istream& s) {
    bool size_known = (this->size() != 0);
    if (size_known) {
      for (size_t i = 0; i < this->size(); ++i) {
        if (!(s >> this->operator()(i))) {
          return false;
        }
      }
      return true;
    }

    // Just read until EOF
    std::vector<Scalar> allvals;
    constexpr size_t initial_reserve_size_for_vector = 128; // An initial guess for how many values to read.
    allvals.reserve(initial_reserve_size_for_vector);
    Scalar value;
    while (s >> value) {
      allvals.push_back(value);
    }
    this->set_size(allvals.size());
    for(int idx =0 ; idx < allvals.size(); ++idx) {
      this->data()[idx] = allvals[idx];
    }
    return true;
  }

  EIGEN_STRONG_INLINE SelfMatrix& read(std::istream& s) {
    SelfMatrix V;
    V.read_ascii(s);
    return V;
  }

  //: Normalise by dividing through by the magnitude
  Matrix& normalize() {
    /* vnl_c_vector<Scalar>::normalize(begin(), size()); */
    Base::Base::normalize();
    return *this;
  }

  inline Matrix operator+(Matrix const& v) const { return this->operator+(v); }

  inline Matrix operator+(Scalar v) const { return this->array() + v; }

  inline Matrix operator-(Scalar v) const { return this->array() - v; }

  inline Matrix operator/(Scalar v) const {
    const Scalar inv = static_cast<Scalar>(1.0) / v;
    return this->Base::operator*(inv);
  }

  using Base::Base::operator+=;
  using Base::Base::operator-=;

  Matrix& operator+=(Scalar value) {
    this->array() += value;
    return *this;
  }

  //: Subtract scalar value from all elements
  inline Matrix& operator-=(Scalar value) {
    this->array() -= value;
    return *this;
  }

  //: Unary plus operator
  // Return new vector = (*this)
  inline Matrix operator+() const { return *this; }

  //inline Matrix operator-() const { return this->operator-(); }

  inline Matrix operator-(Matrix const& v) const { return this->Base::operator-(v); }

  //    vnl_vector<Scalar>
  //    operator*(vnl_matrix<Scalar> const & M) const
  //    {
  //      return this->transpose().operator*(M).transpose().matrix();
  //    }

  Matrix& flip() {
    const size_t last_element{static_cast<size_t>(this->size() - 1)};
    const size_t half_index{static_cast<size_t>(this->size() / 2)};
    for (size_t i = 0; i < half_index; ++i) {
      const Scalar tmp = this->operator()(i);
      const size_t other_index = last_element - i;
      this->operator()(i) = this->operator()(other_index);
      this->operator()(other_index) = tmp;
    }
    return *this;
  }

  Matrix& flip(const size_t& b, const size_t& e) {
#if VNL_CONFIG_CHECK_BOUNDS && (!defined NDEBUG)
    assert(!(b > this->size() || e > this->size() || b > e));
#endif
    for (size_t i = b; i < (e - b) / 2 + b; ++i) {
      const Scalar tmp = this->operator()(i);
      const size_t other_index = e - 1 - (i - b);
      this->operator()(i) = this->operator()(other_index);
      this->operator()(other_index) = tmp;
    }
    return *this;
  }

  inline Matrix roll(const int& shift) const {
    Matrix v(this->size());
    const size_t wrapped_shift = shift % this->size();
    if (0 == wrapped_shift) return v.copy_in(this->data_block());
    for (size_t i = 0; i < this->size(); ++i) {
      v((i + wrapped_shift) % this->size()) = this->data()[i];
    }
    return v;
  }

  inline Matrix& roll_inplace(const int& shift) {
    assert(0 == 1 && "This function not supported from for Eigen based VNL");
    return *this;
  }

  void swap(Matrix& that) noexcept {
    // Not efficient
    Matrix temp = *this;
    *this = that;
    that = temp;
  }


  void clear() { this->resize(0,0);};



  //: Make a vector by applying a function across rows.

  //: Return the matrix made by applying "f" to each element.
  Matrix apply(Scalar (*f)(const Scalar &)) const
  {
    Matrix ret(this->rows(),this->cols());
    vnl_c_vector<Scalar>::apply(this->data(), this->rows() * this->cols(), f, ret.data_block());
    return ret;
  }

  //: Return the matrix made by applying "f" to each element.
  Matrix apply(Scalar (*f)(Scalar)) const
  {
    Matrix ret(this->rows(),this->cols());
    vnl_c_vector<Scalar>::apply(this->data(), this->rows() * this->cols(), f, ret.data_block());
    return ret;
  }

  eigen_vnl_vector_same
  apply_rowwise(Scalar (*f)(const eigen_vnl_vector_same &)) const
  {
    eigen_vnl_vector_same v(this->rows());
    for (unsigned int i = 0; i < this->rows(); ++i)
      v.put(i,f(this->get_row(i)));
    return v;
  }

  //: Make a vector by applying a function across columns.
  eigen_vnl_vector_same
  apply_columnwise(Scalar (*f)(const eigen_vnl_vector_same &)) const
  {
    eigen_vnl_vector_same v(this->cols());
    for (unsigned int i = 0; i < this->cols(); ++i)
      v.put(i,f(this->get_column(i)));
    return v;
  }

  //: Returns a subvector specified by the start index and length. O(n).

  Matrix extract(size_t len, size_t start) const { return this->segment(start, len); }


  //: Replaces the submatrix of THIS matrix, starting at top left corner, by the elements of matrix m. O(m*n).
  // This is the reverse of extract().
  Matrix & update (const Matrix & m, Eigen::Index top)
  {
    Eigen::Index bottom = top + m.rows();

    for (Eigen::Index i = top; i < bottom; i++)

        this->operator()(i) = m(i-top);
    return *this;
  }

  Matrix & update (const Matrix & m, Eigen::Index top, Eigen::Index left)
  {
    Eigen::Index bottom = top + m.rows();
    Eigen::Index right = left + m.cols();
#ifndef NDEBUG
    if (this->rows() < bottom || this->cols() < right)
      vnl_error_matrix_dimension ("update",
                                 bottom, right, m.rows(), m.cols());
#endif
    for (Eigen::Index i = top; i < bottom; i++)
      for (Eigen::Index j = left; j < right; j++)
        this->operator()(i,j) = m(i-top,j-left);
    return *this;
  }

  void extract( Matrix& submatrix,
               Eigen::Index top, Eigen::Index left) const {
    Eigen::Index rowz = submatrix.rows();
    Eigen::Index colz = submatrix.cols();
#ifndef NDEBUG
    Eigen::Index bottom = top + rowz;
    Eigen::Index right = left + colz;
    if ((this->rows() < bottom) || (this->cols() < right))
      vnl_error_matrix_dimension ("extract",
                                 this->rows(), this->cols(), bottom, right);
#endif
    for (Eigen::Index i = 0; i < rowz; i++)      // actual copy of all elements
      for (Eigen::Index j = 0; j < colz; j++)    // in submatrix
        submatrix(i,j) = this->operator()(top+i , left+j);
  }

  //: Returns a copy of submatrix of THIS matrix, specified by the top-left corner and size in rows, cols. O(m*n).
  // Use update() to copy new values of this submatrix back into THIS matrix.
  Matrix extract (Eigen::Index rowz, Eigen::Index colz,
                 Eigen::Index top, Eigen::Index left) const {
    Matrix result(rowz, colz);
    this->extract(result, top, left );
    return result;
  }
#ifdef NOT_USED_BY_ITK
  //: Returns a copy of n rows, starting from "row"
  inline Matrix get_n_rows(unsigned row, unsigned n) const {
    Matrix temp = this->block(row, 0, row + n, this->cols());
    return temp;
  }

  //: Returns a copy of n columns, starting from "column".
  template <class Scalar>
  inline Matrix get_n_columns(unsigned column, unsigned n) const {
#ifndef NDEBUG
    if (column + n > this->cols()) vnl_error_matrix_col_index("get_n_columns", column);
#endif

    Matrix result(this->rows(), n);
    for (unsigned int c = 0; c < n; ++c) {
      for (unsigned int r = 0; r < this->rows(); ++r) {
        result(r, c) = this->operator()(r, column + c);
      }
    }
    return result;
  }
#endif

  //: Create a vector out of row[row_index].
  eigen_vnl_vector_same get_row(unsigned row_index) const {
#ifdef ERROR_CHECKING
    if (row_index >= this->rows()) vnl_error_matrix_row_index("get_row", row_index);
#endif

    eigen_vnl_vector_same v(this->cols());
    for (unsigned int j = 0; j < this->cols(); j++)  // For each element in row
      v[j] = this->operator()(row_index, j);
    return v;
  }

  //: Create a vector out of column[column_index].
  eigen_vnl_vector_same get_column(unsigned column_index) const {
#ifdef ERROR_CHECKING
    if (column_index >= this->cols()) vnl_error_matrix_col_index("get_column", column_index);
#endif

    eigen_vnl_vector_same v(this->rows());
    for (unsigned int j = 0; j < this->rows(); j++)  // For each element in row
      v[j] = this->operator()(j, column_index);
    return v;
  }

  //: Create a vector out of row[row_index].
#ifdef NOT_USED_BY_ITK
  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> get_rows(eigen_vnl_vector<int> i) const {
    Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> m(i.size(), this->cols());
    for (unsigned int j = 0; j < i.size(); ++j) {
      m.set_row(j, this->get_row(i.get(j)));
    }
    return m;
  }

  //: Create a vector out of column[column_index].
  Matrix get_columns(eigen_vnl_vector<unsigned int> i) const {
    Matrix m(this->rows(), i.size());
    for (unsigned int j = 0; j < i.size(); ++j) m.set_column(j, this->get_column(i.get(j)));
    return m;
  }
#endif

  //: Return a vector with the content of the (main) diagonal
  eigen_vnl_vector_same get_diagonal() const {
    eigen_vnl_vector_same v(this->rows() < this->cols() ? this->rows() : this->cols());
    for (unsigned int j = 0; j < this->rows() && j < this->cols(); ++j) v[j] = this->operator()(j, j);
    return v;
  }



  //--------------------------------------------------------------------------------

  //: Set row[row_index] to data at given address. No bounds check.
  inline Matrix& set_row(unsigned row_index, const Scalar* v) {
    for (unsigned int j = 0; j < this->cols(); j++)  // For each element in row
      this->operator()(row_index, j) = v[j];
    return *this;
  }

  //: Set row[row_index] to given vector.
  inline Matrix& set_row(unsigned row_index, const eigen_vnl_vector_same& v) {
#ifndef NDEBUG
    if (v.size() != this->cols()) vnl_error_vector_dimension("vnl_matrix::set_row", v.size(), this->cols());
#endif
    set_row(row_index, v.data());
    return *this;
  }

  //: Set row[row_index] to given value.
  template <class Scalar>
  inline Matrix& set_row(unsigned row_index, Scalar v) {
    for (unsigned int j = 0; j < this->cols(); j++)  // For each element in row
      this->operator()(row_index, j) = v;
    return *this;
  }

  //--------------------------------------------------------------------------------

  //: Set column[column_index] to data at given address.
  inline Matrix& set_column(unsigned column_index, const Scalar* v) {
    for (unsigned int i = 0; i < this->rows(); i++)  // For each element in row
      this->operator()(i, column_index) = v[i];
    return *this;
  }

  //: Set column[column_index] to given vector.
  inline Matrix& set_column(unsigned column_index, const eigen_vnl_vector_same& v) {
#ifndef NDEBUG
    if (v.size() != this->rows()) vnl_error_vector_dimension("vnl_matrix::set_column", v.size(), this->rows());
#endif
    set_column(column_index, v.data());
    return *this;
  }

  //: Set column[column_index] to given value.
  template <class Scalar>
  inline Matrix& set_column(unsigned column_index, Scalar v) {
    for (unsigned int j = 0; j < this->rows(); j++)  // For each element in row
      this->operator()(j, column_index) = v;
    return *this;
  }

  //: Set columns starting at starting_column to given matrix
  template <class Scalar>
  inline Matrix& set_columns(unsigned starting_column, const eigen_vnl_vector_same& m) {
#ifndef NDEBUG
    if (this->rows() != m.rows() || this->cols() < m.cols() + starting_column)  // Size match?
      vnl_error_matrix_dimension("set_columns", this->rows(), this->cols(), m.rows(), m.cols());
#endif

    for (unsigned int j = 0; j < m.cols(); ++j)
      for (unsigned int i = 0; i < this->rows(); i++)  // For each element in row
        this->operator()(i, starting_column + j) = m(i, j);
    return *this;
  }


  //: Sets all diagonal elements of matrix to specified value. O(n).

  Matrix & fill_diagonal (Scalar const& value)
  {
    for (unsigned int i = 0; i < this->rows() && i < this->cols(); ++i) {
      this->operator()(i, i) = value;
    }
    return *this;
  }

  //: Sets the diagonal elements of this matrix to the specified list of values.

  Matrix & set_diagonal(const eigen_vnl_vector_same & diag)
  {
    assert(diag.size() >= this->rows() ||
           diag.size() >= this->cols());
    // The length of the diagonal of a non-square matrix is the minimum of
    // the matrix's width & height; that explains the "||" in the assert,
    // and the "&&" in the upper bound for the "for".
    for (unsigned int i = 0; i < this->rows() && i < this->cols(); ++i)
      this->operator()(i,i) = diag(i);
    return *this;
  }

  //: Fill this matrix with the given data.
  //  We assume that p points to a contiguous rows*cols array, stored rowwise.

  Matrix & copy_in(Scalar const *p)
  {
    std::copy( p, p + this->rows() * this->cols(), this->data() );
    return *this;
  }
  
  //: Fill the given array with this matrix.
  //  We assume that p points to a contiguous rows*cols array, stored rowwise.
  void copy_out(Scalar *p) const
  {
    std::copy( this->data(), this->data() + this->rows() * this->cols(), p );
  }

  //: Fill this matrix with a matrix having 1s on the main diagonal and 0s elsewhere.
  Matrix & set_identity()
  {
    //TODO:
    this->setZero();
    for(Eigen::Index i = 0; i < this->rows() && i < this->cols(); ++i)
    {
      this->operator()(i,i) = 1;
    }
    return *this;
  }
#ifdef NOT_USED_BY_ITK
  //: Make each row of the matrix have unit norm.
  // All-zero rows are ignored.
  Matrix & normalize_rows()
  {
    typedef typename vnl_numeric_traits<Scalar>::abs_t Abs_t;
    typedef typename vnl_numeric_traits<Scalar>::real_t Real_t;
    typedef typename vnl_numeric_traits<Real_t>::abs_t abs_real_t;
    for (unsigned int i = 0; i < this->rows(); ++i) {  // For each row in the Matrix
      Abs_t norm(0); // double will not do for all types.
      for (unsigned int j = 0; j < this->cols(); ++j)  // For each element in row
        norm += vnl_math::squared_magnitude(this->operator()(i,j));

      if (norm != 0) {
        abs_real_t scale = abs_real_t(1)/(std::sqrt((abs_real_t)norm));
        for (unsigned int j = 0; j < this->cols(); ++j)
          this->operator()(i,j) = Scalar(Real_t(this->operator()(i,j)) * scale);
      }
    }
    return *this;
  }

  //: Make each column of the matrix have unit norm.
  // All-zero columns are ignored.
  template <class Scalar>
  Matrix & normalize_columns()
  {
    typedef typename vnl_numeric_traits<Scalar>::abs_t Abs_t;
    typedef typename vnl_numeric_traits<Scalar>::real_t Real_t;
    typedef typename vnl_numeric_traits<Real_t>::abs_t abs_real_t;
    for (unsigned int j = 0; j < this->cols(); j++) {  // For each column in the Matrix
      Abs_t norm(0); // double will not do for all types.
      for (unsigned int i = 0; i < this->rows(); i++)
        norm += vnl_math::squared_magnitude(this->operator()(i,j));

      if (norm != 0) {
        abs_real_t scale = abs_real_t(1)/(std::sqrt((abs_real_t)norm));
        for (unsigned int i = 0; i < this->rows(); i++)
          this->operator()(i,j) = Scalar(Real_t(this->operator()(i,j)) * scale);
      }
    }
    return *this;
  }

  //: Multiply row[row_index] by value
  template <class Scalar>
  Matrix & scale_row(unsigned row_index, Scalar value)
  {
#ifndef NDEBUG
    if (row_index >= this->rows())
      vnl_error_matrix_row_index("scale_row", row_index);
#endif
    for (unsigned int j = 0; j < this->cols(); j++)    // For each element in row
      this->operator()(row_index,j) *= value;
    return *this;
  }

  //: Multiply column[column_index] by value

  Matrix & scale_column(unsigned column_index, Scalar value)
  {
#ifndef NDEBUG
    if (column_index >= this->cols())
      vnl_error_matrix_col_index("scale_column", column_index);
#endif
    for (unsigned int j = 0; j < this->rows(); j++)    // For each element in column
      this->operator()(j,column_index) *= value;
    return *this;
  }
#endif

  
  bool is_identity() const
  {
    Scalar const zero(0);
    Scalar const one(1);
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j) {
        Scalar xm = this->operator()(i,j);
        if ( !((i == j) ? (xm == one) : (xm == zero)) )
          return false;
      }
    return true;
  }

  //: Return true if maximum absolute deviation of M from identity is <= tol.
  bool is_identity(double tol) const
  {
    Scalar one(1);
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j) {
        Scalar xm = this->operator()(i,j);
        double absdev = (i == j) ? vnl_math::abs(xm - one) : vnl_math::abs(xm);
        if (absdev > tol)
          return false;
      }
    return true;
  }

  bool is_zero() const
  {
    Scalar const zero(0);
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j)
        if ( this->operator()(i, j) != zero )
          return false;

    return true;
  }

  //: Return true if max(abs((*this))) <= tol.
  bool is_zero(double tol) const
  {
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j)
        if (vnl_math::abs(this->operator()(i,j)) > tol)
          return false;

    return true;
  }

  //: Return true if any element of (*this) is nan
  bool has_nans() const
  {
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j)
        if (vnl_math::isnan(this->operator()(i,j)))
          return true;

    return false;
  }

  //: Return false if any element of (*this) is inf or nan
  bool is_finite() const
  {
    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j)
        if (!vnl_math::isfinite(this->operator()(i,j)))
          return false;

    return true;
  }

  //: Abort if any element of M is inf or nan
  void assert_finite_internal() const
  {

    if (is_finite())
      return;

//
//    std::cerr << "\n\n" __FILE__ ": " << __LINE__ << ": matrix has non-finite elements\n";
//
//    if (this->rows() <= 20 && this->cols() <= 20) {
//      std::cerr << __FILE__ ": here it is:\n" << *this;
//    }
//    else {
//      std::cerr << __FILE__ ": it is quite big (" << this->rows() << 'x' << this->cols() << ")\n"
//                << __FILE__ ": in the following picture '-' means finite and '*' means non-finite:\n";
//
//      for (unsigned int i=0; i<this->rows(); ++i) {
//        for (unsigned int j=0; j<this->cols(); ++j)
//          std::cerr << char(vnl_math::isfinite(this->operator()(i, j)) ? '-' : '*');
//        std::cerr << '\n';
//      }
//    }
//    std::cerr << __FILE__ ": calling abort()\n";
    std::abort();
  }

  //: Abort unless M has the given size.
  void assert_size_internal(unsigned rs,unsigned cs) const
  {
    if (this->rows()!=rs || this->cols()!=cs) {
//      std::cerr << __FILE__ ": size is " << this->rows() << 'x' << this->cols()
//                << ". should be " << rs << 'x' << cs << std::endl;
      std::abort();
    }
  }

 private:
  mutable std::unique_ptr<Scalar *> _data_ptr_ptr{nullptr};
 public:
  //  //: Access the 2D array, so that elements can be accessed with array[row][col] directly.
  //  //  2d array, [row][column].
  Scalar const * const *
  data_array() const
  {
    _data_ptr_ptr.reset( new Scalar * [this->rows()] );
    for (int r = 0; r < this->rows(); ++r)
    {
      _data_ptr_ptr.get()[r] = const_cast<Scalar *>(this->data()) + r * this->cols();
    }
    return _data_ptr_ptr.get();
  }
  //
  //  //: Access the 2D array, so that elements can be accessed with array[row][col] directly.
  //  //  2d array, [row][column].
  Scalar **
  data_array()
  {
    _data_ptr_ptr.reset( new Scalar *[this->rows()] );
    for (int r = 0; r < this->rows(); ++r)
    {
      _data_ptr_ptr[r] = this->data() + r * this->cols();
    }
    return _data_ptr_ptr;
  }


  //: Type def for norms.
  using Base::Base::array_one_norm;
  using Base::Base::array_two_norm;
  using Base::Base::array_inf_norm;
  using Base::Base::absolute_value_sum;
  using Base::Base::absolute_value_max;

  //: abort if matrix contains any INFs or NANs.
  // This function does or tests nothing if NDEBUG is defined
  void assert_finite() const
  {
#ifndef NDEBUG
    assert_finite_internal();
#endif
  }

  //: abort if size is not as expected
  // This function does or tests nothing if NDEBUG is defined
#ifndef NDEBUG
  void assert_size(unsigned r, unsigned c) const
  {
    assert_size_internal(r, c);
  }
#else
  void assert_size(unsigned , unsigned ) const
  {
  //NOOP
  }
#endif
  using abs_t = typename Base::Base::abs_t;
  // || M ||  = \max \sum | M   |
  //        1     j    i     ij
  abs_t operator_one_norm() const
  {
    abs_t max = 0;
    for (unsigned int j=0; j<this->num_cols; ++j) {
      abs_t tmp = 0;
      for (unsigned int i=0; i<this->num_rows; ++i)
        tmp += vnl_math::abs(this->data[i][j]);
      if (tmp > max)
        max = tmp;
    }
    return max;
  }

  // || M ||   = \max \sum | M   |
  //        oo     i    j     ij
  abs_t operator_inf_norm() const
  {
    abs_t max = 0;
    for (unsigned int i=0; i<this->rows(); ++i) {
      abs_t tmp = 0;
      const auto numcols = this->cols();
      for (unsigned int j=0; j<numcols; ++j)
        tmp += vnl_math::abs(this->data()[i*numcols+j]);
      if (tmp > max)
        max = tmp;
    }
    return max;
  }

  //: Return Frobenius norm of matrix (sqrt of sum of squares of its elements)
  abs_t
  frobenius_norm() const
  {
    return array_two_norm();
  }

  //: Return Frobenius norm of matrix (sqrt of sum of squares of its elements)
  abs_t
  fro_norm() const
  {
    return frobenius_norm();
  }


  // adjoint/hermitian transpose


 Matrix conjugate_transpose() const
  {
    Matrix result(*this);
    result.transposeInPlace();
    vnl_c_vector<Scalar>::conjugate(result.data(),  // src
                               result.data(),  // dst
                               result.size());  // size of block
    return result;
  }


 protected:
  template <typename Derived, typename OtherDerived, bool IsVector>
  friend struct internal::conservative_resize_like_impl;

  using Base::m_storage;
};

/** \defgroup matrixtypedefs Global matrix typedefs
 *
 * \ingroup Core_Module
 *
 * %Eigen defines several typedef shortcuts for most common matrix and vector types.
 *
 * The general patterns are the following:
 *
 * \c MatrixSizeType where \c Size can be \c 2,\c 3,\c 4 for fixed size square matrices or \c X for dynamic size,
 * and where \c Type can be \c i for integer, \c f for float, \c d for double, \c cf for complex float, \c cd
 * for complex double.
 *
 * For example, \c Matrix3d is a fixed-size 3x3 matrix type of doubles, and \c MatrixXf is a dynamic-size matrix of
 * floats.
 *
 * There are also \c VectorSizeType and \c RowVectorSizeType which are self-explanatory. For example, \c Vector4cf is
 * a fixed-size vector of 4 complex floats.
 *
 * With \cpp11, template alias are also defined for common sizes.
 * They follow the same pattern as above except that the scalar type suffix is replaced by a
 * template parameter, i.e.:
 *   - `MatrixSize<Type>` where `Size` can be \c 2,\c 3,\c 4 for fixed size square matrices or \c X for dynamic size.
 *   - `MatrixXSize<Type>` and `MatrixSizeX<Type>` where `Size` can be \c 2,\c 3,\c 4 for hybrid dynamic/fixed matrices.
 *   - `VectorSize<Type>` and `RowVectorSize<Type>` for column and row vectors.
 *
 * With \cpp11, you can also use fully generic column and row vector types: `Vector<Type,Size>` and
 * `RowVector<Type,Size>`.
 *
 * \sa class Matrix
 */

#define EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, Size, SizeSuffix)    \
  /** \ingroup matrixtypedefs */                                   \
  /** \brief `Size`&times;`Size` matrix of type `Type`. */         \
  typedef Matrix<Type, Size, Size> Matrix##SizeSuffix##TypeSuffix; \
  /** \ingroup matrixtypedefs */                                   \
  /** \brief `Size`&times;`1` vector of type `Type`. */            \
  typedef Matrix<Type, Size, 1> Vector##SizeSuffix##TypeSuffix;    \
  /** \ingroup matrixtypedefs */                                   \
  /** \brief `1`&times;`Size` vector of type `Type`. */            \
  typedef Matrix<Type, 1, Size> RowVector##SizeSuffix##TypeSuffix;

#define EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, Size)          \
  /** \ingroup matrixtypedefs */                                   \
  /** \brief `Size`&times;`Dynamic` matrix of type `Type`. */      \
  typedef Matrix<Type, Size, Dynamic> Matrix##Size##X##TypeSuffix; \
  /** \ingroup matrixtypedefs */                                   \
  /** \brief `Dynamic`&times;`Size` matrix of type `Type`. */      \
  typedef Matrix<Type, Dynamic, Size> Matrix##X##Size##TypeSuffix;

#define EIGEN_MAKE_TYPEDEFS_ALL_SIZES(Type, TypeSuffix) \
  EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, 2, 2)           \
  EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, 3, 3)           \
  EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, 4, 4)           \
  EIGEN_MAKE_TYPEDEFS(Type, TypeSuffix, Dynamic, X)     \
  EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, 2)        \
  EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, 3)        \
  EIGEN_MAKE_FIXED_TYPEDEFS(Type, TypeSuffix, 4)

EIGEN_MAKE_TYPEDEFS_ALL_SIZES(int, i)
EIGEN_MAKE_TYPEDEFS_ALL_SIZES(float, f)
EIGEN_MAKE_TYPEDEFS_ALL_SIZES(double, d)
EIGEN_MAKE_TYPEDEFS_ALL_SIZES(std::complex<float>, cf)
EIGEN_MAKE_TYPEDEFS_ALL_SIZES(std::complex<double>, cd)

#undef EIGEN_MAKE_TYPEDEFS_ALL_SIZES
#undef EIGEN_MAKE_TYPEDEFS
#undef EIGEN_MAKE_FIXED_TYPEDEFS

#define EIGEN_MAKE_TYPEDEFS(Size, SizeSuffix)                    \
  /** \ingroup matrixtypedefs */                                 \
  /** \brief \cpp11 `Size`&times;`Size` matrix of type `Type`.*/ \
  template <typename Type>                                       \
  using Matrix##SizeSuffix = Matrix<Type, Size, Size>;           \
  /** \ingroup matrixtypedefs */                                 \
  /** \brief \cpp11 `Size`&times;`1` vector of type `Type`.*/    \
  template <typename Type>                                       \
  using Vector##SizeSuffix = Matrix<Type, Size, 1>;              \
  /** \ingroup matrixtypedefs */                                 \
  /** \brief \cpp11 `1`&times;`Size` vector of type `Type`.*/    \
  template <typename Type>                                       \
  using RowVector##SizeSuffix = Matrix<Type, 1, Size>;

#define EIGEN_MAKE_FIXED_TYPEDEFS(Size)                              \
  /** \ingroup matrixtypedefs */                                     \
  /** \brief \cpp11 `Size`&times;`Dynamic` matrix of type `Type` */  \
  template <typename Type>                                           \
  using Matrix##Size##X = Matrix<Type, Size, Dynamic>;               \
  /** \ingroup matrixtypedefs */                                     \
  /** \brief \cpp11 `Dynamic`&times;`Size` matrix of type `Type`. */ \
  template <typename Type>                                           \
  using Matrix##X##Size = Matrix<Type, Dynamic, Size>;

EIGEN_MAKE_TYPEDEFS(2, 2)
EIGEN_MAKE_TYPEDEFS(3, 3)
EIGEN_MAKE_TYPEDEFS(4, 4)
EIGEN_MAKE_TYPEDEFS(Dynamic, X)
EIGEN_MAKE_FIXED_TYPEDEFS(2)
EIGEN_MAKE_FIXED_TYPEDEFS(3)
EIGEN_MAKE_FIXED_TYPEDEFS(4)

/** \ingroup matrixtypedefs
 * \brief \cpp11 `Size`&times;`1` vector of type `Type`. */
template <typename Type, int Size>
using Vector = Matrix<Type, Size, 1>;

/** \ingroup matrixtypedefs
 * \brief \cpp11 `1`&times;`Size` vector of type `Type`. */
template <typename Type, int Size>
using RowVector = Matrix<Type, 1, Size>;

#undef EIGEN_MAKE_TYPEDEFS
#undef EIGEN_MAKE_FIXED_TYPEDEFS

}  // end namespace Eigen

#endif  // EIGEN_MATRIX_H
