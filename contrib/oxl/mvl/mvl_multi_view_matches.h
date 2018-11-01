// This is oxl/mvl/mvl_multi_view_matches.h
#ifndef mvl_multi_view_matches_h_
#define mvl_multi_view_matches_h_
//:
//  \file
// \brief Multiple view matches
//
//    A class to hold matches over multiple views, allowing for unmatched data.
//    With each 3d feature there is associated a multiple-view match. A
//    multiple-view match is a vector of integers which identify
//    2d features in each view.
//
//    If for example a 3d point X has NViewMatch "Xmatches", then
//    Xmatches[v] = i implies that the image of X in view "v" is the corner
//    with index "i" in view "v".
//
// \author
//     David Capel, Oxford RRG, 16 April 2000
//-----------------------------------------------------------------------------

#include <iostream>
#include <functional>
#include <vector>
#include <iosfwd>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class mvl_multi_view_matches
{
 public:
  typedef std::map<unsigned int, unsigned int, std::less<unsigned int> > Map;

  mvl_multi_view_matches(char const* filename);
  mvl_multi_view_matches(std::vector<int> const& views);
  mvl_multi_view_matches(int start, int end, int step = 1);
  mvl_multi_view_matches(int N);

  ~mvl_multi_view_matches();

  //: Set the view indices to which this match set will refer
  void set_views(std::vector<int> const& views);
  void set_views(int start, int end, int step = 1);
  void set_views(int N);

  //: Merge-in single match tracks. The view indices are \e real (i.e on-disk) indices.
  void add_pair(int view1, int corner1, int view2, int corner2);
  void add_triplet(int view1, int corner1, int view2, int corner2, int view3, int corner3);
  void add_track(std::vector<int> const& views, std::vector<int> const& corners);

  //: Merge-in a whole set of matches (possibly from a different set of views)
  void add_matches(mvl_multi_view_matches const& matches);

  int num_views() const { return views_.size(); }
  int num_tracks() const { return tracks_.size(); }

  //: Get the <frame,corner> map for track i (frame = track.first, corner = track.second)
  Map& get_track(int i) { return tracks_[i]; }
  //: Get the mapping from track frame to real view index (real_view_index = get_view_indices[track.first])
  std::vector<int>& get_view_indices () { return views_; }

  //: Standard I/O
  std::ostream& print(std::ostream&) const;
  std::istream& read(std::istream&);
  std::ostream& write(std::ostream&) const;

  //: Convenience
  void read(char const* file);
  void write(char const* file) const;

 protected:
  std::vector<int> views_;                 // maps internal frame index to real view indices
  Map view_to_internal_map_;              // maps real view indices to internal frame index
  std::vector<Map> tracks_;                // one map<internal_frame, corner> per track
  std::vector<Map> corner_to_track_maps_;  // one map<corner, track> per internal_frame

  void init();
  void update_maps(int track_index);      // iterate over tracks_[track_index]
                                          // and update the corner_to_track_maps_ to point to track_index
  void remove_maps(int track_index);      // iterate over tracks_[track_index] and remove the corner_to_track_maps_
};

inline std::ostream& operator<<(std::ostream& s, mvl_multi_view_matches const& v) { return v.print(s); }
inline std::istream& operator>>(std::istream& s, mvl_multi_view_matches& v) { return v.read(s); }

#endif // mvl_multi_view_matches_h_
