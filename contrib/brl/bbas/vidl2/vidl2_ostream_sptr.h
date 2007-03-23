// This is brl/bbas/vidl2/vidl2_ostream_sptr.h
#ifndef vidl2_ostream_sptr_h_
#define vidl2_ostream_sptr_h_
//:
// \file
// \brief A smart pointer to an output stream
//
// \author Matt Leotta
// \date 23 Mar 2007

#include <vil/vil_smart_ptr.h>

class vidl2_ostream;

//: A smart pointer to an output stream
typedef vil_smart_ptr<vidl2_ostream> vidl2_ostream_sptr;

#endif // vidl2_ostream_sptr_h_
