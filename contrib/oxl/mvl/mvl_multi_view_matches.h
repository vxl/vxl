// This is oxl/mvl/mvl_multi_view_matches.h
#ifndef mvl_multi_view_matches_h_
#define mvl_multi_view_matches_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <vcl_functional.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vcl_map.h>

class mvl_multi_view_matches
{
 public:
  typedef vcl_map<unsigned int, unsigned int, vcl_less<unsigned int> > Map;

  mvl_multi_view_matches(char const* filename);
  mvl_multi_view_matches(vcl_vector<int> const& views);
  mvl_multi_view_matches(int start, int end, int step = 1);
  mvl_multi_view_matches(int N);

  ~mvl_multi_view_matches();

  //: Set the view indices to which this match set will refer
  void set_views(vcl_vector<int> const& views);
  void set_views(int start, int end, int step = 1);
  void set_views(int N);

  //: Merge-in single match tracks. The view indices are *real* (i.e on-disk) indices.
  void add_pair(int view1, int corner1, int view2, int corner2);
  void add_triplet(int view1, int corner1, int view2, int corner2, int view3, int corner3);
  void add_track(vcl_vector<int> const& views, vcl_vector<int> const& corners);

  //: Merge-in a whole set of matches (possibly from a different set of views)
  void add_matches(mvl_multi_view_matches const& matches);

  int num_views() { return views_.size(); }
  int num_tracks() { return tracks_.size(); }

  //: Get the <frame,corner> map for track i (frame = track.first, corner = track.second)
  Map& get_track(int i) { return tracks_[i]; }
  //: Get the mapping from track frame to real view index (real_view_index = get_view_indices[track.first])
  vcl_vector<int>& get_view_indices () { return views_; }

  //: Standard I/O
  vcl_ostream& print(vcl_ostream&) const;
  vcl_istream& read(vcl_istream&);
  vcl_ostream& write(vcl_ostream&) const;

  //: Convenience
  void read(char const* file);
  void write(char const* file) const;

 protected:
  vcl_vector<int> views_;                 // maps internal frame index to real view indices
  Map view_to_internal_map_;              // maps real view indices to internal frame index
  vcl_vector<Map> tracks_;                // one map<internal_frame, corner> per track
  vcl_vector<Map> corner_to_track_maps_;  // one map<corner, track> per internal_frame

  void init();
  void update_maps(int track_index);      // iterate over tracks_[track_index]
                                          // and update the corner_to_track_maps_ to point to track_index
  void remove_maps(int track_index);      // iterate over tracks_[track_index] and remove the corner_to_track_maps_
};

inline vcl_ostream& operator<<(vcl_ostream& s, mvl_multi_view_matches const& v) { return v.print(s); }
inline vcl_istream& operator>>(vcl_istream& s, mvl_multi_view_matches& v) { return v.read(s); }

#endif // mvl_multi_view_matches_h_
