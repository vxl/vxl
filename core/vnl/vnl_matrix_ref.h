// This is core/vnl/vnl_matrix_ref.h
#ifndef vnl_matrix_ref_h_
#define vnl_matrix_ref_h_
//:
// \file
// \brief vnl_matrix reference to user-supplied storage.
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   04 Aug 96
//
// \verbatim
//  Modifications
//   Documentation updated by Ian Scott 12 Mar 2000
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//
//-----------------------------------------------------------------------------

#include "vnl_matrix.h"
#include "vnl/vnl_math.h"
#include "vnl/vnl_export.h"

//: vnl_matrix reference to user-supplied storage
//    vnl_matrix_ref is a vnl_matrix for which the data space has been
//    supplied externally.  This is useful for two main tasks:
//    (a) Treating some row-based "C" matrix as a vnl_matrix in order to
//    perform vnl_matrix operations on it.
//
//    This is a dangerous class.  I believe that I've covered all the bases, but
//    it's really only intended for interfacing with the Fortran routines.
//
//    The big warning is that returning a vnl_matrix_ref pointer will free non-heap
//    memory if deleted through a vnl_matrix pointer.  This should be
//    very difficult though, as vnl_matrix_ref objects may not be constructed using
//    operator new, and are therefore unlikely to be the unwitting subject
//    of an operator delete.
template <class T>
using eigen_vnl_matrix_ref = Eigen::Map<eigen_vnl_matrix<T>>;

template <class T>
class VNL_EXPORT vnl_matrix_ref : public eigen_vnl_matrix_ref<T>
{
public:
  typedef eigen_vnl_matrix_ref<T> Base;
protected:
  using Superclass=eigen_vnl_matrix_ref<T> ;
public:

  // Constructors/Destructors--------------------------------------------------
  vnl_matrix_ref(unsigned int row, unsigned int col, const T *datablck)
    : eigen_vnl_matrix_ref<T>( const_cast<T *>(datablck), row, col)
  { }

  vnl_matrix_ref(const vnl_matrix_ref<T> & other)
  : eigen_vnl_matrix_ref<T>(const_cast<T *>(other.data()), other.rows(), other.cols())
  { }

  ~vnl_matrix_ref() = default;

  T * data_block() { return this->Superclass::data(); }
  // T const * const data_block() const { return this->Superclass::data(); }


  //: Copy and move constructor from vnl_matrix_ref<T> is disallowed by default
  // due to other constructor definitions.
  //: assignment and move-assignment is disallowed
  //  because it does not define external memory to be managed.
  vnl_matrix_ref & operator=( vnl_matrix_ref<T> const& ) = delete;
  vnl_matrix_ref & operator=( vnl_matrix_ref<T> && ) = delete;

  //: Return the number of columns.
  // A synonym for cols().
  inline unsigned int
  columns() const
  {
    return this->cols();
  }

  //: Reference to self to make non-const temporaries.
  // This is intended for passing vnl_matrix_fixed objects to
  // functions that expect non-const vnl_matrix references:
  // \code
  //   void mutator( vnl_matrix<double>& );
  //   ...
  //   vnl_matrix_fixed<double,5,3> my_m;
  //   mutator( m );        // Both these fail because the temporary vnl_matrix_ref
  //   mutator( m.as_ref() );  // cannot be bound to the non-const reference
  //   mutator( m.as_ref().non_const() ); // works
  // \endcode
  // \attention Use this only to pass the reference to a
  // function. Otherwise, the underlying object will be destructed and
  // you'll be left with undefined behaviour.
  vnl_matrix_ref& non_const() { return *this; }

  //: Extract a sub-matrix of size r x c, starting at (top,left)
  //  Thus it contains elements  [top,top+r-1][left,left+c-1]
  vnl_matrix<T>
  extract(unsigned r, unsigned c, unsigned top = 0, unsigned left = 0) const
  {
    // Sub-optimal, equivalent interface
    vnl_matrix<T> tmp{*this};
    return tmp.extract(r, c, top, left);
  }

  //: Extract a sub-matrix starting at (top,left)
  //
  //  The output is stored in \a sub_matrix, and it should have the
  //  required size on entry.  Thus the result will contain elements
  //  [top,top+sub_matrix.rows()-1][left,left+sub_matrix.cols()-1]
  void
  extract(vnl_matrix<T> & sub_matrix, unsigned top = 0, unsigned left = 0) const
  {
    // Sub-optimal, equivalent interface
    vnl_matrix<T> tmp{*this};
    return tmp.extract(sub_matrix, top, left);
  }

  bool is_equal(vnl_matrix_ref<T> const& rhs, double tol) const
  {
    if (this == &rhs)                                      // same object => equal.
      return true;

    if (this->rows() != rhs.rows() || this->cols() != rhs.cols())
      return false;                                        // different sizes => not equal.

    for (unsigned int i = 0; i < this->rows(); ++i)
      for (unsigned int j = 0; j < this->columns(); ++j)
        if (vnl_math::abs(this->operator()(i,j) - rhs(i,j)) > tol)
          return false;                                    // difference greater than tol

    return true;
  }

 private:
  //: Resizing is disallowed
  bool resize (unsigned int, unsigned int) { return false; }
  //: Resizing is disallowed
  bool make_size (unsigned int, unsigned int) { return false; }
  //: Resizing is disallowed
  bool set_size (unsigned int, unsigned int) { return false; }

};

#endif // vnl_matrix_ref_h_
