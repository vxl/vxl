#ifndef vbl_sort_h_
#define vbl_sort_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME        vbl_sort - Collection of common predicates for sorting.
// .LIBRARY     vbl
// .HEADER	Basics Package
// .INCLUDE     vbl/vbl_sort.h
// .FILE        vbl/vbl_sort.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 19 Nov 97
//
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <vcl/vcl_vector.h>

//: Collection of common predicates for "C" library and COOL sort routines
class vbl_sort {
public:
  // Constructors/Destructors--------------------------------------------------

  static int double_ascending(double const&, double const&);
  static int double_descending(double const&, double const&);

  static int int_ascending(int const&, int const&);
  static int int_descending(int const&, int const&);

protected:
  // Data Members--------------------------------------------------------------
  
  // Helpers-------------------------------------------------------------------
};

template <class T> 
struct vbl_sort_helper {
  static int ascend(const void* a, const void* b) {
    const T& ta = *((const T*)a);
    const T& tb = *((const T*)b);
    if (tb > ta)
      return -1;
    if (tb == ta)
      return 0;
    return 1;
  }
  static int descend(const void* a, const void* b) {
    return 1 - ascend(a,b);
  }
};

// -- Sort a C array into ascending order, using the standard comparison
// operations for T, namely operator> and operator==.
template <class T>
void vbl_qsort_ascending(T* base, int n)
{
  qsort(base, n, sizeof base[0], vbl_sort_helper<T>::ascend);
}

// -- Sort a C array into descending order, using the standard comparison
// operations for T, namely "operator>" and "operator==".
template <class T>
void vbl_qsort_descending(T* base, int n)
{
  qsort(base, n, sizeof base[0], vbl_sort_helper<T>::ascend);
}

// -- Sort am STL vector into ascending order, using the standard comparison
// operations for T, namely operator> and operator==.  I know STL has a sort,
// but this is easier, and faster in the 20th century.
template <class T>
void vbl_qsort_ascending(vcl_vector<T>& v)
{
  qsort(v.begin(), v.size(), sizeof v[0], vbl_sort_helper<T>::ascend);
}

// -- Sort an STL vector into descending order, using the standard comparison
// operations for T, namely "operator>" and "operator==".
template <class T>
void vbl_qsort_descending(vcl_vector<T>& v)
{
  qsort(v.begin(), v.size(), sizeof v[0], vbl_sort_helper<T>::descend);
}

#define VBL_SORT_INSTANTIATE(T) "error, see .txx file"

#endif

