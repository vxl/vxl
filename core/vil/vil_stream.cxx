// This is contrib/mul/vil2/vil2_stream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil2_stream.h"

#include <vcl_cassert.h>

#define log_xtor 0
#if log_xtor
# include <vcl_iostream.h>
#endif

vil2_stream::vil2_stream()
{
  refcount_ = 0;
#if log_xtor
  vcl_cerr << __FILE__ ", vil2_stream ctor : this = " << (void*)this << vcl_endl;
#endif
}

vil2_stream::~vil2_stream()
{
#if log_xtor
  vcl_cerr << __FILE__ ", vil2_stream dtor : this = " << (void*)this << vcl_endl;
#endif
}


void vil2_stream::ref()
{
  ++refcount_; 
}

void vil2_stream::unref() 
{
  assert(refcount_ >= 0); // negative refcount is very serious
  if (--refcount_ == 0)
    delete this;
}
