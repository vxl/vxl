// This is core/vbl/vbl_quadruple.h
#ifndef vbl_quadruple_h_
#define vbl_quadruple_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a templated 4-tuple
// \author fsm

#include <vcl_compiler.h>

//: a templated 4-tuple
template <class T1, class T2, class T3, class T4>
struct vbl_quadruple
{
  typedef T1 first_type;
  typedef T2 second_type;
  typedef T3 third_type;
  typedef T4 fourth_type;

  T1 first;
  T2 second;
  T3 third;
  T4 fourth;

  vbl_quadruple() { }
  vbl_quadruple(T1 const &a, T2 const &b, T3 const &c, T4 const &d)
    : first (a)
    , second(b)
    , third (c)
    , fourth(d) { }
#if VCL_HAS_MEMBER_TEMPLATES
  template <class U1, class U2, class U3, class U4>
  vbl_quadruple(vbl_quadruple<U1, U2, U3, U4> const &that)
    : first (that.first )
    , second(that.second)
    , third (that.third )
    , fourth(that.fourth) { }
#endif
};

template <class T1, class T2, class T3, class T4>
inline bool operator==(vbl_quadruple<T1, T2, T3, T4> const &x, vbl_quadruple<T1, T2, T3, T4> const &y)
{
  return
    x.first  == y.first  &&
    x.second == y.second &&
    x.third  == y.third  &&
    x.fourth == y.fourth;
}

template <class T1, class T2, class T3, class T4>
inline bool operator!=(vbl_quadruple<T1, T2, T3, T4> const &x, vbl_quadruple<T1, T2, T3, T4> const &y)
{ return !(x == y); }

template <class T1, class T2, class T3, class T4>
inline bool operator< (vbl_quadruple<T1, T2, T3, T4> const &x, vbl_quadruple<T1, T2, T3, T4> const &y)
{
  return x.first  != y.first  ? x.first  < y.first  :
         x.second != y.second ? x.second < y.second :
         x.third  != y.third  ? x.third  < y.third  :
         x.fourth != y.fourth ? x.fourth < y.fourth :
         false;
}

template <class T1, class T2, class T3, class T4>
inline vbl_quadruple<T1, T2, T3, T4> vbl_make_quadruple(T1 const &x, T2 const &y, T3 const &z, T4 const &w)
{
  return vbl_quadruple<T1, T2, T3, T4>(x, y, z, w);
}

#endif // vbl_quadruple_h_
