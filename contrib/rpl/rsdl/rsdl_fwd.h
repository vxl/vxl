// This is rpl/rsdl/rsdl_fwd.h
#ifndef rsdl_fwd_h_
#define rsdl_fwd_h_

#include <vbl/vbl_smart_ptr.h>

class rsdl_point;
class rsdl_kd_tree;
typedef vbl_smart_ptr<rsdl_kd_tree> rsdl_kd_tree_sptr;

template  < class COORD_T, class VALUE_T >
class rsdl_bins_2d;

template< unsigned N, typename CoordType, typename ValueType >
class rsdl_bins;

#endif // rsdl_fwd_h_
