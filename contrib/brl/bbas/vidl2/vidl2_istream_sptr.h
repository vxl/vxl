// This is brl/bbas/vidl2/vidl2_istream_sptr.h
#ifndef vidl2_istream_sptr_h_
#define vidl2_istream_sptr_h_
//:
// \file
// \brief A smart pointer to an input stream
//
// \author Matt Leotta
// \date 23 Mar 2007

#include <vil/vil_smart_ptr.h>

class vidl2_istream;

//: A smart pointer to an input stream
typedef vil_smart_ptr<vidl2_istream> vidl2_istream_sptr;

#endif // vidl2_istream_sptr_h_
