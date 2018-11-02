#ifndef bvxm_opinion_h_
#define bvxm_opinion_h_
//:
// \file
// \brief  This class is for keeping the uncertainty values beside belief probabilities.
// \       When there is no data to support the value b_ available yet, u_ is 1.0 and it
//         starts to decrease when some observations builds up some belief and goes to 0.0
//         when certainty achieved.
//
// \author Gamze Tunali
// \date   June 12, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bvxm_opinion
{
 public:
  bvxm_opinion() : u_(1.0f), b_(0.0f) {}
  bvxm_opinion(float u, float b) : u_(u), b_(b) {}

  //: constructor where uncertainty is assumed to go away when a belief value is set
  bvxm_opinion(float b) { b_=b; u_=1.0f-b; }

  //: copy constructor
  bvxm_opinion(bvxm_opinion const& o) : u_(o.u()), b_(o.b()) {}

  //: destructor
  ~bvxm_opinion() = default;

  float u() const { return u_; }

  float b() const { return b_; }

  float expectation() const { return b_+u_*0.5f; }

  void set(float u, float b) { u_=u; b_=b; }

  void set_u(float u) { u_=u; }

  void set_b(float b) { b_=b; u_=1.0f-b; }

  float operator*(float o) const { bvxm_opinion obj(b_*o); return obj.b(); }

  bvxm_opinion& operator*=(bvxm_opinion const& o) {  b_*=o.b(); return *this; }

  bvxm_opinion& operator*=(float o) {  b_*=o; return *this; }

  bvxm_opinion operator+(bvxm_opinion const& o) const { return bvxm_opinion(b_+o.b()); }

  float operator+(float o) const { bvxm_opinion obj(b_+o); return obj.b(); }

  bvxm_opinion& operator+=(bvxm_opinion const& o) { b_+=o.b(); return *this; }

  bvxm_opinion& operator+=(float o) { b_+=o; return *this; }

  bvxm_opinion operator-(bvxm_opinion const& o) const { return bvxm_opinion(b_-o.b()); }

  float operator-(float o) const { bvxm_opinion obj(b_-o); return obj.b(); }

  bvxm_opinion operator/(bvxm_opinion const& o) const { return bvxm_opinion(b_/o.b()); }

  bvxm_opinion operator/(float o) const { return bvxm_opinion(b_/o); }

  bool operator>(float o) const { return b_>o; }

  bool operator==(bvxm_opinion const& o) const { return b_==o.b() && u_==o.u(); }
 private:
  float u_;  // uncertainty value [0.0,1.0], 1.0 if total uncertainty, 0.0 if total certainty
  float b_;  // probability as a belief value [0.0,1.0]
};

inline bool operator>(bvxm_opinion const& lhs, bvxm_opinion const& rhs) { return lhs.b() > rhs.b(); }
inline bool operator>=(bvxm_opinion const& lhs, bvxm_opinion const& rhs) { return lhs.b() >= rhs.b(); }

inline bool operator<(bvxm_opinion const& lhs, bvxm_opinion const& rhs) { return lhs.b() < rhs.b(); }

inline bvxm_opinion operator+(bvxm_opinion const& lhs, bvxm_opinion const& rhs) { return bvxm_opinion(lhs.b()+rhs.b()); }

inline float operator+(bvxm_opinion const& lhs, float rhs) { return lhs.b() + rhs; }

inline float operator*(bvxm_opinion const& lhs, bvxm_opinion const& rhs) { return lhs.b() * rhs.b(); }

inline bvxm_opinion operator/(float const& o1, bvxm_opinion const& o2) { return bvxm_opinion(o1/o2.b()); }

inline bvxm_opinion operator/(bvxm_opinion const& o1, bvxm_opinion const& o2) { return bvxm_opinion(o1.b()/o2.b()); }

inline std::ostream& operator<< (std::ostream& s, bvxm_opinion const& o) { s << "bvxm_opinion [b=" << o.b() << " u=" << o.u() << ']' << std::endl; return s; }

#endif
