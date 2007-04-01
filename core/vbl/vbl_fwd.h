#ifndef vbl_fwd_h_
#define vbl_fwd_h_

#include <vcl_compiler.h> // for export keyword

template <class T> struct vbl_array_1d;
export
template <class T> class vbl_array_2d;
export
template <class T> class vbl_array_3d;
template <class T> class vbl_sparse_array_1d;
template <class T> class vbl_sparse_array_2d;
template <class T> class vbl_sparse_array_3d;
template <class T> class vbl_big_sparse_array_3d;
class vbl_bit_array_2d;
class vbl_bit_array_3d;

template <class T, int DIM> class vbl_bounding_box;

class vbl_ref_count;
template <class T> class vbl_smart_ptr;
template <class T> class vbl_scoped_ptr;
template <class T> class vbl_shared_pointer;

template <class T1, class T2, class T3> struct vbl_triple;
class vbl_bool_ostream;

#endif // vbl_fwd_h_
