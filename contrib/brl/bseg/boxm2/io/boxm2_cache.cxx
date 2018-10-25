#include "boxm2_cache.h"
//:
// \file

//: global initialization for singleton instance_
boxm2_cache_sptr boxm2_cache::instance_ = nullptr;

//: Only one instance should be created (by a derived class)
boxm2_cache_sptr boxm2_cache::instance()
{
  if (!instance_)
    std::cerr<<"warning: boxm2_cache:: instance has not been created\n";
  return instance_;
}

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_cache const&  /*scene*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, const boxm2_cache* & /*p*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_cache_sptr&  /*sptr*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_cache_sptr const&  /*sptr*/){}

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache & /*scene*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache*  /*p*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache_sptr&  /*sptr*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache_sptr const&  /*sptr*/){}
