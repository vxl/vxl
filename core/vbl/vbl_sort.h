#ifndef vbl_sort_h_
#define vbl_sort_h_
#ifdef __GNUC__
#pragma interface
#endif

// .NAME    vbl_sort - Collection of common predicates for sorting
// .LIBRARY vbl
// .HEADER  vxl package
// .INCLUDE vbl/vbl_sort.h
// .FILE    vbl_sort.txx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 19 Nov 97
//
// .SECTION Modifications
//    971119 AWF Initial version.
//
//-----------------------------------------------------------------------------

int vbl_sort_double_ascending(double const&, double const&);
int vbl_sort_double_descending(double const&, double const&);

int vbl_sort_int_ascending(int const&, int const&);
int vbl_sort_int_descending(int const&, int const&);

template <class T>
struct vbl_sort_helper {
  static int ascend(const void* a, const void* b) {
    T const& ta = *((T const*)a);
    T const& tb = *((T const*)b);
    if (tb > ta)
      return -1;
    if (tb == ta)
      return 0;
    return 1;
  }
  static int descend(const void* a, const void* b) {
    return - ascend(a,b);
  }
};

#define VBL_SORT_INSTANTIATE(T)\
template struct vbl_sort_helper<T >

#endif // vbl_sort_h_
