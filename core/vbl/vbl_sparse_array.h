#ifndef vbl_sparse_array_h_
#define vbl_sparse_array_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_sparse_array.h

//:
// \file
// \brief Sparse array allowing space efficient access of the form s[3000]=2.
// \deprecated
// \author  Andrew W. Fitzgibbon, Oxford RRG
// \date    02 Oct 96
//
// \verbatim
// Modifications:
//     230497 AWF Converted from vbl_sparse_array3D
//     10 April 2001 Ian Scott (Manchester, ISBE) Deprecated in favour of vbl_sparse_array_1d
// \endverbatim
//---------------------------------------------------------------------------

#include <vcl_deprecated_header.h>

#include <vbl/vbl_sparse_array_1d.h>
#define vbl_sparse_array vbl_sparse_array_1d

#endif // vbl_sparse_array_h_
