#include "bstm_cache.h"
//:
// \file

//: global initialization for singleton instance_
bstm_cache_sptr bstm_cache::instance_ = 0;

//: global initialization for singleton destroyer instance
bstm_cache_destroyer bstm_cache::destroyer_;

//: Only one instance should be created (by a derived class)
bstm_cache_sptr bstm_cache::instance()
{
  if (!instance_)
    vcl_cerr<<"warning: bstm_cache:: instance has not been created\n";
  return instance_;
}

//: Binary write bstm_cache  to stream
void vsl_b_write(vsl_b_ostream& os, bstm_cache const& scene){}
void vsl_b_write(vsl_b_ostream& os, const bstm_cache* &p){}
void vsl_b_write(vsl_b_ostream& os, bstm_cache_sptr& sptr){}
void vsl_b_write(vsl_b_ostream& os, bstm_cache_sptr const& sptr){}

//: Binary load bstm_cache  from stream.
void vsl_b_read(vsl_b_istream& is, bstm_cache &scene){}
void vsl_b_read(vsl_b_istream& is, bstm_cache* p){}
void vsl_b_read(vsl_b_istream& is, bstm_cache_sptr& sptr){}
void vsl_b_read(vsl_b_istream& is, bstm_cache_sptr const& sptr){}


bstm_cache_destroyer::bstm_cache_destroyer(bstm_cache_sptr s)
{
  s_ = s;
}

//: the destructor deletes the instance
bstm_cache_destroyer::~bstm_cache_destroyer()
{
  if (s_ != 0)
    s_->unref();  // smart pointer is deleted when ref cnt is zero
}

void bstm_cache_destroyer::set_singleton(bstm_cache_sptr s)
{
  s_ = s;
}

