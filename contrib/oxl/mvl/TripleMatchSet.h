// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifndef TripleMatchSet_h_
#define TripleMatchSet_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        TripleMatchSet - Store integer triplets
// .LIBRARY	MViewBasics
// .HEADER	MultiView package
// .INCLUDE	mvl/TripleMatchSet.h
// .FILE        TripleMatchSet.h
// .FILE        TripleMatchSet.C
// .EXAMPLE     ../Examples/exampleTripleMatchSet.C
//
// .SECTION Description
//    A TripleMatchSet contains "triplets": 3-tuples of integers (i1, i2, i3),
//    and methods to access and modify the set.    Access is fast only for
//    "forward" matches of the form i1->i2->i3, but this has proved to be
//    adequate for all of the triplet work so far.
//
//    An iterator class is provided to allow iteration through all triplets
//    in the set.
//
//    TripleMatchSet storage is a pair of subclass-supplied PairMatchSets.
//    Modifying the TripleMatchSet by adding or removing tuples will modify
//    the underlying PairMatchSets.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
//-----------------------------------------------------------------------------

#include <mvl/MatchSet.h>
#include <mvl/PairMatchSet.h>

class TripleMatchSet : public MatchSet {
protected:
  TripleMatchSet(PairMatchSet* match12, PairMatchSet* match23);
public:
  TripleMatchSet(int i1_max, int i2_max, int i3_max);
  ~TripleMatchSet();
  
  void clear_matches();
  void clear_nontriplets();
  int  count() const;

  bool add_match(int i, int ii, int iii);
  bool delete_match(int i1, int i2, int i3);
  bool contains(int i1, int i2, int i3);
  
  bool get_1(int i1, int* i1out, int* i2out, int* i3out) const;
  bool get_2(int i2, int* i1out, int* i2out, int* i3out) const;
  bool get_3(int i3, int* i1out, int* i2out, int* i3out) const;

  int get_match_12(int i1) const;
  int get_match_23(int i2) const;
  int get_match_31(int i3) const;
  int get_match_21(int i2) const;
  int get_match_32(int i3) const;
  int get_match_13(int i1) const;

  int get_match_123(int i1, int i2) const;

  void set(PairMatchSet* match12, PairMatchSet* match23);
  void set_from_pairwise_matches(const PairMatchSet& matches12, const PairMatchSet& matches23);

  void write_ascii(ostream& s) const;
  bool read_ascii(istream& s);
  
  void update_feature_match_data();
  int  size() const;
  bool get_match(int at, int* i1, int* i2, int* i3) const { return get_1(at, i1, i2, i3); }

  // -------------------------------------------------------
  class iterator {
  public:
//  iterator(bool full_only);
    iterator(const TripleMatchSet& ccc, bool full_only = true);
    iterator& operator=(const TripleMatchSet& ccc);
    int get_i1() const { return i1; }
    int get_i2() const { return i2; }
    int get_i3() const { return i3; }
    bool next();
    iterator& operator ++ (/*prefix*/) { next(); return *this; }
    bool isfull() const;
    operator bool () const;
  private:
    iterator& operator ++ (int /*postfix*/) { abort(); return *this; } 

  protected:
    const TripleMatchSet* _c;
    int _match_index;
    int i1, i2, i3;
    bool _full_only;
  };
  iterator begin() const { return iterator(*this, true); }
  
  // -------------------------------------------------------

protected:
  PairMatchSet *_match12;
  PairMatchSet *_match23;
};
istream& operator >> (istream& s,  TripleMatchSet& ccc);
ostream& operator << (ostream& s, const TripleMatchSet& ccc);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS TripleMatchSet.
