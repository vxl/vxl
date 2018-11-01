// This is oxl/mvl/TripleMatchSet.h
#ifndef TripleMatchSet_h_
#define TripleMatchSet_h_
//:
// \file
// \brief Store integer triplets
//
// A TripleMatchSet contains "triplets": 3-tuples of integers (i1, i2, i3),
// and methods to access and modify the set.  Access is fast only for
// "forward" matches of the form i1->i2->i3, but this has proved to be
// adequate for all of the triplet work so far.
//
// An iterator class is provided to allow iteration through all triplets
// in the set.
//
// TripleMatchSet storage is a pair of subclass-supplied PairMatchSets.
// Modifying the TripleMatchSet by adding or removing tuples will modify
// the underlying PairMatchSets.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// \verbatim
//  Modifications:
//   8 jun 97: PVr - removed std::vector<bool> instantiation (is in Templates/stl_bool.C)
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include <iosfwd>
#include <mvl/MatchSet.h>
#include <mvl/PairMatchSet.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class TripleMatchSet : public MatchSet
{
 protected:
  TripleMatchSet(PairMatchSet* match12, PairMatchSet* match23);
 public:
  TripleMatchSet(int i1_max, int i2_max, int i3_max);
  ~TripleMatchSet() override;

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

  void write_ascii(std::ostream& s) const;
  bool read_ascii(std::istream& s);

  void update_feature_match_data();
  int  size() const;
  bool get_match(int at, int* i1, int* i2, int* i3) const { return get_1(at, i1, i2, i3); }

  // -------------------------------------------------------
  class iterator
  {
   public:
//  iterator(bool full_only);
    iterator(const TripleMatchSet& ccc, bool full_only = true);
    iterator& operator=(const TripleMatchSet& ccc);
    //: Return the i1 of the pointed-to match
    int get_i1() const { return i1; }
    //: Return the i2 of the pointed-to match
    int get_i2() const { return i2; }
    //: Return the i3 of the pointed-to match
    int get_i3() const { return i3; }
    bool next();
    iterator& operator ++ (/*prefix*/) { next(); return *this; }
    bool isfull() const;
    operator bool () const;
   private:
    iterator operator ++ (int /*postfix*/) { std::abort(); return *this; }

   protected:
    const TripleMatchSet* c_;
    int match_index_;
    int i1, i2, i3;
    bool full_only_;
  };
  iterator begin() const { return iterator(*this, true); }

  // -------------------------------------------------------

 protected:
  PairMatchSet *match12_;
  PairMatchSet *match23_;
};

std::istream& operator >> (std::istream& s,  TripleMatchSet& ccc);
std::ostream& operator << (std::ostream& s, const TripleMatchSet& ccc);

#endif // TripleMatchSet_h_
