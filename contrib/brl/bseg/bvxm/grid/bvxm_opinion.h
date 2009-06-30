#ifndef bvxm_opinion_h_
#define bvxm_opinion_h_
//:
// \file
// \brief  This class is for keeping the uncertainity values beside belief probabilities.
// \       When there is no data to support the value b_ available yet, u_ is 1.0 and it
//         starts to decrease when some observations builds up some belief and goes to 0.0
//         when certainity achieved.
// \
// \author Gamze Tunali
// \date   June 12, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "vcl_iostream.h"

class bvxm_opinion
{

 public:
  bvxm_opinion() : u_(1.0f), b_(0.0f) {}
  bvxm_opinion(float u, float b) : u_(u), b_(b) {}

  //: constructor where uncertinity is assumed to go away when a belief value is set
  bvxm_opinion(float b) {b_=b; u_=1.0f-b; }

  //: copy constructor
  bvxm_opinion(bvxm_opinion const& o) : u_(o.u()), b_(o.b()) {}

  //: destructor
  ~bvxm_opinion(){}

  float u() const {return u_; }
  float b() const {return b_; }
  
  void set(float u, float b) { u_=u; b_=b;}

  void set_u(float u) { u_=u; }
  void set_b(float b) { b_=b; u_=1.0f-b; }

  float operator*(float o) { 
    bvxm_opinion obj(b_*o); 
    return obj.b();
  }

  bvxm_opinion& operator*=(bvxm_opinion& o) {  
    b_*=o.b(); 
    return *this;
  }

  bvxm_opinion& operator*=(float o) {  
    b_*=o; 
    return *this;
  }

  bvxm_opinion operator+(bvxm_opinion& o) { 
    bvxm_opinion obj(b_+o.b());
    return obj;
  }

  float operator+(float o) { 
    bvxm_opinion obj(b_+o); 
    //return obj;
    return obj.b();
  }

  bvxm_opinion& operator+=(bvxm_opinion& o) { 
    b_+=o.b(); 
    return *this;
  }

  bvxm_opinion& operator+=(float o) { 
    b_+=o;  
    return *this;
  }

  bvxm_opinion operator-(bvxm_opinion& o) { 
    bvxm_opinion obj(b_-o.b()); 
    return obj;
  }

  float operator-(float o) { 
    bvxm_opinion obj(b_-o); 
    return obj.b();
  }

  bvxm_opinion operator/(bvxm_opinion& o) { 
    bvxm_opinion obj(b_/o.b()); 
    return obj;
  }

  bvxm_opinion operator/(float o) { 
    bvxm_opinion obj(b_/o); 
    return obj; }

  bool operator>(float o) { 
    return (b_>o); }

  bool operator==(bvxm_opinion& o) { 
    if (b_==o.b() && u_==o.u())
      return true;
    return false;
  }
private:
  float u_;  // uncertainity value [0.0,1.0], 1.0 if total uncertainity, 0.0 if total certainity
  float b_;  // probability as a belief value [0.0,1.0]
};

inline bool operator>(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return (lhs.b() > rhs.b());
}

inline bool operator<(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return (lhs.b() < rhs.b());
}

inline bvxm_opinion operator+(bvxm_opinion lhs, bvxm_opinion rhs)
{
  float a = lhs.b();
  float b = rhs.b();
  return bvxm_opinion(a+b);
}
inline float operator+(bvxm_opinion lhs, float rhs)
{
  float a = lhs.b();
  return (a+rhs);
}
inline float operator*(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return lhs.b() * rhs.b();
}

inline bvxm_opinion operator/(float o1, bvxm_opinion const& o2) { return bvxm_opinion(o1/o2.b()); }

inline bvxm_opinion operator/(bvxm_opinion const& o1, bvxm_opinion const& o2) { return bvxm_opinion(o1.b()/o2.b()); }

inline vcl_ostream& operator<< (vcl_ostream& s, bvxm_opinion& o) { s << "bvxm_opinion [b=" << o.b() << " u=" << o.u() << "]" << vcl_endl; return s;}

#endif
