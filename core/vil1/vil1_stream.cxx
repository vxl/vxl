// This is vxl/vil/vil_stream.cxx

//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_stream.h"

#include <vcl_cassert.h>

#define log_xtor 0
#if log_xtor
# include <vcl_iostream.h>
#endif

vil_stream::vil_stream()
{
  refcount = 0;
#if log_xtor
  vcl_cerr << __FILE__ ", vil_stream ctor : this = " << (void*)this << vcl_endl;
#endif
}

vil_stream::~vil_stream()
{
#if log_xtor
  vcl_cerr << __FILE__ ", vil_stream dtor : this = " << (void*)this << vcl_endl;
#endif
}


void vil_stream::ref()
{
  ++refcount; 
}

void vil_stream::unref() 
{
  assert(refcount >= 0); // negative refcount is very serious
  if (--refcount == 0)
    delete this;
}
