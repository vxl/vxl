#ifndef PairMatchMultiIterator_h_
#define PairMatchMultiIterator_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : PairMatchMultiIterator
//
// .SECTION Description
//    PairMatchMultiIterator is a helper class that iterates through
//    a set of matches in a PairMatchMulti.
//
// .NAME        PairMatchMultiIterator - Iterator for PairMatchMulti
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/PairMatchMultiIterator.h
// .FILE        PairMatchMultiIterator.h
// .FILE        PairMatchMultiIterator.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Sep 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include "PairMatchMulti.h"

class PairMatchMulti;
class PairMatchMultiIterator {
  vcl_multimap_uint_uint::const_iterator _first;
  vcl_multimap_uint_uint::const_iterator _last;
public:
// -- Construct a PairMatchMultiIterator from two multimap iterators
  PairMatchMultiIterator(const vcl_multimap_uint_uint::iterator& first,
			 const vcl_multimap_uint_uint::iterator& last):
    _first(first),
    _last(last)
  {
  }
    
// -- Construct a PairMatchMultiIterator which will scan all matches in a PairMatchMulti
  PairMatchMultiIterator(PairMatchMulti const& pmm):
    _first(pmm._matches12.begin()),
    _last(pmm._matches12.end())
  {
  }
    
// -- Return true if the iterator is still valid.
  operator bool() { return _first != _last; }

// -- Advance to the next match.
  PairMatchMultiIterator& operator ++ (/*prefix*/) { ++_first; return *this; }

// -- Return the first component of the match pointed to by the iterator.
  int get_i1() const { return (*_first).first; }

// -- Return the second component of the match pointed to by the iterator.
  int get_i2() const { return (*_first).second; }

private:
  PairMatchMultiIterator& operator ++ (int /*postfix*/) {abort(); return *this;}
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS PairMatchMultiIterator.
