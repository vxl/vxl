// This is oxl/mvl/TripleMatchSet.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include "TripleMatchSet.h"
#include <mvl/PairMatchSet.h>

//: Initialize a TripleMatchSet from a pair of PairMatchSets.
// The PairMatchSets are adopted by the TripleMatchSet, which will
// delete them.
TripleMatchSet::TripleMatchSet(PairMatchSet* match12, PairMatchSet* match23)
{
  match12_ = match12;
  match23_ = match23;
}

//: Initialize a TripleMatchSet, by specifying the maximum i1, i2, i3 values.
// Keep these conservative, as arrays of that length will be made.  Currently
// the i3 value is ignored.
TripleMatchSet::TripleMatchSet(int i1_max, int i2_max, int)
{
  match12_ = new PairMatchSet(i1_max);
  match23_ = new PairMatchSet(i2_max);
}

//: Delete the PairMatchSets
TripleMatchSet::~TripleMatchSet()
{
  delete match12_;
  delete match23_;
}

//: Destroy current PairMatchSets and adopt two new ones.
void TripleMatchSet::set(PairMatchSet* match12, PairMatchSet* match23)
{
  delete match12_;
  delete match23_;
  match12_ = match12;
  match23_ = match23;
}

//: Join two PairMatchSets on their 2nd and 1st columns respectively.
// I.e. make the TripleMatchSet which contains (i1,i2,i3) iff
//
// matches12.contains(i1, i2) and matches23.contains(i2, i3)
//
void TripleMatchSet::set_from_pairwise_matches(const PairMatchSet& matches12,
                                               const PairMatchSet& matches23)
{
  clear_matches();
  for (PairMatchSet::iterator p12 = matches12; p12; p12.next()) {
    int i3 = matches23.get_match_12(p12.get_i2());
    if (matchp(i3))
      add_match(p12.get_i1(), p12.get_i2(), i3);
  }
  vcl_cerr << "TripleMatchSet: " << count() << " triplet matches.\n";
}

//: Write as three ascii columns.
void TripleMatchSet::write_ascii(vcl_ostream & s) const
{
  for (iterator match = begin(); match; ++match) {
    s << match.get_i1() << '\t'
      << match.get_i2() << '\t'
      << match.get_i3() << '\n';
  }
}

//: Read from ascii vcl_istream
bool TripleMatchSet::read_ascii(vcl_istream& s)
{
  vnl_matrix<double> m;
  s >> m;
  if (!s.good() && !s.eof())
    return false;

  if (m.columns() != 3) {
    vcl_cerr << "TripleMatchSet::read_ascii(): cols = " << m.columns() << ", not 3\n";
    return false;
  }

  int n = m.rows();

  int i1_max = 0;
  int i2_max = 0;
  for (int i = 0; i < n; ++i) {
    if (m(i,0) > i1_max) i1_max = (int)m(i,0);
    if (m(i,1) > i2_max) i2_max = (int)m(i,2);
  }

  clear_matches();
  // set(new PairMatchSet(i1_max+1), new PairMatchSet(i2_max+1));

  {
    for (int i = 0; i < n; ++i)
      add_match(int(m(i,0)), int(m(i,1)), int(m(i,2)));
  }

  return true;
}

//: Write to vcl_ostream with header
vcl_ostream& operator << (vcl_ostream& s, const TripleMatchSet& matches)
{
  s << "# TripleMatchSet: " << matches.count() << " triplet matches.\n";
  matches.write_ascii(s);
  return s;
}

vcl_istream& operator >> (vcl_istream& s, TripleMatchSet& matches)
{
  matches.read_ascii(s);
  return s;
}

// == TRIPLET ACCESS ==

//: Add triplet (i1, i2, i3) to the matchset.
//  Any existing matches of the form (i1, *, *) are removed. O(1).
bool TripleMatchSet::add_match(int i1, int i2, int i3)
{
  if (get_match_23(i2) != MatchSet::NoMatch) {
    vcl_cerr << "TripleMatchSet::add_match(" <<i1<< ", "<<i2<<", "<<i3<<")\n";
    int old_i1 = get_match_21(i2);
    int old_i3 = get_match_23(i2);
    vcl_cerr<<"*** i2 is already in a match ("<<old_i1<<"/"<<i2<<"/"<<old_i3<<")\n";
  }

  if (get_match_12(i1) != MatchSet::NoMatch) {
    vcl_cerr<<"TripleMatchSet::add_match("<<i1<<", "<<i2<<", "<<i3<<")\n";
    int old_i2 = get_match_12(i1);
    int old_i3 = get_match_23(old_i2);
    vcl_cerr<<"*** i1 is already in a match ("<<i1<<"/"<<old_i2<<"/"<<old_i3<<")\n";
  }

  return match12_->add_match(i1, i2) && match23_->add_match(i2, i3);
}

//: Return the number of triplets. O(n).
int TripleMatchSet::count() const
{
  int c = 0;
  for (iterator match = begin(); match; match.next())
    ++c;
  return c;
}

//: Remove all tuples.
void TripleMatchSet::clear_matches()
{
  match12_->clear();
  match23_->clear();
}

//-----------------------------------------------------------------------------
//: Select(1 = i1).2, meaning take the 2nd component of the tuples in which the first component equals i1.
// Complexity O(1).
int TripleMatchSet::get_match_12(int i1) const
{
  return match12_->get_match_12(i1);
}

//: Select(1 = i1).3
// Complexity O(1)
int TripleMatchSet::get_match_13(int i1) const
{
  return get_match_23(get_match_12(i1));
}

//: Select(2 = i2).3 Complexity O(1)
int TripleMatchSet::get_match_23(int i2) const
{
  return match23_->get_match_12(i2);
}

//: Select(2 = i2).1 Complexity O(n)
int TripleMatchSet::get_match_21(int i2) const
{
  return match12_->get_match_21(i2);
}

//: Select(3 = i3).1 Complexity O(n)
int TripleMatchSet::get_match_31(int i3) const
{
  return get_match_21(get_match_32(i3));
}

//: Select(3 = i3).2 Complexity O(n)
int TripleMatchSet::get_match_32(int i3) const
{
  return match23_->get_match_21(i3);
}

//: Select({1,2} = {i1, i2}).3
// Complexity O(1).
int TripleMatchSet::get_match_123(int i1, int i2) const
{
  int ii = match12_->get_match_12(i1);
  if (ii != i2)
    return MatchSet::NoMatch;
  else
    return get_match_23(i2);
}

//: Select(1 = c).{1,2,3}
//  Complexity O(1).  Returns true iff a match was found.
bool TripleMatchSet::get_1(int c, int* i1, int* i2, int* i3) const
{
  if (c >= match12_->size())
    return false;

  *i1 = c;
  *i2 = match12_->get_match_12(*i1);
  *i3 = match23_->get_match_12(*i2);

  return true;
}

//: Select(2 = c).{1,2,3}  Complexity O(n).
bool TripleMatchSet::get_2(int c, int* i1, int* i2, int* i3) const
{
  return get_1(get_match_21(c), i1, i2, i3);
}

//: Select(3 = c).{1,2,3}  Complexity O(n).
bool TripleMatchSet::get_3(int c, int* i1, int* i2, int* i3) const
{
  return get_1(get_match_31(c), i1, i2, i3);
}

// -----------------------------------------------------------------------------

// == DEVELOPER INFORMATION ==

// - An implementation detail.  If the underlying PairMatchSets
// have been modified, there may be i2-i3 matches that have no corresponding
// i1-i2. This routine clears them (noisily).
void TripleMatchSet::clear_nontriplets()
{
  //mt_clear_affinity_nontriplets (match12_->get_table(), match23_->get_table());
  vcl_vector<bool> accept(match23_->size());
  for (vcl_vector<bool>::iterator i=accept.begin(); i!=accept.end(); ++i)
    *i = false;

  int cleared_count = 0;
  for (int i1 = 0; i1 < match12_->size(); i1++) {
    int i2 = match12_->get_match_12(i1);
    if (i2 != NoMatch) {
      int i3 = NoMatch;
      if (i2 >= match23_->size() || i2 < 0)
        vcl_cerr << "TripleMatchSet::clear_nontriplets() -- bad i2 = " << i2 << vcl_endl;
      else
        i3 = match23_->get_match_12(i2);

      if (i3 == NoMatch) {
        match12_->add_match(i1, NoMatch);
        ++cleared_count;
      } else
        accept[i2] = true;
    }
  }
  vcl_cerr << "TripleMatchSet::clear_nontriplets() -- Cleared i2 " << cleared_count << vcl_endl;

  for (int i2 = 0; i2 < match23_->size(); i2++)
    if (!accept[i2]) {
      match23_->add_match(i2, NoMatch);
      ++cleared_count;
    }

  vcl_cerr << "TripleMatchSet::clear_nontriplets() -- Cleared i3 " << cleared_count << vcl_endl;
}

bool TripleMatchSet::delete_match(int i1, int i2, int i3)
{
  int old_12 = match12_->get_match_12(i1);
  if (old_12 != i2) {
    vcl_cerr << "TripleMatchSet::delete_match - old/new i2 = " << old_12 << "/" << i2 << vcl_endl;
    match23_->clear_match_1(old_12);
  }
  int old_23 = match23_->get_match_12(i2);
  if (old_23 != i3) {
    vcl_cerr << "TripleMatchSet::delete_match - old/new i3 = " << old_23 << "/" << i3 << vcl_endl;
  }

  match12_->clear_match_1(i1);
  match23_->clear_match_1(i2);

  return true;
}

// - Return the maximum allowed value of i1.
int TripleMatchSet::size() const
{
  return match12_->size();
}

// - Call PairMatchSets update_feature_match_data
void TripleMatchSet::update_feature_match_data()
{
  match12_->update_feature_match_data();
  match23_->update_feature_match_data();
}


// -----------------------------------------------------------------------------

#if 0
TripleMatchSet::iterator::iterator(bool)
{
  vcl_abort();
}
#endif

//: Construct an iterator that points to the first triplet of "ccc".
// The full_only flag is of interest only to developers.
TripleMatchSet::iterator::iterator(const TripleMatchSet& ccc, bool full_only):
  c_(&ccc),
  match_index_(0),
  full_only_(full_only)
{
  match_index_ = -1;
  next();
}

//: Advance to point to the next triplet.
bool TripleMatchSet::iterator::next()
{
  if (full_only_) {
    while (c_->get_match(++match_index_, &i1, &i2, &i3))
      if (isfull())
        return true;
    return false;
  }
  return c_->get_match(++match_index_, &i1, &i2, &i3);
}

//: Return true if there are still unseen matches
TripleMatchSet::iterator::operator bool () const
{
  return match_index_ < c_->size();
}

//: Return true if the current "pointed-to" match is a proper triplet.
// Should never return false in normal use.
bool TripleMatchSet::iterator::isfull() const
{
  return i1 != NoMatch && i2 != NoMatch && i3 != NoMatch;
}


#if 0
MA_MATCH_TABLE_STR* TripleMatchSet::make_matchtable()
{
  return mt_3image_affinity_to_match (match12_->get_table(), match23_->get_table());
}

MA_MATCH_TABLE_STR* TripleMatchSet::make_matchtable_12()
{
  //return mt_3image_affinity123_to_match23 (match12_->get_table(), match23_->get_table());
  // In fact this is used instead in vi_3image_match_feature:
  return mt_3image_affinity_to_match (match12_->get_table(), match23_->get_table());
}

MA_MATCH_TABLE_STR* TripleMatchSet::make_matchtable_23()
{
  return mt_3image_affinity123_to_match23 (match12_->get_table(), match23_->get_table());
}

void TripleMatchSet::update_from(MA_MATCH_TABLE_STR* matchtable)
{
  mt_3image_match_to_affinity (matchtable, match12_->get_table(), match23_->get_table());
}
#endif

