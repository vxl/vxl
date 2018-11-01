// This is oxl/mvl/mvl_multi_view_matches.cxx

#include <iostream>
#include <cstdlib>
#include <set>
#include <functional>
#include <fstream>
#include "mvl_multi_view_matches.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_awk.h>

mvl_multi_view_matches::mvl_multi_view_matches(char const* filename)
{
  read(filename);
}

mvl_multi_view_matches::mvl_multi_view_matches(std::vector<int> const& views)
{
  set_views(views);
}

mvl_multi_view_matches::mvl_multi_view_matches(int start, int end, int step)
{
  set_views(start,end,step);
}

mvl_multi_view_matches::mvl_multi_view_matches(int N)
{
  set_views(N);
}


mvl_multi_view_matches::~mvl_multi_view_matches() = default;

void mvl_multi_view_matches::set_views(std::vector<int> const& views)
{
  views_ = views;
  init();
}

void mvl_multi_view_matches::set_views(int start, int end, int step)
{
  views_.clear();
  for (int i=start; i <= end; i+=step)
    views_.push_back(i);
  init();
}

void mvl_multi_view_matches::set_views(int N)
{
  views_.clear();
  for (int i=0; i < N; ++i)
    views_.push_back(i);
  init();
}

void mvl_multi_view_matches::init()
{
  view_to_internal_map_.clear();
  for (unsigned i=0; i < views_.size(); ++i)
    view_to_internal_map_[views_[i]] = i;

  corner_to_track_maps_ = std::vector<Map> (views_.size());
  tracks_.clear();
}

void mvl_multi_view_matches::add_pair(int view1, int corner1, int view2, int corner2)
{
  std::vector<int> views(2);
  std::vector<int> corners(2);
  views[0] = view1;
  views[1] = view2;
  corners[0] = corner1;
  corners[1] = corner2;
  add_track(views, corners);
}

void mvl_multi_view_matches::add_triplet(int view1, int corner1, int view2, int corner2, int view3, int corner3)
{
  std::vector<int> views(3);
  std::vector<int> corners(3);
  views[0] = view1;
  views[1] = view2;
  views[2] = view3;
  corners[0] = corner1;
  corners[1] = corner2;
  corners[2] = corner3;
  add_track(views, corners);
}

void mvl_multi_view_matches::add_track(std::vector<int> const& views, std::vector<int> const& corners)
{
  assert(views.size() == corners.size());

  // gcc hack. It complains spuriously "`Map' is not an aggregate typedef" when
  // seeing this type on line 132. Making a typedef here gets around it. -- fsm
  typedef Map::iterator Map_iterator;

  int track_length = views.size();

  std::vector<int> internal_frames(track_length);

  // Map the given real views to our internal frame indices
  for (int i=0; i < track_length; ++i) {
    auto m = view_to_internal_map_.find(views[i]);
    if (m == view_to_internal_map_.end()) {
      std::cerr << __FILE__ << " : view specified outside range!\n";
      std::abort();
    }
    internal_frames[i] = (*m).second;  // holds the internal-frame index corresponding to given corner[i]
  }

  // Make a new track
  Map new_track;
  for (int i=0; i < track_length; ++i) {
    new_track[internal_frames[i]] = corners[i];
  }
  // Now see if this track shares any <frame,corner> with any existing tracks
  std::set<unsigned int, std::less<unsigned int> > friend_tracks;
  {
    for (int i=0; i < track_length; ++i) {
      auto m=corner_to_track_maps_[internal_frames[i]].find(corners[i]);
      if (m != corner_to_track_maps_[internal_frames[i]].end()) {
        if ((*m).second >= tracks_.size()) {
          std::cerr << __FILE__ << " : URK!" << internal_frames[i] << ' '
                   << corners[i] << ' ' << (*m).second << ' ' << tracks_.size() << std::endl;
          std::abort();
        }
        friend_tracks.insert((*m).second);
      }
    }
  }

  if (friend_tracks.empty()) {
    // No merging is necessary, so just add the brand new track to the back of the list
    tracks_.push_back(new_track);
    update_maps(tracks_.size() - 1);
  }
  else {
    // We have found one or more overlapping tracks, so try to merge them into the new track.
    // A set of tracks is consistent if they are identical in all non-wildcard (empty) positions
    bool consistency_okay = true;
    auto t=friend_tracks.begin();
    for ( ; t != friend_tracks.end() && consistency_okay; ++t) {
      Map& friend_track = tracks_[(*t)];
      // See if friend_track[t] is consistent with the new track
      for (auto i = new_track.begin(); i != new_track.end() && consistency_okay; ++i) {
        unsigned int frame = (*i).first;
        unsigned int corner = (*i).second;
        auto m = friend_track.find(frame);
        if (m != friend_track.end() && (*m).second != corner) consistency_okay = false;
      }
      if (consistency_okay) {
        // Okay, we're good to merge friend_track[t] into the new track
        for (auto & tp : friend_track)
          new_track.insert(tp);
      }
    }
    // All friend tracks are now merged into new track, or inconsistency has been found
    if (consistency_okay) {
      // The new track can now replace friend_track[0], and the other friend tracks can be shuffle-removed
      // by moving the last track into the vacated position in track list. We must use a reverse iterator here
      // just in case the last track is one of those which has just been merged into the new track.
      int merged_track_index = *(friend_tracks.begin());
      friend_tracks.erase(merged_track_index);
      tracks_[merged_track_index] = new_track;
      update_maps(merged_track_index);

      auto track_iterator = friend_tracks.rbegin();
      for ( ; !(track_iterator == friend_tracks.rend()); ++track_iterator) {
        unsigned int dead_track_index = (*track_iterator);
        if (dead_track_index+1 != tracks_.size()) {   // Don't try to shuffle the final track into itself
          tracks_[dead_track_index] = tracks_.back();
          update_maps(dead_track_index);
        }
        tracks_.pop_back();
      }
    }
    else {
      // URK! The tracks pass different corners in the same frame!
      // No choice, but to throw out the new track and all its friend_tracks.
      auto track_iterator = friend_tracks.rbegin();
      for ( ; !(track_iterator == friend_tracks.rend()); ++track_iterator) {
        unsigned int dead_track_index = (*track_iterator);
        remove_maps(dead_track_index);
        if (dead_track_index+1 != tracks_.size()) {   // Don't try to shuffle the final track into itself
          tracks_[dead_track_index] = tracks_.back();
          update_maps(dead_track_index);
        }
        tracks_.pop_back();
      }
    }
  }
}

void mvl_multi_view_matches::add_matches(mvl_multi_view_matches const& /*matches*/)
{
  std::cerr << __FILE__ ": mvl_multi_view_matches::add_matches() not implemented\n";
  std::abort();
}

void mvl_multi_view_matches::update_maps(int track_index)
{
  for (auto i = tracks_[track_index].begin(); i != tracks_[track_index].end(); ++i) {
    int internal_frame = (*i).first;
    int corner = (*i).second;
    corner_to_track_maps_[internal_frame][corner] = track_index;
  }
}

void mvl_multi_view_matches::remove_maps(int track_index)
{
  for (auto i = tracks_[track_index].begin(); i != tracks_[track_index].end(); ++i) {
    int internal_frame = (*i).first;
    int corner = (*i).second;
    corner_to_track_maps_[internal_frame].erase(corner);
  }
}

std::ostream& mvl_multi_view_matches::print(std::ostream& s) const
{
  for (unsigned int i=0; i < tracks_.size(); ++i) {
    s << "Track " << i << " : ";
    for (auto m : tracks_[i])
      s << '(' << views_[m.first] << ',' << m.second << ") ";
    s << std::endl;
  }
  return s;
}

std::istream& mvl_multi_view_matches::read(std::istream& s)
{
  if (!s.good()) return s;

  views_.clear();
  vul_awk awk(s);
  for (int i=0; i < awk.NF(); ++i)
    views_.push_back(atoi(awk[i]));
  ++awk;

  std::cerr << __FILE__ << " : reading views ( ";
  for (int view : views_)
    std::cerr << view << ' ';
  std::cerr << ")\n";

  init();

  tracks_.resize(20000);
  int max_track = 0;
  for (; awk; ++awk) {
    if (awk.NF() != 3) std::abort();
    int track = atoi(awk[0]);
    int frame = atoi(awk[1]);
    int corner = atoi(awk[2]);
    tracks_[track][frame] = corner;
    if (track > max_track) max_track = track;
  }
  tracks_.resize(max_track);

  for (unsigned int i=0; i < tracks_.size(); ++i)
    update_maps(i);

  std::cerr << __FILE__ << " : read " << tracks_.size() << " tracks\n";

  return s;
}

std::ostream& mvl_multi_view_matches::write(std::ostream& s) const
{
  if (!s.good()) return s;

  // Output the view indices on the first line
  for (unsigned int i=0; i < views_.size(); ++i)
    s << i << ' ';
  s << std::endl;

  // Now output the (track, internal frame, corner_index) triplets on each line
  for (unsigned int i=0; i < tracks_.size(); ++i)
    for (auto m = tracks_[i].begin(); m != tracks_[i].end(); ++m)
      s << i << ' ' << (*m).first << ' ' << (*m).second << std::endl;

  std::cerr << __FILE__ << " : wrote " << tracks_.size() << " tracks\n";

  return s;
}

void mvl_multi_view_matches::read(char const* filename)
{
  std::ifstream fin(filename);
  read(fin);
}

void mvl_multi_view_matches::write(char const* filename) const
{
  std::ofstream fout(filename);
  write(fout);
}
