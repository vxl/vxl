// This is core/vil/vil_stream.cxx

#include "vil_stream.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define log_xtor 0
#if log_xtor
# include <std::iostream.h>
#endif

vil_stream::vil_stream()
: refcount_(0)
{
#if log_xtor
  std::cerr << __FILE__ ", vil_stream ctor : this = " << (void*)this << std::endl;
#endif
}

vil_stream::~vil_stream()
{
#if log_xtor
  std::cerr << __FILE__ ", vil_stream dtor : this = " << (void*)this << std::endl;
#endif
}


void vil_stream::unref()
{
  assert(refcount_ >= 0); // negative refcount is very serious
  if (--refcount_ == 0)
    delete this;
}
