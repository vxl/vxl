// This is core/vbl/vbl_sort.h
#ifndef vbl_sort_h_
#define vbl_sort_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Collection of common predicates for sorting
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   19 Nov 97
//
// \verbatim
// Modifications
// 971119 AWF Initial version.
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

int vbl_sort_double_ascending(double const&, double const&);
int vbl_sort_double_descending(double const&, double const&);

int vbl_sort_int_ascending(int const&, int const&);
int vbl_sort_int_descending(int const&, int const&);


//: Collection of common predicates for sorting
template <class T>
struct vbl_sort_helper
{
  static int ascend(const void* a, const void* b) {
    T const& ta = *((T const*)a);
    T const& tb = *((T const*)b);
    return tb > ta ? -1 : tb == ta ? 0 : 1;
  }
  static int descend(const void* a, const void* b) {
    return - ascend(a,b);
  }
};

#define VBL_SORT_INSTANTIATE(T) \
template struct vbl_sort_helper<T >

#endif // vbl_sort_h_
