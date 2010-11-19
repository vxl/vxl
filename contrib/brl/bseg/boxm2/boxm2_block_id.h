#ifndef boxm2_block_id_h_
#define boxm2_block_id_h_
//:
// \file
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_string.h>

//: boxm2_cache: top level storage (abstract) class
// - handles all block io, from both the cache/marshaller and the disk
class boxm2_block_id
{
  public:
    //indices are public
    int i_;
    int j_;
    int k_;
    inline int i() const { return i_; }
    inline int j() const { return j_; }
    inline int k() const { return k_; }

    //: Creates 0,0,0 block.
    inline boxm2_block_id () : i_(0) , j_(0), k_(0) {}

    //: Creates boxm2_block_id(i,j,k)
    inline boxm2_block_id (int i, int j, int k) : i_(i) , j_(j), k_(k) {}

    //: assignment
    inline boxm2_block_id& operator=(boxm2_block_id const& v) {
      i_=v.i(); j_=v.j(); k_=v.k(); return *this; }

    //: Comparison
    inline bool operator==(boxm2_block_id const& v)const{return i_==v.i()&&j_==v.j()&&k_==v.k();}
    inline bool operator!=(boxm2_block_id const& v)const{return !operator==(v);}
    inline bool operator<(boxm2_block_id const& v)const{ if(this->i_ > v.i()) return true; else return false;}
    
    //: to string
    vcl_string to_string(); 
};

//: scene output stream operator
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block_id& id);

#endif //boxm2_block_id_h_
