// This is core/vbl/vbl_triple.h
#ifndef vbl_triple_h_
#define vbl_triple_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief a templated 3-tuple
// \author fsm

#include <vcl_compiler.h>

//: a templated 3-tuple
template <class T1, class T2, class T3>
struct vbl_triple
{
  typedef T1 first_type;
  typedef T2 second_type;
  typedef T3 third_type;

  T1 first;
  T2 second;
  T3 third;

  vbl_triple() { }
  vbl_triple(T1 const &a, T2 const &b, T3 const &c)
    : first (a)
    , second(b)
    , third (c) { }
#if VCL_HAS_MEMBER_TEMPLATES
  template <class U1, class U2, class U3>
  vbl_triple(vbl_triple<U1, U2, U3> const &that)
    : first (that.first )
    , second(that.second)
    , third (that.third ) { }
#endif
};

template <class T1, class T2, class T3>
inline bool operator==(vbl_triple<T1, T2, T3> const &x, vbl_triple<T1, T2, T3> const &y)
{
  return
    x.first  == y.first  &&
    x.second == y.second &&
    x.third  == y.third;
}

template <class T1, class T2, class T3>
inline bool operator!=(vbl_triple<T1, T2, T3> const &x, vbl_triple<T1, T2, T3> const &y)
{ return !(x == y); }

template <class T1, class T2, class T3>
inline bool operator< (vbl_triple<T1, T2, T3> const &x, vbl_triple<T1, T2, T3> const &y)
{
  return x.first  != y.first  ? x.first  < y.first  :
         x.second != y.second ? x.second < y.second :
         x.third  != y.third  ? x.third  < y.third  :
         false;
}

template <class T1, class T2, class T3>
inline vbl_triple<T1, T2, T3> vbl_make_triple(T1 const &x, T2 const &y, T3 const &z)
{
  return vbl_triple<T1, T2, T3>(x, y, z);
}

#endif // vbl_triple_h_
