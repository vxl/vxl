// This is core/vbl/vbl_basic_tuple.h
#ifndef vbl_basic_tuple_h
#define vbl_basic_tuple_h

//:
// \file
// \author Rupert Curwen, GE Corporate Research and Development
// \date   Jan 13th, 1998
//
// \verbatim
// Modifications
// PDA (Manchester) 23/03/2001: Tidied up the documentation
// IMS (Manchester) 21/10/2003: Deprecated - Decision at Providence 2002 Meeting.
//                              Can be deleted after VXL-1.1
// \endverbatim

#include <vcl_iostream.h>
#include <vbl/vbl_basic_optional.h>
#include <vbl/vbl_protection_traits.h>
#include <vcl_deprecated_header.h>

#ifdef __STL_USE_ABBREVS
#define vbl_basic_tuple vBl_Bt
#endif

//: A type safe tuple used by vbl_basic_relation
// The tuple is templated, and accepts a varying number of
// classes, from one to five.  This restriction of up to five
// attributes per tuple may be increased as needed.
//
// \deprecated Because no-one knows what it does.
template <class T1, class T2,
          VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T4,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_basic_tuple
{
  T1 first;
  T2 second;
  T3 third;
  T4 fourth;
  T5 fifth;
 public:
  // Constructors.
  vbl_basic_tuple()
  {
    vbl_protection_traits<T1>::def(first);
    vbl_protection_traits<T2>::def(second);
    if (!vbl_basic_optional_traits<T3>::IsOptional)
      vbl_protection_traits<T3>::def(third);
    if (!vbl_basic_optional_traits<T4>::IsOptional)
      vbl_protection_traits<T4>::def(fourth);
    if (!vbl_basic_optional_traits<T5>::IsOptional)
      vbl_protection_traits<T5>::def(fifth);
  }
  vbl_basic_tuple(T1 t1,T2 t2)
    : first(t1), second(t2)
  {
    if (!vbl_basic_optional_traits<T3>::IsOptional ||
        vbl_basic_optional_traits<T2>::IsOptional)
      vbl_basic_optional::Error("vbl_basic_tuple constructed \
with wrong number of arguments.");
    vbl_protection_traits<T1>::protect(first);
    vbl_protection_traits<T2>::protect(second);
  }
  vbl_basic_tuple(T1 t1,T2 t2,T3 t3)
    : first(t1), second(t2), third(t3)
  {
    if (!vbl_basic_optional_traits<T4>::IsOptional ||
        vbl_basic_optional_traits<T3>::IsOptional)
      vbl_basic_optional::Error("vbl_basic_tuple constructed \
with wrong number of arguments.");
    vbl_protection_traits<T1>::protect(first);
    vbl_protection_traits<T2>::protect(second);
    vbl_protection_traits<T3>::protect(third);
  }
  vbl_basic_tuple(T1 t1,T2 t2,T3 t3,T4 t4)
    : first(t1), second(t2), third(t3), fourth(t4)
  {
    if (!vbl_basic_optional_traits<T5>::IsOptional ||
        vbl_basic_optional_traits<T4>::IsOptional)
      vbl_basic_optional::Error("vbl_basic_tuple constructed \
with wrong number of arguments.");
    vbl_protection_traits<T1>::protect(first);
    vbl_protection_traits<T2>::protect(second);
    vbl_protection_traits<T3>::protect(third);
    vbl_protection_traits<T4>::protect(fourth);
  }
  vbl_basic_tuple(T1 t1,T2 t2,T3 t3,T4 t4,T5 t5)
    : first(t1), second(t2), third(t3), fourth(t4), fifth(t5)
  {
    if (vbl_basic_optional_traits<T5>::IsOptional)
      vbl_basic_optional::Error("vbl_basic_tuple constructed \
with wrong number of arguments.");
    vbl_protection_traits<T1>::protect(first);
    vbl_protection_traits<T2>::protect(second);
    vbl_protection_traits<T3>::protect(third);
    vbl_protection_traits<T4>::protect(fourth);
    vbl_protection_traits<T5>::protect(fifth);
  }

  // Copy constructor.
  vbl_basic_tuple(const vbl_basic_tuple<T1,T2,T3,T4,T5>& tuple)
  {
    first = tuple.first;
    vbl_protection_traits<T1>::protect(first);
    second = tuple.second;
    vbl_protection_traits<T2>::protect(second);
    if (!vbl_basic_optional_traits<T3>::IsOptional)
    {
      third = tuple.third;
      vbl_protection_traits<T3>::protect(third);
    }
    if (!vbl_basic_optional_traits<T4>::IsOptional)
    {
      fourth = tuple.fourth;
      vbl_protection_traits<T4>::protect(fourth);
    }
    if (!vbl_basic_optional_traits<T5>::IsOptional)
    {
      fifth = tuple.fifth;
      vbl_protection_traits<T5>::protect(fifth);
    }
  }

  // Destructor.
  ~vbl_basic_tuple()
  {
    vbl_protection_traits<T1>::unprotect(first);
    vbl_protection_traits<T2>::unprotect(second);
    if (!vbl_basic_optional_traits<T3>::IsOptional)
      vbl_protection_traits<T3>::unprotect(third);
    if (!vbl_basic_optional_traits<T4>::IsOptional)
      vbl_protection_traits<T4>::unprotect(fourth);
    if (!vbl_basic_optional_traits<T5>::IsOptional)
      vbl_protection_traits<T5>::unprotect(fifth);
  }

  // Operator =.
  inline vbl_basic_tuple<T1,T2,T3,T4,T5>&
    operator=(const vbl_basic_tuple<T1,T2,T3,T4,T5>& tuple)
  {
    if (this == &tuple) return *this;

    T1 t1 = first;
    first = tuple.first;
    vbl_protection_traits<T1>::protect(first);
    vbl_protection_traits<T1>::unprotect(t1);

    T2 t2 = second;
    second = tuple.second;
    vbl_protection_traits<T2>::protect(second);
    vbl_protection_traits<T2>::unprotect(t2);

    if (!vbl_basic_optional_traits<T3>::IsOptional)
    {
      T3 t3 = third;
      third = tuple.third;
      vbl_protection_traits<T3>::protect(third);
      vbl_protection_traits<T3>::unprotect(t3);
    }

    if (!vbl_basic_optional_traits<T4>::IsOptional)
    {
      T4 t4 = fourth;
      fourth = tuple.fourth;
      vbl_protection_traits<T4>::protect(fourth);
      vbl_protection_traits<T4>::unprotect(t4);
    }

    if (!vbl_basic_optional_traits<T5>::IsOptional)
    {
      T5 t5 = fifth;
      fifth = tuple.fifth;
      vbl_protection_traits<T5>::protect(fifth);
      vbl_protection_traits<T5>::unprotect(t5);
    }

    return *this;
  }

  // Print the tuple on cout.
  inline void Print() { Dump(vcl_cout); }

  // Print the tuple on cerr.
  void Dump(vcl_ostream& str = vcl_cerr) const
  {
    str << '<' << first << ',' << second;
    if (!vbl_basic_optional_traits<T3>::IsOptional)
      str << ',' << third;
    if (!vbl_basic_optional_traits<T4>::IsOptional)
      str << ',' << fourth;
    if (!vbl_basic_optional_traits<T5>::IsOptional)
      str << ',' << fifth;
    str << '>';
  }

  inline T1 GetFirst() const { return first; }
  inline T2 GetSecond() const { return second; }
  inline T3 GetThird() const { return third; }
  inline T4 GetFourth() const { return fourth; }
  inline T5 GetFifth() const { return fifth; }

  inline void SetFirst(T1 t)
  {
    T1 t1 = first;
    first = t;
    vbl_protection_traits<T1>::protect(first);
    vbl_protection_traits<T1>::unprotect(t1);
  }

  inline void SetSecond(T2 t)
  {
    T2 t2 = second;
    second = t;
    vbl_protection_traits<T2>::protect(second);
    vbl_protection_traits<T2>::unprotect(t2);
  }

  inline void SetThird(T3 t)
  {
    if (!vbl_basic_optional_traits<T3>::IsOptional)
    {
      T3 t3 = third;
      third = t;
      vbl_protection_traits<T3>::protect(third);
      vbl_protection_traits<T3>::unprotect(t3);
    }
  }

  inline void SetFourth(T4 t)
  {
    if (!vbl_basic_optional_traits<T4>::IsOptional)
    {
      T4 t4 = fourth;
      fourth = t;
      vbl_protection_traits<T4>::protect(fourth);
      vbl_protection_traits<T4>::unprotect(t4);
    }
  }

  inline void SetFifth(T5 t)
  {
    if (!vbl_basic_optional_traits<T3>::IsOptional)
    {
      T5 t5 = fifth;
      fifth = t;
      vbl_protection_traits<T5>::protect(fifth);
      vbl_protection_traits<T5>::unprotect(t5);
    }
  }
};

template
<class T1,
 class T2,
 class T3,
 class T4,
 class T5>
inline bool operator==(const vbl_basic_tuple<T1,T2,T3,T4,T5>& x,
                       const vbl_basic_tuple<T1,T2,T3,T4,T5>& y)
{
  bool res = (x.GetFirst() == y.GetFirst()) &&
    (x.GetSecond() == y.GetSecond());
  if (!vbl_basic_optional_traits<T3>::IsOptional)
    res = res && (x.GetThird() == y.GetThird());
  if (!vbl_basic_optional_traits<T4>::IsOptional)
    res = res && (x.GetFourth() == y.GetFourth());
  if (!vbl_basic_optional_traits<T5>::IsOptional)
    res = res && (x.GetFifth() == y.GetFifth());
  return res;
}

template
<class T1,
 class T2,
 class T3,
 class T4,
 class T5>
inline vcl_ostream& operator<<(vcl_ostream& str,
                               const vbl_basic_tuple<T1,T2,T3,T4,T5>& tuple)
{
  tuple.Dump(str);
  return str;
}

#endif // vbl_basic_tuple_h
