// This is mul/mbl/mbl_stl_pred.h
#ifndef mbl_stl_pred_h_
#define mbl_stl_pred_h_
//:
// \file
// \brief Useful predicate functors for use in stl find_if,partition etc.
// \author martin roberts
// \date  July 2004


#include <vcl_functional.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Return true if a string contains a substring
// Note without this you'd need bind2nd mem_fun which can cause reference to reference compile errors
class mbl_stl_pred_str_contains : public vcl_unary_function<vcl_string, bool>
{
  //: The sought substring
  const vcl_string& substr_;
 public:
  mbl_stl_pred_str_contains(const vcl_string& substr): substr_(substr){}

  inline bool operator()(const vcl_string& str) const
  {
    return ( (str.find(substr_) != str.npos) ? true : false);
  }
};

//: Adapt a predicate over a vector to the operation specified on an index into that vector
// T is type of the vector, and Pred the boolean predicate to really be applied
template <class T, class Pred>
class mbl_stl_pred_index_adapter : public vcl_unary_function<unsigned, bool>
{
  //:const reference to vector used to store the objects indexed
  const vcl_vector<T >& vec_;
  //: The predicate to really be applied
  Pred Op_;
 public:
  mbl_stl_pred_index_adapter(vcl_vector<T> const& v, Pred Op):vec_(v),Op_(Op){}

  inline bool operator()(const unsigned& i) const
  {
    return Op_(vec_[i]);
  }
};

//: Helper function to create an index adapter of the appropriate type
// As this is a function not a class, it saves some template gobbledegook in the class name
// Vec is assumed vector<T> where T is the type associated with the constructed adapter
//However note that using this means an extra copy of the predicate functor will occur
template <class T, class Pred>
inline mbl_stl_pred_index_adapter<T,Pred> mbl_stl_pred_create_index_adapter(const vcl_vector<T>& v, Pred Op)
{
  return  mbl_stl_pred_index_adapter<T,Pred>(v,Op);
};


//Order a collection of iterators according to their dereferenced values
//NB assumes the value type supports operator<
//Can also be used for collections of pointers or objects supporting
//dereferencing operator like *p.
template <class Iter>
struct mbl_stl_pred_iter_deref_order : public vcl_binary_function<Iter,Iter, bool>
{
  inline bool  operator()(const Iter& iter1, const Iter& iter2 ) const
  {
    return (*iter1 < *iter2) ? true : false;
  }
};


//Order a collection of pair iterators according to their dereferenced keys
//NB assumes the key type supports operator<
template <class PairIter>
struct mbl_stl_pred_pair_iter_key_order : public vcl_binary_function<PairIter,PairIter, bool>
{
  inline bool  operator()(const PairIter& iter1, const PairIter& iter2 ) const
  {
    return (iter1->first < iter2->first) ? true : false;
  }
};

//Order a collection of pair iterators according to their dereferenced values
//NB assumes the key type supports operator<
template <class PairIter>
struct mbl_stl_pred_pair_iter_value_order : public vcl_binary_function<PairIter,PairIter, bool>
{
  inline bool  operator()(const PairIter& iter1, const PairIter& iter2 ) const
  {
    return (iter1->second < iter2->second) ? true : false;
  }
};

//
//////////////////////////////////////////////////////////////////////////
//Order a collection of pairs
//First is the primary key, second is the secondary key
//NB assumes both the pair types supports operator<
template <class T1, class T2>
struct mbl_stl_pred_pair_order : public vcl_binary_function<vcl_pair<T1,T2>,vcl_pair<T1,T2>, bool>
{
  inline bool  operator()(const vcl_pair<T1,T2>& pair1, const vcl_pair<T1,T2>& pair2 ) const
  {
      if(pair1.first < pair2.first)
          return true;
      else if(pair1.first > pair2.first)
          return false;
      else
          return pair1.second < pair2.second; //Primaries are equal so order on secondary
  }
};

//See if a test pointer is the class type required
//Note the template type T would normally be of pointer type but might also be
//something supporting operator->() in a pointer like way 
//(e.g. auto_ptr mbl_cloneable_ptr etc) 
template <class T>
//NB assumes templated class provides is_a to return its typename
class mbl_stl_pred_is_a : public vcl_unary_function<T, bool>
{
  //:const reference to name of required class type
  const vcl_string& ctype_;
 public:
  mbl_stl_pred_is_a(vcl_string const& ctype):ctype_(ctype){}

  inline bool operator()(const T& p) const
  {
      return (p->is_a()==ctype_) ? true : false;
  }
};

#endif
