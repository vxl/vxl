#ifndef vnl_identity_3x3_h_
#define vnl_identity_3x3_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_identity_3x3 - Undocumented class FIXME
// .LIBRARY	vnl
// .HEADER	vxl package
// .INCLUDE	vnl/vnl_identity_3x3.h
// .FILE        vnl/vnl_identity_3x3.cxx
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 30 Nov 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>

struct vnl_identity_3x3 : public vnl_double_3x3 {
  vnl_identity_3x3() { set_identity(); }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_identity_3x3.
