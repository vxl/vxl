// This is oxl/mvl/PairMatchSet.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "PairMatchSet.h"

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>

#include <mvl/MatchSet.h>

PairMatchSet::iterator& PairMatchSet::iterator::operator ++ (int /*postfix*/)
{
  vcl_abort();
  return *this;
}

//: Construct a PairMatchSet for values of $i_1$ between 0 and size.
PairMatchSet::PairMatchSet(unsigned size):
  matches_(size)
{
  clear();
}

//: Destructor
PairMatchSet::~PairMatchSet()
{
}

// == OPERATIONS ==

PairMatchSet::PairMatchSet(const PairMatchSet& that):
  matches_(that.matches_),
  match_count_(that.match_count_)
{
}

PairMatchSet& PairMatchSet::operator=(const PairMatchSet& that)
{
  matches_ = that.matches_;
  match_count_ = that.match_count_;
  return *this;
}

void PairMatchSet::update_feature_match_data()
{
  assert(false);
}

//: Add the pair (i1, i2) to the match set.
// If i1 had an existing match it is overwritten.
bool PairMatchSet::add_match(int i1, int i2)
{
  if ((unsigned)i1 >= matches_.size()) {
    vcl_cerr << "PairMatchSet: add_match(" << i1 << ") greater than size " << matches_.size() << vcl_endl;
    vcl_abort();
    return false;
  }
  bool had_nomatch = (matches_[i1] == NoMatch);
  bool new_nomatch = (i2 == NoMatch);
  matches_[i1] = i2;
  if (had_nomatch && !new_nomatch)
    ++match_count_;
  return true;
}

//: Remove any match for i1.
//  Specifically, remove tuples whose first element is i1.
void PairMatchSet::clear_match_1(int i1)
{
  if ((unsigned)i1 >= matches_.size()) {
    vcl_cerr << "PairMatchSet: clear squawwk\n";
    return;
  }
  int& i2 = matches_[i1];
  if (i2 != MatchSet::NoMatch) {
    assert(match_count_ > 0);
    --match_count_;
  }

  i2 = MatchSet::NoMatch;
}

//: Return any match for i1.
//  Specifically, return the second element of any tuple whose first element is i1.
int PairMatchSet::get_match_12(int i1) const
{
  if (i1 == MatchSet::NoMatch)
    return MatchSet::NoMatch;

  if ((unsigned)i1 >= matches_.size()) {
    vcl_cerr << "PairMatchSet::get_match_12() -- i1 = " << i1 << " >= matches_.size() = " << matches_.size() << vcl_endl;
    vcl_abort();
    return -1;
  }

  return matches_[i1];
}

//: Return any match for i2.
//  Specifically, return the first element of any tuple whose second element is
//  i2.  This is currently O(n) in the number of matches, consider adding an
//  index to this class if you use it a lot.
int PairMatchSet::get_match_21(int i2) const
{
  for (unsigned i = 0; i < matches_.size(); ++i)
    if (matches_[i] == i2)
      return i;
  return MatchSet::NoMatch;
}

// - Allows selection of a given match.  Use the iterator instead.
bool PairMatchSet::get_match(int c, int* i1, int* i2) const
{
  if (c >= size())
    return false;

  *i1 = c;
  *i2 = get_match_12(*i1);

  return true;
}

//: Empty this match set.
void PairMatchSet::clear()
{
  for (unsigned i = 0; i < matches_.size(); ++i)
    matches_[i] = MatchSet::NoMatch;
  match_count_ = 0;
}

//: Set this match set to contain matches (i,i) for i=0..size
void PairMatchSet::set_identity()
{
  for (unsigned i = 0; i < matches_.size(); ++i)
    matches_[i] = i;
  match_count_ = matches_.size();
}

//: Count the number of matches in this set
int PairMatchSet::compute_match_count()
{
  match_count_ = 0;
  for (unsigned i = 0; i < matches_.size(); ++i)
    if (matches_[i] != MatchSet::NoMatch)
      ++match_count_;

  return match_count_;
}

//: Return the maximum allowed value of i1.
int PairMatchSet::size() const
{
  return matches_.size();
}

//: Set the maximum allowed value of i1 to size.
void PairMatchSet::set_size(unsigned newsize)
{
  if (newsize != matches_.size())
    matches_ = vcl_vector<int>(newsize, NoMatch);
}

void PairMatchSet::update(const vcl_vector<bool>& inliers)
{
  if (inliers.size() != count()) {
    vcl_cerr << "PairMatchSet::update() -- This matchset is not the same length as the inliers\n";
    vcl_abort();
  }

  int n = 0;
  for (PairMatchSet::iterator match (*this); match; match.next(), ++n)
    if (!inliers[n])
      clear_match_1(match.get_i1());
}

// Data Access---------------------------------------------------------------

// == INPUT/OUTPUT ==

//: Write matches in ASCII to stream.
void PairMatchSet::write_ascii(vcl_ostream& s) const
{
  for (unsigned i = 0; i < matches_.size(); ++i) {
    int to_index = matches_[i];
    if (to_index != NoMatch)
      s << i << " " << to_index << vcl_endl;
  }
}

vcl_ostream& operator<<(vcl_ostream& s, const PairMatchSet& cc) {
  cc.write_ascii(s);
  return s;
}

bool PairMatchSet::read_ascii(vcl_istream& s)
{
  clear();
  for (;;) {
    // Read 2 ints
    int i1, i2;
    s >> i1 >> i2;
    // Break if that failed
    if (!s.good())
      break;
    // Eat any whitespace
    s >> vcl_ws;

    // Sanity check
    if (i1 < 0 || i2 < 0 || i1 >= (int)matches_.size()) {
      vcl_cerr << "PairMatchSet::read_ascii -- Pair " << i1 << "-" << i2 << " is outside the valid range.\n";
      clear();
      return false;
    }

    // More sanity checking
    if (matches_[i1] != NoMatch) {
      vcl_cerr << "PairMatchSet::read_ascii() -- Warning:\n";
      vcl_cerr << "Duplicate matches for " << i1 << ": " << matches_[i1] << " and " << i2 << vcl_endl;
      return false;
    }

    matches_[i1] = i2;
  }

  return compute_match_count() > 0;
}

vcl_istream& operator>>(vcl_istream& s, PairMatchSet& cc) {
  cc.read_ascii(s);
  return s;
}


//: Summarize matches on stream
void PairMatchSet::print_brief(vcl_ostream& s) const
{
  s << "PairMatchSet: ";
  for (unsigned i = 0; i < matches_.size(); i++)
    s << matches_[i] << " ";
  s << vcl_endl;
}

//: Summarize matches on cout.
void PairMatchSet::print_brief() const
{
  unsigned n = matches_.size();
  char const *c = "";
  if (n > 30) {
    n = 30;
    c = "...";
  }

  for (unsigned i = 0; i < n; ++i)
    if (matches_[i] != NoMatch)
      vcl_cout << " " << matches_[i];
  vcl_cout << c << vcl_endl;
}

// Data Control--------------------------------------------------------------
// -----------------------------------------------------------------------------


// == ITERATOR [class PairMatchSet::iterator] ==

//: Construct an empty iterator.
PairMatchSet::iterator::iterator(bool full_only):
  c_(0),
  match_index_(0),
  full_only_(full_only)
{
}

//: Construct an iterator which will enumerate the matches in PairMatchSet cc.
PairMatchSet::iterator::iterator(const PairMatchSet& cc, bool full_only):
  c_(&cc),
  match_index_(0),
  full_only_(full_only)
{
  match_index_ = -1;
  next();
}

//: Set an iterator to enumerate the matches in PairMatchSet cc.
PairMatchSet::iterator& PairMatchSet::iterator::operator =(const PairMatchSet& cc)
{
  c_ = &cc;
  match_index_ = -1;
  next();
  return *this;
}

//: Advance the iterator, returning false if all matches have been enumerated.
bool PairMatchSet::iterator::next()
{
  if (full_only_) {
    while (c_->get_match(++match_index_, &i1, &i2))
      if (isfull())
        return true;
    return false;
  }
  return c_->get_match(++match_index_, &i1, &i2);
}

// insert these here for documentation purposes

#if 0
//: Return the first component of the match currently "pointed to" by the match iterator.
int PairMatchSet::iterator::get_i1() const
{
  return i1;
}

//: Return the second component of the match currently "pointed to" by the match iterator.
int PairMatchSet::iterator::get_i2() const
{
  return i2;
}
#endif

//: Return true if neither of the indices of the match pointed to by the iterator is NoMatch.
//  [Users should not need this]
bool PairMatchSet::iterator::isfull() const
{
  return i1 != NoMatch && i2 != NoMatch;
}

//: Return true if the iterator has not yet enumerated all matches.
PairMatchSet::iterator::operator PairMatchSet::iterator::safe_bool () const
{
  return (match_index_ < c_->size())? &safe_bool_dummy::dummy : 0;

}

//: Return false if the iterator has not yet enumerated all matches.
bool PairMatchSet::iterator::operator!() const
{
  return (match_index_ < c_->size())? false : true;
}
