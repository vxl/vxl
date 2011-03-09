#include "boxm2_cache.h"

//: global initialization for singleton instance_
boxm2_cache* boxm2_cache::instance_ = 0;

//: Insure only one instance is created
boxm2_cache* boxm2_cache::instance()
{
  if (!instance_) 
    vcl_cout<<"boxm2_cache:: instance has not been created "<<vcl_endl;
  return boxm2_cache::instance_;
}
