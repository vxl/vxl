// This is oxl/mvl/NViewMatches.cxx
//:
//  \file

#include <iostream>
#include <cstdlib>
#include <fstream>
#include "NViewMatches.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_awk.h>
#include <vul/vul_printf.h>

/////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& s, const NViewMatch& c)
{
  for (unsigned i = 0; i < c.size(); ++i)
    vul_printf(s, "%-4d ", c[i]);

  return s;
}

//: Return true if B is consistent with this match.
//  Matches are consistent if they are identical in all non-wildcard positions.
//  However, it additionally requires that they share at least min_overlap
//  non-wildcard positions.
bool NViewMatch::matches(const NViewMatch& b, int min_overlap) const
{
  unsigned l = size();

  if (l != b.size()) {
    std::cerr << "NViewMatch::matches(B): matching vectors of different lengths\n";
    return false;
  }

  int overlap = 0;
  for (unsigned i = 0; i < l; ++i)
    if ((*this)[i] != NViewMatch::nomatch && b[i] != NViewMatch::nomatch) {
      if ((*this)[i] != b[i])
        return false;
      ++overlap;
    }
  return overlap >= min_overlap;
}

//: Fill any wildcard positions with the corresponding position in B.
void NViewMatch::incorporate(const NViewMatch& b)
{
  unsigned l = size();
  for (unsigned i = 0; i < l; ++i)
    if ((*this)[i] == NViewMatch::nomatch)
      (*this)[i] = b[i];
}

//: Check for any inconsistencies between matches
bool NViewMatch::is_consistent(const NViewMatch& b) const
{
  unsigned l = size();
  for (unsigned i = 0; i < l; ++i)
    if ((*this)[i] != NViewMatch::nomatch && b[i] != NViewMatch::nomatch)
      if ((*this)[i] != b[i])
        return false;
  return true;
}

//: Count how many non-wildcard entries are in this NViewMatch
int NViewMatch::count_observations() const
{
  unsigned l = size();
  int c = 0;
  for (unsigned i = 0; i < l; ++i)
    if ((*this)[i] != NViewMatch::nomatch)
      ++c;
  return c;
}

/////////////////////////////////////////////////////////////////////////////

// Default ctor
NViewMatches::NViewMatches():
  min_overlap_(2)
{
}

NViewMatches::NViewMatches(std::istream& s)
{
  load(s);
}

NViewMatches::NViewMatches(const char* filename)
{
  load(filename);
}

NViewMatches::NViewMatches(int nviews, int min_overlap):
  nviews_(nviews), min_overlap_(min_overlap)
{
}

NViewMatches::~NViewMatches()
{
  //  delete indx_array_;
}

void NViewMatches::clear()
{
  resize(0);
}

bool NViewMatches::load(const char* filename)
{
  std::ifstream s(filename);
  if (!s.good()) {
    std::cerr << "NViewMatches::load(" << filename << ") - bad filename\n";
    return false;
  }
  return load(s);
}

bool NViewMatches::load(std::istream& s)
{
  clear();
  for (vul_awk awk(s); awk; ++awk) {
    // On first line, set nviews_ to field count
    // On subsequent lines, check the field count matches the first line.
    if (awk.NR() == 1)
      nviews_ = awk.NF();
    else
      if (awk.NF() != nviews_) {
        std::cerr << "NViewMatches::load() ERROR: only " << awk.NF() << " fields on line " << awk.NR() << '\n';
        return false;
      }

    // Build NViewMatch from this line and add it.
    NViewMatch v(nviews_);
    for (int j = 0; j < nviews_; ++j) {
      char const* cp = awk[j];
      if (cp[0] == '*')
        v[j] = NViewMatch::nomatch;
      else
        v[j] = atoi(cp);
    }
    push_back(v);
  }

  return true;
}

bool NViewMatches::save(std::ostream& s)
{
  for (unsigned i = 0; i < size(); ++i)
    s << (*this)[i] << '\n';
  return s.good() != 0;
}

bool NViewMatches::save(const char* filename)
{
  std::ofstream o(filename);
  return save(o);
}

//: Count how many matches are consistent with \p match
int NViewMatches::count_matches(const NViewMatch& match)
{
  int nmatches = 0;
  for (unsigned i = 0; i < size(); ++i)
    if ((*this)[i].matches(match,min_overlap_))
      ++nmatches;
  return nmatches;
}

//: Return an array of the indices that match the given \p match
std::vector<int> NViewMatches::get_matches(const NViewMatch& match)
{
  std::vector<int> ret;
  for (unsigned i = 0; i < size(); ++i)
    if (operator[](i).matches(match,min_overlap_))
      ret.push_back(i);
  return ret;
}

//: Add a new nview match to the set.
// If it is just an update of an existing match (filling in a wildcard),
// merge it.  Otherwise add it at the end.
//
// If the new match is consistent with two existing consistent matches
// then all 3 are merged.
//
// If the new match is consistent with two existing inconsistent matches
// they are both removed.
//
// Returns the index of the match into which the new match was merged, or
// the index of the new match if it was consistent with nothing.
// Returns -1 if inconsistency was found.

int NViewMatches::incorporate(const NViewMatch& newtrack)
{
  int nmatches = 0;
  auto merged = end();
  std::abort(); // This routine is untested.....
  for (auto i = begin(); i != end(); ++i) {
    if ((*i).matches(newtrack,min_overlap_)) {
      if (nmatches == 0) {
        // This is the first consistent match found for newtrack
        (*i).incorporate(newtrack);
        ++nmatches;
        merged = i;
      }
      else if ((*i).is_consistent(*merged)) {
        std::cerr << "Merge : " << (*i) << '\n'
                 << "        " << (*merged) << '\n';
        // A further consistent match, so merge the two match tracks
        (*merged).incorporate((*i));
        erase(i);
        --i; // step back so we don't skip any matches
        ++nmatches;
      }
      else {
        // Two matches are inconsistent so remove them both and return -1
        //  std::cerr << "NViewMatches::incorporate(): Doh! Found inconsistent matches - removing them\n";
        erase(i);
        erase(merged);
        return -1;
      }
    }
  }
  if (nmatches == 0) {
    push_back(newtrack);
    return size() - 1;
  }

#ifdef DEBUG
  if (nmatches > 1) {
    std::cerr << "NViewMatches::incorporate(): " << nmatches << " consistent matches merged\n";
  }
#endif
  return merged - begin();
}

//: Build an NViewMatch from the triplet \p (base_view..base_view+2)
NViewMatch NViewMatches::make_triplet_match(int base_view, int c1, int c2, int c3)
{
  assert(base_view+2 < nviews_);
  NViewMatch newtrack(nviews_);
  newtrack[base_view] = c1;
  newtrack[base_view+1] = c2;
  newtrack[base_view+2] = c3;
  return newtrack;
}

//: Build an NViewMatch from the triplet \p (base_view..base_view+2), and incorporate.
int NViewMatches::incorporate_triplet(int base_view, int c1, int c2, int c3)
{
  assert(base_view+2 < nviews_);
  NViewMatch newtrack(nviews_);
  newtrack[base_view] = c1;
  newtrack[base_view+1] = c2;
  newtrack[base_view+2] = c3;
  return incorporate(newtrack);
}

// Should ALWAYS be called before using the match matrix!
// Efficiently checks for inconsistencies and removes them.
void NViewMatches::remove_inconsistencies()
{
}
