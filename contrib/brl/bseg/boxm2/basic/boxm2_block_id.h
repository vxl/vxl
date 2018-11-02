#ifndef boxm2_block_id_h_
#define boxm2_block_id_h_
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
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//: boxm2_cache: top level storage (abstract) class
// - handles all block io, from both the cache/marshaller and the disk
class boxm2_block_id : public vbl_ref_count
{
  public:
    //indices are public
    int i_;
    int j_;
    int k_;
    inline int i() const { return i_; }
    inline int j() const { return j_; }
    inline int k() const { return k_; }

    //: Creates "uninitialized" block - should never exist
    inline boxm2_block_id () : i_(-65536) , j_(-65536), k_(-65536) {}

    //: Creates boxm2_block_id(i,j,k)
    inline boxm2_block_id (int i, int j, int k) : i_(i) , j_(j), k_(k) {}

    //: copy constructor
    inline boxm2_block_id (const boxm2_block_id& that) : vbl_ref_count(), i_(that.i()), j_(that.j()), k_(that.k()) {}

    //: assignment
    inline boxm2_block_id& operator=(boxm2_block_id const& v) {
      i_=v.i(); j_=v.j(); k_=v.k(); return *this;
    }

    //: Comparison
    inline bool operator==(boxm2_block_id const& v) const { return i_==v.i()&&j_==v.j()&&k_==v.k(); }
    inline bool operator!=(boxm2_block_id const& v) const { return !operator==(v); }
    inline bool operator< (boxm2_block_id const& v) const {
      return i_<v.i()
         ||  (i_==v.i() && j_<v.j())
         ||  (i_==v.i() && j_==v.j() && k_<v.k());
    }
    inline bool operator<=(boxm2_block_id const& v) const {
      return i_<=v.i()
         ||  (i_==v.i() && j_<=v.j())
         ||  (i_==v.i() && j_==v.j() && k_<=v.k());
    }
    inline bool operator> (boxm2_block_id const& v) const { return !operator<=(v); }
    inline bool operator>=(boxm2_block_id const& v) const { return !operator<(v); }

    //: to string
    std::string to_string() const;
};

typedef vbl_smart_ptr<boxm2_block_id> boxm2_block_id_sptr;

//: scene output stream operator
std::ostream& operator <<(std::ostream &s, boxm2_block_id const& id);


//------IO Necessary for smart pointer linking ---------------------------------
//: Binary write boxm2_block to stream.
void vsl_b_write(vsl_b_ostream& os, boxm2_block_id_sptr const& sptr);

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_id_sptr& sptr);

//: Binary load boxm2_block from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block_id_sptr const& sptr);


#endif //boxm2_block_id_h_
