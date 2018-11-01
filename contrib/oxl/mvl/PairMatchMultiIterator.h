// This is oxl/mvl/PairMatchMultiIterator.h
#ifndef PairMatchMultiIterator_h_
#define PairMatchMultiIterator_h_
//:
//  \file
// \brief Iterator for PairMatchMulti
//
//    PairMatchMultiIterator is a helper class that iterates through
//    a set of matches in a PairMatchMulti.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Sep 96
//-----------------------------------------------------------------------------

#include <functional>
#include <map>
#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class PairMatchMulti;

// conceptually a list of tuples (index1, index2, strength)
class vcl_multimap_uint_uint : public std::multimap<unsigned,unsigned, std::less<unsigned> >
{
  typedef std::multimap<unsigned, unsigned, std::less<unsigned> > base;
 public:
  iterator insert(unsigned int key, unsigned int value);
  void clear();
};

class PairMatchMultiIterator
{

  vcl_multimap_uint_uint::const_iterator first_;
  vcl_multimap_uint_uint::const_iterator last_;
 public:
//: Construct a PairMatchMultiIterator from two multimap iterators
  PairMatchMultiIterator(const vcl_multimap_uint_uint::iterator& first,
                         const vcl_multimap_uint_uint::iterator& last):
    first_(first),
    last_(last)
  {
  }

  //: Construct a PairMatchMultiIterator which will scan all matches in a PairMatchMulti
  PairMatchMultiIterator(PairMatchMulti const& pmm);

  //: Return true if the iterator is still valid.
  explicit operator bool () const
    { return (first_ != last_)? true : false; }

  //: Return false if the iterator is still valid.
  bool operator!() const
    { return (first_ != last_)? false : true; }

  //: Advance to the next match.
  PairMatchMultiIterator& operator ++ (/*prefix*/) { ++first_; return *this; }

  //: Return the first component of the match pointed to by the iterator.
  int get_i1() const { return (*first_).first; }

  //: Return the second component of the match pointed to by the iterator.
  int get_i2() const { return (*first_).second; }

 private:
  PairMatchMultiIterator operator++ (int /*postfix*/) { std::abort(); return *this; }
};

#endif // PairMatchMultiIterator_h_
