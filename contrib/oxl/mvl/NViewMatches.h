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
#ifndef NViewMatches_h_
#define NViewMatches_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : NViewMatches
//
// .SECTION Description
// 
//    A class to hold matches over multiple views, allowing for unmatched data.
//    With each 3d feature there is associated a multiple-view match. A
//    multiple-view match is a vector of integers (NViewMatch) which identify
//    2d features in each view.
//
//    If for example a 3d point X has NViewMatch "Xmatches", then
//    Xmatches[v] = i implies that the image of X in view "v" is the corner
//    with index "i" in view "v".
//    
//
// .NAME        NViewMatches - Multiple view matches with wildcards.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/NViewMatches.h
// .FILE        NViewMatches.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 May 97
// .SECTION Modifications:
//     270897 PRV Moved vcl_vector<NViewMatch> instantiation to Templates package
//     151097 AWF Added OffsetNViewMatch.
//     280498 David Capel made minimum match overlap user-definable,
//            allowed merging of consistent multiple-match tracks.
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_iosfwd.h>

//template <class T> class vbl_sparse_array_2d;

struct NViewMatch : public vnl_vector<int> {
  // Constants
  enum { nomatch = -1 };

  // Constructors
  NViewMatch() {}
  NViewMatch(int n): vnl_vector<int>(n, nomatch) {}
  
  // Operations
  bool matches(const NViewMatch& b, int min_overlap) const;
  void incorporate(const NViewMatch& b);
  bool is_consistent(const NViewMatch& b) const;
  int count_observations() const;
};
ostream& operator<<(ostream& s, const NViewMatch& c);

#ifdef VCL_GCC_27
inline bool operator==(const NViewMatch& a, const NViewMatch& b)
{
  return *(vnl_vector<int> const*)&a == *(vnl_vector<int> const*)&b;
}
#endif

class NViewMatches : public vcl_vector<NViewMatch> {
public:

  // Constructors/Destructors--------------------------------------------------
  NViewMatches();
  NViewMatches(istream& s);
  NViewMatches(const char* filename);
  NViewMatches(int nviews, int min_overlap = 2);
  ~NViewMatches();

  // NViewMatches(const NViewMatches& that); - use default
  // NViewMatches& operator=(const NViewMatches& that); - use default

  // Operations----------------------------------------------------------------
  int nviews() const { return _nviews; }
  
  bool load(istream&);
  bool load(const char* filename);

  bool save(ostream&);
  bool save(const char* filename);

  void clear();
  
  int count_matches(const NViewMatch& match);
  vcl_vector<int> get_matches(const NViewMatch& match);
  int incorporate_triplet(int base_view, int c1, int c2, int c3);
  int incorporate(const NViewMatch& matches);
  void remove_inconsistencies();
  NViewMatch make_triplet_match(int base_view, int c1, int c2, int c3);

  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  int _nviews;
  int _min_overlap;

  // Helpers-------------------------------------------------------------------
};

class OffsetNViewMatch : public NViewMatch {
  int min_view_;
public:
  OffsetNViewMatch(int min_view, int max_view):
    NViewMatch(max_view - min_view + 1),
    min_view_(min_view)
  {
  }

  OffsetNViewMatch(const OffsetNViewMatch& that):
    NViewMatch(that),
    min_view_(that.min_view_)
  {
  }

  OffsetNViewMatch& operator=(const OffsetNViewMatch& that)
  {
    NViewMatch::operator=(that);
    min_view_ = that.min_view_;
    return *this;
  }
  
  int& operator[] (int i) { return NViewMatch::operator[] (i - min_view_); }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS NViewMatches.

