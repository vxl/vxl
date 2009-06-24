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

class bvxm_opinion
{
 public:
  bvxm_opinion() : u_(1.0), b_(0.0) {}
  bvxm_opinion(float u, float b) : u_(u), b_(b) {}

  //: constructor where uncertinity is assumed to go away when a belief value is set
  bvxm_opinion(float b) { u_=1.0-b; b_=b; }

  ~bvxm_opinion() {}

  float u() { return u_; }
  float b() { return b_; }
  float d() { return d_; }
  float a() { return a_; }
  void set(float u, float b) { u_=u; b_=b; a_=0.5; d_=1-b_; }
  void set_u(float u) { u_=u; }
  void set_b(float b) { b_=b; u_=1.0-b; }
  void set_d(float d) { d_=d; }
  void set_a(float a) { a_=a; }
  float operator()(){ return b_; }
  float operator*() { return b_; }
  float operator*(float o) { return b_*o; }
  float operator*=(bvxm_opinion& o) { return b_=b_*o.b(); }
  float operator*=(float o) { return b_=b_*o; }
  float operator+(bvxm_opinion& o) { return b_+o.b(); }
  float operator+(float o) { return b_+o; }
  float operator+=(bvxm_opinion& o) { return b_=b_+o.b(); }
  float operator+=(float o) { return b_=b_+o; }
  float operator-(bvxm_opinion& o) { return b_-o.b(); }
  float operator-(float o) { return b_-o; }
  float operator/(bvxm_opinion& o) { return b_/o.b(); }
  float operator/(float o) { return b_/o; }
  bool operator>(float o) { return b_>o; }
 private:
  float u_;  // uncertainity value [0.0,1.0], 1.0 if total uncertainity, 0.0 if total certainity
  float b_;  // probability as a belief value [0.0,1.0]
  float d_;  // disbelief, it is d_=1-b_
  float a_;
};

//vcl_ostream& operator<<(vcl_ostream& os, const bvxm_opinion& o);

inline bool operator>(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return lhs.b() > rhs.b();
}

inline bool operator<(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return lhs.b() < rhs.b();
}

inline float operator+(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return lhs.b() + rhs.b();
}

inline float operator*(bvxm_opinion lhs, bvxm_opinion rhs)
{
  return lhs.b() * rhs.b();
}

inline float operator/(float o1, bvxm_opinion o2)
{
  return o1 / o2.b();
}

#endif
