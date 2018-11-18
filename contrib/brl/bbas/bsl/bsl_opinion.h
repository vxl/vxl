#ifndef bsl_opinion_h_
#define bsl_opinion_h_
//:
// \file
// \brief  A binomial opinion, x = (b_,d_,a_,u_).
// * b_: belief
// * d_: disbelief
// * u_: uncertainty about belief
// * a_: atomicity is the prior probability of the two outcomes, usually 0.5
//
// \author Ozge C. Ozcanli
// \date   Jan 26, 2012
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <limits>
#include <cmath>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bsl_opinion
{
 public:
  bsl_opinion() : u_(1.0f), b_(0.0f), a_(0.5f) {}
  bsl_opinion(float u, float b) : u_(u), b_(b), a_(0.5) {}
  bsl_opinion(float u, float b, float a) : u_(u), b_(b), a_(a) {}

  //: constructor where uncertainty is assumed to go away when a belief value is set
  bsl_opinion(float b) { b_=b; u_=1.0f-b; a_ = 0.5f;}

  //: copy constructor
  bsl_opinion(bsl_opinion const& o) : u_(o.u()), b_(o.b()), a_(o.a()) {}

  //: destructor
  ~bsl_opinion() = default;

  float u() const { return u_; }

  float b() const { return b_; }

  float a() const { return a_; }

  float d() const { return 1.0f-b_-u_; }

  float expectation() const { return b_+u_*a_; }

  void set(float u, float b) { u_=u; b_=b; }

  void set_u(float u) { u_=u; }

  void set_b(float b) { b_=b; u_=1.0f-b; }

  void set_a(float a) { a_ = a; }

  bool operator==(bsl_opinion const& o) const { return b_==o.b() && u_==o.u() && a_==o.a(); }

 private:
  float u_;  // uncertainty value [0.0,1.0], 1.0 if total uncertainty, 0.0 if total certainty
  float b_;  // probability as a belief value [0.0,1.0]
  float a_;  // atomicity, default is .5f;
};

inline bsl_opinion fuse(bsl_opinion const& lhs, bsl_opinion const& rhs)
{
  float bA = lhs.b();
  float bB = rhs.b();
  float uA = lhs.u();
  float uB = rhs.u();
  // then, dA = 1.0f-bA-uA
  // and   dB = 1.0f-bB-uB
  float kappa = uA + uB - uA*uB; // = 1 - (1-uA)*(1-uB)

  // if one of the uncertainties is non-zero
  if (std::abs(uA) > std::numeric_limits<float>::epsilon() || std::abs(uB) > std::numeric_limits<float>::epsilon()) {
    float b = (bA*uB + bB*uA)/kappa;
    float u = uA*uB/kappa;
    return bsl_opinion(u,b,lhs.a()); // atomicity should be the same for lhs & rhs
  }
  else {
    bsl_opinion op((bA+bB)/2.0f); op.set_a(lhs.a());
    return op;
  }
}

inline bool operator> (bsl_opinion const& lhs, bsl_opinion const& rhs) { return lhs.b() >  rhs.b(); }
inline bool operator>=(bsl_opinion const& lhs, bsl_opinion const& rhs) { return lhs.b() >= rhs.b(); }
inline bool operator< (bsl_opinion const& lhs, bsl_opinion const& rhs) { return lhs.b() <  rhs.b(); }

inline bsl_opinion operator+(bsl_opinion const& lhs, bsl_opinion const& rhs) { return bsl_opinion(lhs.b()+rhs.b()); }
inline float operator+(bsl_opinion const& lhs, float rhs) { return lhs.b() + rhs; }
inline float operator*(bsl_opinion const& lhs, bsl_opinion const& rhs) { return lhs.b() * rhs.b(); }

inline bsl_opinion operator/(float o1, bsl_opinion const& o2) { return bsl_opinion(o1/o2.b()); }
inline bsl_opinion operator/(bsl_opinion const& o1, bsl_opinion const& o2) { return bsl_opinion(o1.b()/o2.b()); }

inline std::ostream& operator<< (std::ostream& s, bsl_opinion const& o) { s << "bsl_opinion [b=" << o.b() << " u=" << o.u() << ']' << std::endl; return s; }


#endif
