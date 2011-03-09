#include "boxm2_cache.h"
//:
// \file

//: global initialization for singleton instance_
boxm2_cache* boxm2_cache::instance_ = 0;

//: Only one instance should be created (by a derived class)
boxm2_cache* boxm2_cache::instance()
{
  if (!instance_)
    vcl_cerr<<"warning: boxm2_cache:: instance has not been created\n";
  return instance_;
}
