// This is core/vidl/vidl_istream_sptr.h
#ifndef vidl_istream_sptr_h_
#define vidl_istream_sptr_h_
//:
// \file
// \brief A smart pointer to an input stream
//
// \author Matt Leotta
// \date 23 Mar 2007

#include <vil/vil_smart_ptr.h>
#include <vidl/vidl_istream.h>

//: A smart pointer to an input stream
typedef vil_smart_ptr<vidl_istream> vidl_istream_sptr;

#endif // vidl_istream_sptr_h_
