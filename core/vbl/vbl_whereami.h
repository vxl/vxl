#ifndef vbl_whereami_h_
#define vbl_whereami_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_whereami.h

//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_iostream.h>

#define vbl_whereami (vcl_cerr << __FILE__ ", line " << __LINE__ << " : ")

#endif // vbl_whereami_h_
