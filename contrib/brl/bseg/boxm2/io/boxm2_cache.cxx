#include "boxm2_cache.h"
//:
// \file

//: global initialization for singleton instance_
boxm2_cache_sptr boxm2_cache::instance_ = 0;

//: global initialization for singleton destroyer instance
boxm2_cache_destroyer boxm2_cache::destroyer_;

//: Only one instance should be created (by a derived class)
boxm2_cache_sptr boxm2_cache::instance()
{
  if (!instance_)
    vcl_cerr<<"warning: boxm2_cache:: instance has not been created\n";
  return instance_;
}

//: Binary write boxm2_cache  to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_cache const& scene){}
void vsl_b_write(vsl_b_ostream& os, const boxm2_cache* &p){}
void vsl_b_write(vsl_b_ostream& os, boxm2_cache_sptr& sptr){}
void vsl_b_write(vsl_b_ostream& os, boxm2_cache_sptr const& sptr){}

//: Binary load boxm2_cache  from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_cache &scene){}
void vsl_b_read(vsl_b_istream& is, boxm2_cache* p){}
void vsl_b_read(vsl_b_istream& is, boxm2_cache_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, boxm2_cache_sptr const& sptr){}


boxm2_cache_destroyer::boxm2_cache_destroyer(boxm2_cache_sptr s)
{
  s_ = s;
}

//: the destructor deletes the instance
boxm2_cache_destroyer::~boxm2_cache_destroyer()
{
  if (s_ != 0)
    s_->unref();  // smart pointer is deleted when ref cnt is zero
}

void boxm2_cache_destroyer::set_singleton(boxm2_cache_sptr s)
{
  s_ = s;
}

