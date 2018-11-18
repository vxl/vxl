#include "boxm2_cache1.h"
//:
// \file

//: global initialization for singleton instance_
boxm2_cache1_sptr boxm2_cache1::instance_ = nullptr;

//: global initialization for singleton destroyer instance
boxm2_cache1_destroyer boxm2_cache1::destroyer_;

//: Only one instance should be created (by a derived class)
boxm2_cache1_sptr boxm2_cache1::instance()
{
  if (!instance_)
    std::cerr<<"warning: boxm2_cache1:: instance has not been created\n";
  return instance_;
}

//: Binary write boxm2_cache1  to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_cache1 const&  /*scene*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, const boxm2_cache1* & /*p*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_cache1_sptr&  /*sptr*/){}
void vsl_b_write(vsl_b_ostream&  /*os*/, boxm2_cache1_sptr const&  /*sptr*/){}

//: Binary load boxm2_cache1  from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache1 & /*scene*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache1*  /*p*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache1_sptr&  /*sptr*/){}
void vsl_b_read(vsl_b_istream&  /*is*/, boxm2_cache1_sptr const&  /*sptr*/){}


boxm2_cache1_destroyer::boxm2_cache1_destroyer(const boxm2_cache1_sptr& s)
{
  s_ = s;
}

//: the destructor deletes the instance
boxm2_cache1_destroyer::~boxm2_cache1_destroyer()
{
  if (s_ != nullptr)
    s_->unref();  // smart pointer is deleted when ref cnt is zero
}

void boxm2_cache1_destroyer::set_singleton(const boxm2_cache1_sptr& s)
{
  s_ = s;
}
