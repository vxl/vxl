// This is core/vidl/vidl_ostream_sptr.h
#ifndef vidl_ostream_sptr_h_
#define vidl_ostream_sptr_h_
//:
// \file
// \brief A smart pointer to an output stream
//
// \author Matt Leotta
// \date 23 Mar 2007

#include <vil/vil_smart_ptr.h>
#include <vidl/vidl_ostream.h>

//: A smart pointer to an output stream
typedef vil_smart_ptr<vidl_ostream> vidl_ostream_sptr;

#endif // vidl_ostream_sptr_h_
