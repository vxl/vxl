// This is oxl/mvl/PairMatchMulti.h
#ifndef PairMatchMulti_h_
#define PairMatchMulti_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Set of pairs of integers
//
//    PairMatchMulti is a binary relationship between integers
//    $i_1 \rightarrow i_2$ where there may be multiple $i_2$ for
//    each $i_1$.
//
//    The current implementation uses an STL multimap.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 16 Sep 96
//
// \verbatim
//  Modifications:
//    June 97 - Peter Vanroose - operator==() added
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_utility.h> //vxl_filter
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_iosfwd.h>

// conceptually a list of tuples (index1, index2, strength)
class vcl_multimap_uint_uint : public vcl_multimap<unsigned,unsigned, vcl_less<unsigned> >
{
  typedef vcl_multimap<unsigned, unsigned, vcl_less<unsigned> > base;
 public:
  iterator insert(unsigned int key, unsigned int value);
  void clear();
};

template <class T> class vbl_sparse_array_2d;

class PairMatchSet;

class PairMatchMulti
{
 public:
  friend class PairMatchMultiIterator;

  // Constructors/Destructors--------------------------------------------------

  PairMatchMulti();
  PairMatchMulti(vcl_istream& s);
  PairMatchMulti(const PairMatchMulti& that);
 ~PairMatchMulti();

  PairMatchMulti& operator=(const PairMatchMulti& that);

  // Operations----------------------------------------------------------------

  //: Add a match $(i_1, i_2)$ to the set
  void add_match(int i1, int i2) {
    _matches12.insert(i1, i2);
  }

  bool contains(int i1, int i2) const;

  //: Add a match $(i_1, i_2)$ to the set
  void add_match(int i1, int i2, double score);

  double get_score(int i1, int i2) const;
  void set_score(int i1, int i2, double score);

  //: Clear all matches
  void clear() {
    _matches12.erase(_matches12.begin(), _matches12.end());
  }

  int size() const { return _matches12.size(); }

  // for compatibility with PairMatchSet
  int count() const { return _matches12.size(); }

  unsigned count_matches_12(int i1);

  PairMatchMultiIterator get_match_12(int i1);
  PairMatchMultiIterator get_match_21(int i1);

  inline PairMatchMultiIterator iter();

  // Computations--------------------------------------------------------------
  bool is_superset(PairMatchSet& unique_set);

  // Data Access---------------------------------------------------------------
  bool operator==(PairMatchMulti const& that) const {
    if (this == &that) return true;
    return false; // always false, except when identical memory location
  }

  bool load(char const* filename);
  bool read_ascii(vcl_istream& s);

 protected:
  // Data Members--------------------------------------------------------------
  vcl_multimap_uint_uint _matches12;
  vbl_sparse_array_2d<double> *_scores;
};

#include "PairMatchMultiIterator.h"

//: Return an iterator which will run through the list of matches for feature index i1.
//  Example usage: to print all matches for "target"
// <verb>
//   for (PairMatchMultiIterator p = mm.get_match_12(target); !p.done(); p.next())
//     vcl_cout << p.get_i1() << " " << p.get_i2() << vcl_endl;
// </verb>
// Complexity is O(log n).
inline PairMatchMultiIterator PairMatchMulti::get_match_12(int i1)
{
  return PairMatchMultiIterator(_matches12.lower_bound(i1), _matches12.upper_bound(i1));
}

//:
// This may be expensive.  If it is used a lot, it may be worth maintaining
// forward and backward maps.  Right now it's not even implemented.
inline PairMatchMultiIterator PairMatchMulti::get_match_21(int) { vcl_abort(); return iter(); }

//: Return the number of matches for i1.
inline
unsigned PairMatchMulti::count_matches_12(int i1)
{
  return _matches12.count(i1);
}

//: Return an iterator that traverses the entire match set
inline
PairMatchMultiIterator PairMatchMulti::iter()
{
  return PairMatchMultiIterator(_matches12.begin(), _matches12.end());
}

vcl_ostream& operator << (vcl_ostream&, const PairMatchMulti&);
vcl_istream& operator >> (vcl_istream&, PairMatchMulti&);

#endif // PairMatchMulti_h_
