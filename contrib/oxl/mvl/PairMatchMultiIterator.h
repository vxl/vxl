// This is oxl/mvl/PairMatchMultiIterator.h
#ifndef PairMatchMultiIterator_h_
#define PairMatchMultiIterator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include "PairMatchMulti.h"
#include <vcl_cstdlib.h> // for vcl_abort()

class PairMatchMulti;
class PairMatchMultiIterator
{
  vcl_multimap_uint_uint::const_iterator _first;
  vcl_multimap_uint_uint::const_iterator _last;
 public:
//: Construct a PairMatchMultiIterator from two multimap iterators
  PairMatchMultiIterator(const vcl_multimap_uint_uint::iterator& first,
                         const vcl_multimap_uint_uint::iterator& last):
    _first(first),
    _last(last)
  {
  }

//: Construct a PairMatchMultiIterator which will scan all matches in a PairMatchMulti
  PairMatchMultiIterator(PairMatchMulti const& pmm):
    _first(pmm._matches12.begin()),
    _last(pmm._matches12.end())
  {
  }

//: Return true if the iterator is still valid.
  operator bool() { return _first != _last; }

//: Advance to the next match.
  PairMatchMultiIterator& operator ++ (/*prefix*/) { ++_first; return *this; }

//: Return the first component of the match pointed to by the iterator.
  int get_i1() const { return (*_first).first; }

//: Return the second component of the match pointed to by the iterator.
  int get_i2() const { return (*_first).second; }

 private:
  PairMatchMultiIterator& operator ++ (int /*postfix*/) {vcl_abort(); return *this;}
};

#endif // PairMatchMultiIterator_h_
