// This is core/vil1/vil1_stream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_stream.h"

#include <vcl_cassert.h>

#define log_xtor 0
#if log_xtor
# include <vcl_iostream.h>
#endif

vil1_stream::vil1_stream()
{
  refcount_ = 0;
#if log_xtor
  vcl_cerr << __FILE__ ", vil1_stream ctor : this = " << (void*)this << vcl_endl;
#endif
}

vil1_stream::~vil1_stream()
{
#if log_xtor
  vcl_cerr << __FILE__ ", vil1_stream dtor : this = " << (void*)this << vcl_endl;
#endif
}


void vil1_stream::unref()
{
  assert(refcount_ >= 0); // negative refcount is very serious
  if (--refcount_ == 0)
    delete this;
}
