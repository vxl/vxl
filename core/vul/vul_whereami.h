#ifndef vul_whereami_h_
#define vul_whereami_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vul/vul_whereami.h

//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_iostream.h>

#define vul_whereami (vcl_cerr << __FILE__ ", line " << __LINE__ << " : ")

#endif // vul_whereami_h_
