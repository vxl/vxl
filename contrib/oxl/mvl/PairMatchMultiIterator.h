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

#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_cstdlib.h> // for vcl_abort()
class PairMatchMulti;

// conceptually a list of tuples (index1, index2, strength)
class vcl_multimap_uint_uint : public vcl_multimap<unsigned,unsigned, vcl_less<unsigned> >
{
  typedef vcl_multimap<unsigned, unsigned, vcl_less<unsigned> > base;
 public:
  iterator insert(unsigned int key, unsigned int value);
  void clear();
};

class PairMatchMultiIterator
{
  //: Helper types for safe boolean conversion.
  struct safe_bool_dummy { void dummy() {} };
  typedef void (safe_bool_dummy::* safe_bool)();

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
  operator safe_bool () const
    { return (first_ != last_)? &safe_bool_dummy::dummy : 0; }

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
  PairMatchMultiIterator& operator ++ (int /*postfix*/) {vcl_abort(); return *this;}
};

#endif // PairMatchMultiIterator_h_
