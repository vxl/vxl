// This is oxl/mvl/PairMatchMulti.h
#ifndef PairMatchMulti_h_
#define PairMatchMulti_h_
//:
// \file
// \brief Set of pairs of integers
//
//  PairMatchMulti is a binary relationship between integers
//  $i_1 \rightarrow i_2$ where there may be multiple $i_2$ for
//  each $i_1$.
//
//  The current implementation uses an STL multimap.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 16 Sep 96
//
// \verbatim
//  Modifications:
//   June 97 - Peter Vanroose - operator==() added
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "PairMatchMultiIterator.h"

template <class T> class vbl_sparse_array_2d;

class PairMatchSet;

class PairMatchMulti
{
  // Data Members--------------------------------------------------------------
  vcl_multimap_uint_uint matches12_;
  vbl_sparse_array_2d<double> *scores_;
 public:
  friend class PairMatchMultiIterator;

  // Constructors/Destructors--------------------------------------------------

  PairMatchMulti();
  PairMatchMulti(std::istream& s);
  PairMatchMulti(const PairMatchMulti& that);
 ~PairMatchMulti();

  PairMatchMulti& operator=(const PairMatchMulti& that);

  // Operations----------------------------------------------------------------

  //: Add a match $(i_1, i_2)$ to the set
  void add_match(int i1, int i2) {
    matches12_.insert(i1, i2);
  }

  bool contains(int i1, int i2) const;

  //: Add a match $(i_1, i_2)$ to the set
  void add_match(int i1, int i2, double score);

  double get_score(int i1, int i2) const;
  void set_score(int i1, int i2, double score);

  //: Clear all matches
  void clear() {
    matches12_.erase(matches12_.begin(), matches12_.end());
  }

  int size() const { return matches12_.size(); }

  // for compatibility with PairMatchSet
  int count() const { return matches12_.size(); }

  //: Return the number of matches for i1.
  unsigned count_matches_12(int i1) const { return matches12_.count(i1); }

  //: Return an iterator which will run through the list of matches for feature index i1.
  //  Example usage: to print all matches for "target"
  // \code
  //   for (PairMatchMultiIterator p = mm.get_match_12(target); !p.done(); p.next())
  //     std::cout << p.get_i1() << ' ' << p.get_i2() << std::endl;
  // \endcode
  // Complexity is O(log n).
  PairMatchMultiIterator get_match_12(int i1)
  {
    return PairMatchMultiIterator(matches12_.lower_bound(i1), matches12_.upper_bound(i1));
  }

  //: Return an iterator which will run through the list of matches for feature index i2.
  // This may be expensive.  If it is used a lot, it may be worth maintaining
  // forward and backward maps.  Right now it's not even implemented.
  PairMatchMultiIterator get_match_21(int/*i2*/) { std::abort(); return iter(); }

  //: Return an iterator that traverses the entire match set
  PairMatchMultiIterator iter() {
    return PairMatchMultiIterator(matches12_.begin(), matches12_.end());
  }

  // Computations--------------------------------------------------------------
  bool is_superset(PairMatchSet& unique_set);

  // Data Access---------------------------------------------------------------
  bool operator==(PairMatchMulti const& that) const {
    return this == &that; // always false, except when identical memory location
  }

  bool load(char const* filename);
  bool read_ascii(std::istream& s);
};

std::ostream& operator<< (std::ostream&, const PairMatchMulti&);
std::istream& operator>> (std::istream&, PairMatchMulti&);

#endif // PairMatchMulti_h_
