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
#ifdef __GNUC__
#pragma implementation "NViewMatches.h"
#endif
//
// Class: NViewMatches
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 17 May 97
// Modifications:
//   970517 AWF Initial version.
//   280498 David Capel made minimum match overlap user-definable,
//          allowed merging of consistent multiple-match tracks,
//          and added index arrays to make match insertion O(1)
//   280299 AWF Changed disk format to use "-1" instead of "*" for easier
//          matlab interaction.
//-----------------------------------------------------------------------------

#include "NViewMatches.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_fstream.h>
#include <vbl/vbl_awk.h>
#include <vbl/vbl_printf.h>

//#include <vbl/vbl_sparse_array_2d.h>

/////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& s, const NViewMatch& c)
{
  for(int i = 0; i < c.size(); ++i)
    vbl_printf(s, "%-4d ", c[i]);
  
  return s;
}

// -- Return true if B is consistent with this match.  Matches are consistent if
// they are identical in all non-wildcard positions.  However, it additionally
// requires that they share at least min_overlap non-wildcard positions.
bool NViewMatch::matches(const NViewMatch& b, int min_overlap) const
{
  int l = size();

  if (l != b.size()) {
    cerr << "NViewMatch::matches(B): matching vectors of different lengths\n";
    return false;
  }
  
  int overlap = 0;
  for(int i = 0; i < l; ++i)
    if ((*this)[i] != NViewMatch::nomatch && b[i] != NViewMatch::nomatch) {
      if ((*this)[i] != b[i])
	return false;
      ++overlap;
    }
  return overlap >= min_overlap;
}

// -- Fill any wildcard positions with the corresponding position in B.
void NViewMatch::incorporate(const NViewMatch& b)
{
  int l = size();
  for(int i = 0; i < l; ++i)
    if ((*this)[i] == NViewMatch::nomatch)
      (*this)[i] = b[i];
}

// -- Check for any inconsistencies between matches
bool NViewMatch::is_consistent(const NViewMatch& b) const
{
  int l = size();
  for(int i = 0; i < l; ++i)
    if ((*this)[i] != NViewMatch::nomatch && b[i] != NViewMatch::nomatch)
      if ((*this)[i] != b[i])
	return false;
  return true;
}

// -- Count how many non-wildcard entries are in this NViewMatch
int NViewMatch::count_observations() const
{
  int l = size();
  int c = 0;
  for (int i = 0; i < l; ++i)
    if ((*this)[i] != NViewMatch::nomatch)
      ++c;
  return c;
}

/////////////////////////////////////////////////////////////////////////////

// Default ctor
NViewMatches::NViewMatches():
  _min_overlap(2)
  //,_indx_array(new vbl_sparse_array_2d<int>)
{
}

NViewMatches::NViewMatches(istream& s)
{
  load(s);
}

NViewMatches::NViewMatches(const char* filename)
{
  load(filename);
}

NViewMatches::NViewMatches(int nviews, int min_overlap):
  _nviews(nviews), _min_overlap(min_overlap)
  //, _indx_array(new vbl_sparse_array_2d<int>)
{
}

NViewMatches::~NViewMatches() {
  //  delete _indx_array;
}

void NViewMatches::clear()
{
  resize(0);
}

bool NViewMatches::load(const char* filename)
{
  ifstream s(filename);
  if (!s.good()) {
    cerr << "NViewMatches::load(" << filename << ") - bad filename\n";
    return false;
  }
  return load(s);
}

bool NViewMatches::load(istream& s)
{
  clear();
  for(vbl_awk awk(s); awk; ++awk) {
    // On first line, set _nviews to field count
    // On subsequent lines, check the field count matches the first line.
    if (awk.NR() == 1)
      _nviews = awk.NF();
    else 
      if (awk.NF() != _nviews) {
	cerr << "NViewMatches::load() ERROR: only " << awk.NF() << " fields on line " << awk.NR() << endl;
	return false;
      }

    // Build NViewMatch from this line and add it.
    NViewMatch v(_nviews);
    for(int j = 0; j < _nviews; ++j) {
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

bool NViewMatches::save(ostream& s)
{
  for(int i = 0; i < size(); ++i)
    s << (*this)[i] << "\n";
  return s.good() != 0;
}

bool NViewMatches::save(const char* filename)
{
  ofstream o(filename);
  return save(o);
}

// -- Count how many matches are consistent with \argfont{match}
int NViewMatches::count_matches(const NViewMatch& match)
{
  int nmatches = 0;
  for(int i = 0; i < size(); ++i)
    if ((*this)[i].matches(match,_min_overlap))
      ++nmatches;
  return nmatches;
}

// -- Return an array of the indices that match the given match
vcl_vector<int> NViewMatches::get_matches(const NViewMatch& match)
{
  vcl_vector<int> ret;
  for(int i = 0; i < size(); ++i)
    if (operator[](i).matches(match,_min_overlap))
      ret.push_back(i);
  return ret;
}

// -- Add a new nview match to the set.   If it is just an update
// of an existing match (filling in a wildcard), merge it.  Otherwise
// add it at the end.
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
  iterator merged = end();
  abort(); // This routine is untested.....
  for(iterator i = begin(); i != end(); ++i) {
    if ((*i).matches(newtrack,_min_overlap)) {
      if (nmatches == 0) {
	// This is the first consistent match found for newtrack
	(*i).incorporate(newtrack);
	++nmatches;
	merged = i;
      } else {
	if ((*i).is_consistent(*merged)) {
	  cerr << "Merge : " << (*i) << endl;
	  cerr << "        " << (*merged) << endl;
	  // A further consistent match, so merge the two match tracks
	  (*merged).incorporate((*i));
	  erase(i);
	  --i; // step back so we don't skip any matches
	  ++nmatches;
	} else {
	  // Two matches are inconsistent so remove them both and return -1
	  //	  cerr << "NViewMatches::incorporate(): Doh! Found inconsistent matches - removing them" << endl;
	  erase(i);
	  erase(merged);
	  return -1;
	}
      }
    }
  }
  if (nmatches == 0) {
    push_back(newtrack);
    return size() - 1;
  }

//  if (nmatches > 1) {
//    cerr << "NViewMatches::incorporate(): " << nmatches << " consistent matches merged" << endl;
//  }

  return merged - begin();
}

// -- Build an NViewMatch from the triplet (base_view..base_view+2)
NViewMatch NViewMatches::make_triplet_match(int base_view, int c1, int c2, int c3)
{
  assert(base_view+2 < _nviews);
  NViewMatch newtrack(_nviews);
  newtrack[base_view] = c1;
  newtrack[base_view+1] = c2;
  newtrack[base_view+2] = c3;
  return newtrack;
}

// -- Build an NViewMatch from the triplet (base_view..base_view+2), and incorporate.
int NViewMatches::incorporate_triplet(int base_view, int c1, int c2, int c3)
{
  assert(base_view+2 < _nviews);
  NViewMatch newtrack(_nviews);
  newtrack[base_view] = c1;
  newtrack[base_view+1] = c2;
  newtrack[base_view+2] = c3;
  return incorporate(newtrack);
}

// Should ALWAYS be called before using the match matrix!
// Efficiently checks for inconsistencies and removes them.
void NViewMatches::remove_inconsistencies() {
}
