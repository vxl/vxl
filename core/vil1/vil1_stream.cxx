//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_stream.h"

#include <vcl/vcl_cassert.h>

#define log_xtor 0
#if log_xtor
# include <vcl/vcl_iostream.h>
#endif

vil_stream::vil_stream()
{
  refcount = 0;
#if log_xtor
  cerr << __FILE__ ", vil_stream ctor : this = " << (void*)this << endl;
#endif
}

vil_stream::~vil_stream()
{
#if log_xtor
  cerr << __FILE__ ", vil_stream dtor : this = " << (void*)this << endl;
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
