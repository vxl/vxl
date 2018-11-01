// This is oxl/mvl/PairMatchSetMulti.h
#ifndef PairMatchSetMulti_h_
#define PairMatchSetMulti_h_
//:
// \file
// \brief Multimap of ints
//
//    PairMatchSetMulti is a list of tuples (i1, i2) which allows
//    efficient O(log n) indexing by I1, and O(n) by i2.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 16 Sep 96
//
//-----------------------------------------------------------------------------

class PairMatchSetMulti
{
 public:
  // Constructors/Destructors--------------------------------------------------

  PairMatchSetMulti();
  PairMatchSetMulti(const PairMatchSetMulti& that);
 ~PairMatchSetMulti();

  PairMatchSetMulti& operator=(const PairMatchSetMulti& that);

  // Operations----------------------------------------------------------------
  void add_match(int i1, int i2);
};

#endif // PairMatchSetMulti_h_
