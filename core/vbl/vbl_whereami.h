#ifndef vbl_whereami_h_
#define vbl_whereami_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .SECTION Author
//  fsm@robots.ox.ac.uk
//

#include <vcl/vcl_iostream.h>

#define vbl_whereami (cerr << __FILE__ ", line " << __LINE__ << " : ")

#endif // vbl_whereami_h_
