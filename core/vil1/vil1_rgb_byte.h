// This is vxl/vil/vil_rgb_byte.h
#ifndef vil_rgb_byte_h_
#define vil_rgb_byte_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author   awf@robots.ox.ac.uk
// \date 17 Feb 00
//
//\verbatim
//  Modifications
//     000217 AWF Initial version.
//     001206 FSM make this identical to vil_rgb<vil_byte> to avoid confusion
//\endverbatim

#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>

typedef vil_rgb<vil_byte> vil_rgb_byte;

#endif // vil_rgb_byte_h_
