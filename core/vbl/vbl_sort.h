// This is core/vbl/vbl_sort.h
#ifndef vbl_sort_h_
#define vbl_sort_h_
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

int
vbl_sort_double_ascending(const double &, const double &);
int
vbl_sort_double_descending(const double &, const double &);

int
vbl_sort_int_ascending(const int &, const int &);
int
vbl_sort_int_descending(const int &, const int &);


//: Collection of common predicates for sorting
template <class T>
struct vbl_sort_helper
{
  static int
  ascend(const void * a, const void * b)
  {
    const T & ta = *((const T *)a);
    const T & tb = *((const T *)b);
    return tb > ta ? -1 : tb == ta ? 0 : 1;
  }
  static int
  descend(const void * a, const void * b)
  {
    return -ascend(a, b);
  }
};

#define VBL_SORT_INSTANTIATE(T) template struct vbl_sort_helper<T>

#endif // vbl_sort_h_
