#ifndef NViewMatches_h_
#define NViewMatches_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \brief Multiple view matches with wildcards
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
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 May 97
// \verbatim
// Modifications:
//     970517 AWF Initial version.
//     270897 PRV Moved vcl_vector<NViewMatch> instantiation to Templates package
//     151097 AWF Added OffsetNViewMatch.
//     280498 David Capel made minimum match overlap user-definable,
//            allowed merging of consistent multiple-match tracks.
//     280299 AWF Changed disk format to use "-1" instead of "*" for easier
//            matlab interaction.
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

struct NViewMatch : public vnl_vector<int>
{
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

vcl_ostream& operator<<(vcl_ostream& s, const NViewMatch& c);

class NViewMatches : public vcl_vector<NViewMatch>
{
  // Data Members--------------------------------------------------------------
  int _nviews;
  int _min_overlap;

 public:
  // Constructors/Destructors--------------------------------------------------
  NViewMatches();
  NViewMatches(vcl_istream& s);
  NViewMatches(const char* filename);
  NViewMatches(int nviews, int min_overlap = 2);
  ~NViewMatches();

  // NViewMatches(const NViewMatches& that); - use default
  // NViewMatches& operator=(const NViewMatches& that); - use default

  // Operations----------------------------------------------------------------
  int nviews() const { return _nviews; }

  bool load(vcl_istream&);
  bool load(const char* filename);

  bool save(vcl_ostream&);
  bool save(const char* filename);

  void clear();

  int count_matches(const NViewMatch& match);
  vcl_vector<int> get_matches(const NViewMatch& match);
  int incorporate_triplet(int base_view, int c1, int c2, int c3);
  int incorporate(const NViewMatch& matches);
  void remove_inconsistencies();
  NViewMatch make_triplet_match(int base_view, int c1, int c2, int c3);
};

class OffsetNViewMatch : public NViewMatch
{
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

#endif // NViewMatches_h_
