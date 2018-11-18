// This is brl/bseg/bapl/bapl_connectivity.cxx
#include <iostream>
#include <algorithm>
#include <queue>
#include "bapl_connectivity.h"
//:
// \file

#include <bapl/bapl_keypoint_set.h>
#include <bapl/bapl_keypoint.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_lowe_keypoint.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: For sorting keypoint_match_sets
bool second_less( const bapl_keypoint_match_set_sptr& left_set, const bapl_keypoint_match_set_sptr& right_set)
{
  return left_set->id_right_ < right_set->id_right_;
}


//: add this match set symmetrically, i.e. into the list of img id 1 as well img id 2, while adding for img id 2, reverse the keypoint pairs
bool bapl_conn_table::add_sym(const bapl_keypoint_match_set_sptr& set)
{
  // first add match set directly for id1, id2:
  if (this->contains(set->id_left_, set->id_right_) || this->contains(set->id_right_, set->id_left_))
    return false;

  bapl_conn& conn = conns_[set->id_left_];
  auto p = lower_bound(conn.begin(), conn.end(), set, second_less);
  conn.insert(p, set);

  //: reverse the match set
  std::vector<bapl_key_match>& matches = set->matches_;
  std::vector<bapl_key_match> reversed_matches;
  for (auto & matche : matches) {
    bapl_key_match key(matche.second, matche.first);
    reversed_matches.push_back(key);
  }
  bapl_keypoint_match_set_sptr e = new bapl_keypoint_match_set(set->id_right_, set->id_left_, reversed_matches);
  bapl_conn& conn_other = conns_[set->id_right_];
  p = lower_bound(conn_other.begin(), conn_other.end(), e, second_less);
  conn_other.insert(p, e);
  return true;
}

//: add this match set
bool bapl_conn_table::add(const bapl_keypoint_match_set_sptr& set)
{
  // first add match set directly for id1, id2:
  if (this->contains(set->id_left_, set->id_right_))
    return false;

  bapl_conn& conn = conns_[set->id_left_];
  auto p = lower_bound(conn.begin(), conn.end(), set, second_less);
  conn.insert(p, set);
  return true;
}

//: make the table symmetric, only necessary if add() method is used as opposed to add_sym()
void bapl_conn_table::make_symmetric()
{
  for (auto & conn : conns_) {
    for (const auto& ms : conn) {
      //: reverse the match set
      std::vector<bapl_key_match>& matches = ms->matches_;
      std::vector<bapl_key_match> reversed_matches;
      for (auto & matche : matches) {
        bapl_key_match key(matche.second, matche.first);
        reversed_matches.push_back(key);
      }
      bapl_keypoint_match_set_sptr e = new bapl_keypoint_match_set(ms->id_right_, ms->id_left_, reversed_matches);
      this->add(e);
    }
  }
}


//: check if vector of image id1 already contains a match set for image id2.
//  conn is kept sorted with respect to second image ids, binary search to see if set->id_right_ exists
bool bapl_conn_table::contains(int id1, int id2)
{
  bapl_conn& conn = conns_[id1];
  std::vector<bapl_key_match> matches; // dummy vector
  bapl_keypoint_match_set_sptr e = new bapl_keypoint_match_set(id1, id2, matches);
  std::pair<bapl_conn::const_iterator, bapl_conn::const_iterator> p = equal_range(conn.begin(), conn.end(), e, second_less);
  return p.first != p.second; // true if not points to the end, false otherwise
}

void bapl_conn_table::print_table()
{
  for (const auto& conn : conns_) {
    int crnt_id = 0;
    for (auto & j : conn) {
      int id2 = j->id_right_;
      while (id2 > crnt_id) {
        std::cout << "0 ";
        crnt_id++;
      }
      std::cout << j->matches_.size() << ' ';
      crnt_id++;
    }
    std::cout << std::endl;
  }
}

void bapl_conn_table::print_table_with_matches()
{
  for (const auto& conn : conns_) {
    for (auto & j : conn) {
      std::cout << j->id_left_ << ' ' << j->id_right_ << '\n'
               << j->matches_.size() << '\n';
      for (unsigned k = 0; k < j->matches_.size(); k++) {
        std::cout << j->matches_[k].first->id() << ' ' << j->matches_[k].second->id() << '\n';
      }
      std::cout << '\n';
    }
  }
}

//: return the number of neighbors for image with id i
unsigned bapl_conn_table::get_number_of_neighbors(unsigned i)
{
  return conns_[i].size();
}

bool compare_first(const bapl_key_match &k1, const bapl_key_match &k2)
{
  return k1.first->id() < k2.first->id();
}

//: compute a set of tracks, each corresponding to a separate 3d point.
//  assumes a symmetric connectivity table
bool bapl_conn_table::compute_tracks(std::vector<bapl_track_data>& tracks, int  /*new_image_start*/)
{
  unsigned num_images = conns_.size();
  //: check if image data is set for each image
  for (unsigned i = 0; i < num_images; i++) {
    if (!img_data_[i]) {
      std::cout << "Data of img: " << i << " has not been set!\n";
      return false;
    }
  }

  // clear all marks for new images
  for (unsigned i = 0; i < num_images; i++) {
    // skip images with no neighbors
    int num_nbrs = (int)this->get_number_of_neighbors(i);

    if (num_nbrs == 0)
      continue;

    int num_features = img_data_[i]->keys_.size();
    img_data_key_flags_[i].resize(num_features);
  }

  int pt_idx = 0;

  // sort all match lists
  for (unsigned i = 0; i < num_images; i++) {
    bapl_conn& conn = conns_[i];
    for (auto & j : conn) {
      std::vector<bapl_key_match>& matches = j->matches_;
      std::sort(matches.begin(), matches.end(), compare_first);
    }
  }

  std::vector<bool> img_visited(num_images, false);
  std::vector<int> img_touched;
  img_touched.reserve(num_images);

  for (unsigned int i = 0; i < num_images; i++) {
    int num_features = img_data_[i]->keys_.size();

    // skip images with no neighbors
    int num_nbrs = (int) this->get_number_of_neighbors(i);

    if (num_nbrs == 0)
      continue;

    for (int j = 0; j < num_features; j++) {
      bapl_keypoint_sptr kp = img_data_[i]->keys_[j];
      bapl_image_key_vector features;
      std::queue<bapl_image_key> features_queue;

      if (img_data_key_flags_[i][j])
       continue; // already visited this feature

      int num_touched = img_touched.size();
      for (int k = 0; k < num_touched; k++)
        img_visited[img_touched[k]] = false;
      img_touched.clear();

      // breadth first search given this feature
      img_data_key_flags_[i][j] = true;

      features.push_back(bapl_image_key(i, kp));
      features_queue.push(bapl_image_key(i, kp));

      img_visited[i] = true;
      img_touched.push_back(i);

      int num_rounds = 0;
      while (!features_queue.empty())
      {
        num_rounds++;

        bapl_image_key feature = features_queue.front();
        features_queue.pop();

        int img_id = feature.first;

        bapl_keypoint_sptr dummy_sptr;
        bapl_key_match dummy(feature.second, dummy_sptr);

        //int start_idx = (img_id >= new_image_start) ?  new_image_start : 0;

        bapl_conn &nbrs = this->get_neighbors(img_id);

        bapl_conn::iterator iter;
        for (iter = nbrs.begin(); iter != nbrs.end(); iter++) {
          unsigned int k = (*iter)->id_right_;
          if (img_visited[k])
            continue;

          // binary search for the feature
          std::pair<std::vector<bapl_key_match>::iterator, std::vector<bapl_key_match>::iterator > p;
          p = equal_range((*iter)->matches_.begin(), (*iter)->matches_.end(), dummy, compare_first);

          if (p.first == p.second)  continue;
          assert((p.first)->first->id() == feature.second->id());
          int second_id = (p.first)->second->id();

          assert(second_id < (int)img_data_[k]->keys_.size());

          if (img_data_key_flags_[k][second_id])
            continue;

          img_data_key_flags_[k][second_id] = true;
          features.push_back(bapl_image_key(k, (p.first)->second));
          features_queue.push(bapl_image_key(k, (p.first)->second));

          img_visited[k] = true;
          img_touched.push_back(k);
        }
      }

      if (features.size() >= 2) {
        tracks.emplace_back(features);
        pt_idx++;
      }
    }
  } // for loop over images

  std::cout << "Found " << tracks.size() << " points!\n";
  assert(pt_idx == (int) tracks.size());

  return true;
}

//: Print tracks as correspondences in BWM_VIDEO_SITE format for visualization
void print_tracks(std::ostream& os, std::vector<bapl_track_data>& tracks, int img_width, int img_height)
{
  os << "<BWM_VIDEO_SITE name=\"\">\n"
     << "<videoPath path=\"\">\n</videoPath>\n"
     << "<cameraPath path=\"\">\n</cameraPath>\n"
     << "<videoSiteDir path=\"\">\n</videoSiteDir>\n"
     << "<Correspondences>\n";
  for (auto & track : tracks) {
    os << "<Correspondence>\n";
    for (unsigned j = 0; j < track.views_.size(); j++) {
      bapl_lowe_keypoint_sptr kp;
      kp.vertical_cast(track.views_[j].second);
      double x = kp->location_j();
      double y = kp->location_i();
      if (img_height > 0 && img_width > 0) {
        x += 0.5 * img_width;
        y += 0.5 * img_height;
        y = img_height - y - 1.0;
      }
      os << "<CE fr=\"" << track.views_[j].first << "\" u=\"" << y << "\" v=\"" << x << "\">\n</CE>\n";
    }
    os << "</Correspondence>\n";
  }
  os << "</Correspondences>\n"
     << "</BWM_VIDEO_SITE>\n";
}


//: Print a summary of the connectivity data to a stream
std::ostream& operator<< (std::ostream& os, bapl_conn_table const & t)
{
  for (unsigned i = 0; i < t.conns_.size() ; i++) {
    os << "img("<< i << ") number of neighbors: " << t.conns_[i].size() << std::endl;
  }
  return os;
}


//: Binary io, NOT IMPLEMENTED, signatures defined to use bapl_keypoint_set as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, bapl_conn_table const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & /*is*/, bapl_conn_table & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, bapl_conn_table* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new bapl_conn_table(1);
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const bapl_conn_table* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
