#ifndef vnl_identity_3x3_h_
#define vnl_identity_3x3_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vnl/vnl_identity_3x3.h

//: \file
//  \author Andrew W. Fitzgibbon, Oxford RRG
//  \date   30 Nov 96
//
// \verbatim
// Modifications:
// LSB (Manchester) 23/3/01 tidied documentation
// \endverbatim
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>

struct vnl_identity_3x3 : public vnl_double_3x3 {
  vnl_identity_3x3() { set_identity(); }
};

#endif // vnl_identity_3x3_h_
