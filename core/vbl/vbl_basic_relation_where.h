// This is core/vbl/vbl_basic_relation_where.h
#ifndef vbl_basic_relation_where_h
#define vbl_basic_relation_where_h
//:
// \file
// \author Rupert W. Curwen, GE Corporate Research and Development
// \date   Nov 13, 1997
//
// \verbatim
//  Modifications
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   IMS (Manchester) 21/10/2003: Deprecated - Decision at Providence 2002 Meeting.
//                               Can be deleted after VXL-1.1
// \endverbatim


#include <vbl/vbl_basic_tuple.h>
#include <vcl_iosfwd.h>
#include <vcl_deprecated_header.h>

//: A tuple used to select in vbl_basic_relation
//  This class is used internal to the vbl_basic_relation class to specify
//  the attributes currently selected in the relation.
//
// \deprecated Because no-one knows what it does.
template <class T1, class T2,
          VCL_DFL_TYPE_PARAM_STLDECL(T3,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T4,vbl_basic_optional),
          VCL_DFL_TYPE_PARAM_STLDECL(T5,vbl_basic_optional)>
class vbl_basic_relation_where : public vbl_basic_tuple<T1,T2,T3,T4,T5>
{
  // Wild card attributes
  bool first_wild;
  bool second_wild;
  bool third_wild;
  bool fourth_wild;
  bool fifth_wild;

 public:
  //: Constructors.
  vbl_basic_relation_where()
    : first_wild(true),
      second_wild(true),
      third_wild(true),
      fourth_wild(true),
      fifth_wild(true)
  {
  }

  //: Copy constructor.
  vbl_basic_relation_where(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w)
    : vbl_basic_tuple<T1,T2,T3,T4,T5>(w),
      first_wild(w.first_wild),
      second_wild(w.second_wild),
      third_wild(w.third_wild),
      fourth_wild(w.fourth_wild),
      fifth_wild(w.fifth_wild)
  {
  }

  //: Construct exact clause.
  vbl_basic_relation_where(const vbl_basic_tuple<T1,T2,T3,T4,T5>& t)
    : vbl_basic_tuple<T1,T2,T3,T4,T5>(t),
      first_wild(false),
      second_wild(false),
      third_wild(false),
      fourth_wild(false),
      fifth_wild(false)
  {
  }

  //: Destructor.
  ~vbl_basic_relation_where()
  {
  }

  //: Operator =.
  inline vbl_basic_relation_where<T1,T2,T3,T4,T5>&
    operator=(const vbl_basic_relation_where<T1,T2,T3,T4,T5>& w)
  {
    if (this == &w) return *this;

    (*(vbl_basic_tuple<T1,T2,T3,T4,T5>*)this) = w;
    first_wild = w.first_wild;
    second_wild = w.second_wild;
    if (!vbl_basic_optional_traits<T3>::IsOptional)
      third_wild = w.third_wild;
    if (!vbl_basic_optional_traits<T4>::IsOptional)
      fourth_wild = w.fourth_wild;
    if (!vbl_basic_optional_traits<T5>::IsOptional)
      fifth_wild = w.fifth_wild;

    return *this;
  }

  //: Test for match.
  inline bool match(const vbl_basic_tuple<T1,T2,T3,T4,T5>& w)
  {
    if ((!first_wild) && !(GetFirst() == w.GetFirst())) return false;
    if ((!second_wild) && !(GetSecond() == w.GetSecond())) return false;
    if (!vbl_basic_optional_traits<T3>::IsOptional)
      if ((!third_wild) && !(GetThird() == w.GetThird())) return false;
    if (!vbl_basic_optional_traits<T4>::IsOptional)
      if ((!fourth_wild) && !(GetFourth() == w.GetFourth())) return false;
    if (!vbl_basic_optional_traits<T5>::IsOptional)
      if ((!fifth_wild) && !(GetFifth() == w.GetFifth())) return false;
    return true;
  }

  //: Test for match.
  inline bool match(const vbl_basic_tuple<T1,T2,T3,T4,T5>* w)
  {
    return match(*w);
  }

  //: Print the where clause on cout.
  inline void Print() { Dump(vcl_cout); }

  //: Print the where clause on cerr or stream.
  void Dump(vcl_ostream& str = vcl_cerr) const
  {
    str << "<";
    if (first_wild) str << "*";
    else str << GetFirst();
    if (second_wild) str << ",*";
    else str << "," << GetSecond();
    if (!vbl_basic_optional_traits<T3>::IsOptional)
      {
        if (third_wild) str << ",*";
        else str << "," << GetThird();
      }
    if (!vbl_basic_optional_traits<T4>::IsOptional)
      {
        if (fourth_wild) str << ",*";
        else str << "," << GetFourth();
      }
    if (!vbl_basic_optional_traits<T5>::IsOptional)
      {
        if (fifth_wild) str << ",*";
        else str << "," << GetFifth();
      }
    str << ">";
  }

  inline bool FirstWild() { return first_wild; }
  inline bool SecondWild() { return second_wild; }
  inline bool ThirdWild() { return third_wild; }
  inline bool FourthWild() { return fourth_wild; }
  inline bool FifthWild() { return fifth_wild; }

  inline void SetFirst(T1 t)
  {
    vbl_basic_tuple<T1,T2,T3,T4,T5>::SetFirst(t);
    first_wild = false;
  }

  inline void SetSecond(T2 t)
  {
    vbl_basic_tuple<T1,T2,T3,T4,T5>::SetSecond(t);
    second_wild = false;
  }

  inline void SetThird(T3 t)
  {
    vbl_basic_tuple<T1,T2,T3,T4,T5>::SetThird(t);
    third_wild = false;
  }

  inline void SetFourth(T4 t)
  {
    vbl_basic_tuple<T1,T2,T3,T4,T5>::SetFourth(t);
    fourth_wild = false;
  }

  inline void SetFifth(T5 t)
  {
    vbl_basic_tuple<T1,T2,T3,T4,T5>::SetFifth(t);
    fifth_wild = false;
  }

  //------------------------------------------------------------
  //: Return the primary key for the where clause.
  // The primary key is the number of the first non-wild attribute in the where
  // clause, with one being the first attribute.  If all attributes
  // are wild, zero is returned.
  int PrimaryKey() const
  {
    if (!first_wild) return 1;
    else if (!second_wild) return 2;
    else if (!third_wild) return 3;
    else if (!fourth_wild) return 4;
    else if (!fifth_wild) return 5;
    else return 0;
  }
};

#endif // vbl_basic_relation_where_h
