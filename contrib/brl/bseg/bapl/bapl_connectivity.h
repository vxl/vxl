// This is brl/bseg/bapl/bapl_connectivity.h
#ifndef bapl_connectivity_h_
#define bapl_connectivity_h_
//:
// \file
// \brief A class to build image connectivity graph, see Snavely, Seitz, Szeliski - Modeling the World from Internet Photo Collections.
// \author Ozge C. Ozcanli, (ozge@lems.brown.edu)
// \date Sep 21, 2010
//
// \verbatim
//   none
// \endverbatim

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vbl/vbl_ref_count.h>
#include <bapl/bapl_keypoint_set_sptr.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef std::pair< int, bapl_keypoint_sptr > bapl_image_key;
typedef std::vector< bapl_image_key > bapl_image_key_vector;

//: class to hold point tracks (2d correspondences in each image for a single 3d point)
class bapl_track_data : public vbl_ref_count
{
 public:
  bapl_track_data(bapl_image_key_vector& vec) : views_(vec) {}

  bapl_image_key_vector views_;
};

//: For sorting keypoint_match_sets
bool second_less( const bapl_keypoint_match_set_sptr& left_set, const bapl_keypoint_match_set_sptr& right_set);

//: a type to hold a list of matches to other images for an image, initially during construction connectivities of each image will be empty
//  Stores all the connectivities of this image to the other images in a set of images
typedef std::vector<bapl_keypoint_match_set_sptr> bapl_conn;

//: Class to hold connectivities of each image in a list
//  If an image does not have a conn to any other image then its bapl_conn vector will be empty
class bapl_conn_table : public vbl_ref_count
{
 public:
  //: Constructor
  bapl_conn_table(int n_images) : conns_(n_images), img_data_(n_images), img_data_key_flags_(n_images, std::vector<bool>()) {}  // initialize with an empty connectivity
  //: add this match set symmetrically, i.e. into the list of img id 1 as well img id 2, while adding for img id 2, reverse the keypoint pairs
  bool add_sym(const bapl_keypoint_match_set_sptr& set);
  bool add(const bapl_keypoint_match_set_sptr& set);

  //: add keypoints for the given image with id i
  void add_image_data(int i, bapl_keypoint_set_sptr data) { img_data_[i] = data; }

  //: check if vector of image id1 already contains a match set for image id2
  bool contains(int id1, int id2);

  //: make the table symmetric, only necessary if add() method is used as opposed to add_sym()
  void make_symmetric();

  void print_table();
  void print_table_with_matches();

  //: return the number of neighbors for image with id i
  unsigned get_number_of_neighbors(unsigned i);
  bapl_conn& get_neighbors(unsigned i) { return conns_[i]; }

  //: compute a set of tracks, each corresponding to a separate 3d point
  //  Assumes a symmetric connectivity table
  bool compute_tracks(std::vector<bapl_track_data>& tracks, int new_image_start = 0);

  std::vector<bapl_conn> conns_;
  std::vector<bapl_keypoint_set_sptr> img_data_;
  std::vector<std::vector<bool> > img_data_key_flags_;
};

//: Print a summary of the connectivity data to a stream
std::ostream& operator<< (std::ostream& os, bapl_conn_table const & t);

//: Print tracks as correspondences in BWM_VIDEO_SITE format for visualization
void print_tracks(std::ostream& os, std::vector<bapl_track_data>& tracks, int img_width = 0, int img_height = 0);

// Binary io, NOT IMPLEMENTED, signatures defined to use bapl_keypoint_set as a brdb_value
void vsl_b_write(vsl_b_ostream & os, bapl_conn_table const &ph);
void vsl_b_read(vsl_b_istream & is, bapl_conn_table &ph);
void vsl_b_read(vsl_b_istream& is, bapl_conn_table* ph);
void vsl_b_write(vsl_b_ostream& os, const bapl_conn_table* &ph);

#endif // bapl_connectivity_h_
