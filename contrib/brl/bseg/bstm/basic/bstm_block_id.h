#ifndef bstm_block_id_h_
#define bstm_block_id_h_
//:
// \file
#include <iostream>
#include <sstream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

//smart pointer stuff
#include <vbl/vbl_smart_ptr.h>

#include <boxm2/basic/boxm2_block_id.h>

class bstm_block_id : public boxm2_block_id
{
 public:

  int t_;
  inline int t() const { return t_; }

  //: Creates "uninitialized" block - should never exist
  inline bstm_block_id () : boxm2_block_id(), t_(-65536) {}

  //: Creates bstm_block_id(i,j,k,t)
  inline bstm_block_id (int i, int j, int k, int t) : boxm2_block_id(i,j,k), t_(t) {}

  //: copy constructor
  inline bstm_block_id (const bstm_block_id& that) : boxm2_block_id(that), t_(that.t()) {}

  //operators
  inline bool operator==(bstm_block_id const& v) const { return i_==v.i()&&j_==v.j()&&k_==v.k()&&t_==v.t(); }
  inline bool operator==(boxm2_block_id const& v) const { return i_==v.i()&&j_==v.j()&&k_==v.k(); }

  inline bool operator!=(bstm_block_id const& v) const { return !operator==(v); }

  inline bool operator< (bstm_block_id const& v) const {
    return i_<v.i()
       ||  (i_==v.i() && j_<v.j())
       ||  (i_==v.i() && j_==v.j() && k_<v.k())
       ||  (i_==v.i() && j_==v.j() && k_==v.k() && t_<v.t()) ;
  }

  inline bool operator<=(bstm_block_id const& v) const {
      return i_<=v.i()
         ||  (i_==v.i() && j_<=v.j())
         ||  (i_==v.i() && j_==v.j() && k_<=v.k())
         ||  (i_==v.i() && j_==v.j() && k_==v.k() && t_<=v.t());
  }

  inline bool operator> (bstm_block_id const& v) const { return !operator<=(v); }
  inline bool operator>=(bstm_block_id const& v) const { return !operator<(v); }

  //: to string
  virtual std::string to_string() const;
};


typedef vbl_smart_ptr<bstm_block_id> bstm_block_id_sptr;

//: scene output stream operator
std::ostream& operator <<(std::ostream &s, bstm_block_id const& id);


//------IO Necessary for smart pointer linking ---------------------------------
//: Binary write boxm2_block to stream.
void vsl_b_write(vsl_b_ostream& os, bstm_block_id_sptr const& sptr);

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, bstm_block_id_sptr& sptr);

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, bstm_block_id_sptr const& sptr);

#endif //bstm_block_id_h_
