#ifndef vbl_sequence_filename_map_h_
#define vbl_sequence_filename_map_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_sequence_filename_map - maps sequence frame indices to filenames
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_sequence_filename_map.h
// .FILE	vbl_sequence_filename_map.cxx
//
// .SECTION Description
//
// vbl_sequence_filename_map maps sequence frame indices to filenames. It also performs
// some disk probing functions to discover the image extension, directories and sequence
// template if not specified by the user.
// The template can have any of the following forms :
//   "pgm/img.%03d.pgm,0:5:100", "pgm/img.####.pgm", "img.###.pgm", "img.###,5:1:20", "img.%02d,:5:" (you get the idea..)
// If the image directory and/or filename extension are not specified they
// are automagically determined by probing the current directory for likely candidates.
// The vector of indices specifies the mapping from sequence frame-index to filename-index.
// If not specified, the image directory is probed to determine the sequence start/end frames.
//
// .SECTION Author
//     David Capel, Oxford RRG, 15 April 2000
//
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_string.h> // C++ specific includes first
#include <vcl/vcl_iosfwd.h>
#include <vcl/vcl_vector.h>

class vbl_sequence_filename_map {
public:
  vbl_sequence_filename_map ();
  vbl_sequence_filename_map (vcl_string const & seq_template, vcl_vector<int> const & indices);
  vbl_sequence_filename_map (vcl_string const & seq_template, int start, int end, int step = 1);
  vbl_sequence_filename_map (vcl_string const & seq_template, int step = 1);
  ~vbl_sequence_filename_map ();

  // returns frame name with no extension, e.g. "img.003", "img.003.004"
  vcl_string name(int frame);
  vcl_string pair_name(int i, int j);
  vcl_string triplet_name(int i, int j, int k);

  vcl_string image_name(int frame) { return image_dir_ + name(frame) + image_extension_; }

  // returns the image directory e.g. "pgm/"
  vcl_string get_image_dir() const { return image_dir_; }

  // returns the image extension e.g. ".pgm"
  vcl_string get_image_extension() const { return image_extension_; }

  int get_real_index(int frame) const { return indices_[frame]; }
  vcl_vector<int> const& get_real_indices() const { return indices_; }

  int get_nviews() const { return indices_.size(); }


  // pretty print
  ostream& print(ostream& s) const;

protected:
  bool filter_dirent(char const* name, vcl_string const& extension);
  int extract_index(char const* name);

  vcl_string seq_template_;
  vcl_vector<int> indices_;
  vcl_string basename_;
  vcl_string index_format_;
  vcl_string image_dir_;
  vcl_string image_extension_;
  int start_;
  int step_;
  int end_;

  void parse();
};

ostream& operator<<(ostream &os, const vbl_sequence_filename_map& s);

#endif // vbl_sequence_filename_map_h_
