// This is core/vul/vul_sequence_filename_map.h
#ifndef vul_sequence_filename_map_h_
#define vul_sequence_filename_map_h_
//:
// \file
// \brief Contains class for mapping sequence frame indices to filenames
// \author David Capel, Oxford RRG
// \date   15 April 2000
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim

#include <string>
#include <iosfwd>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Maps sequence frame indices to filenames
// vul_sequence_filename_map maps sequence frame indices to filenames. It
// also performs some disk probing functions to discover the image extension,
// directories and sequence template if not specified by the user.
//
// The template can have any of the following forms :
//   "pgm/img.%03d.pgm,0:5:100", "pgm/img.####.pgm", "img.###.pgm",
// "img.###,5:1:20", "img.%02d,:5:" (you get the idea..) If the image
// directory and/or filename extension are not specified they
// are automagically determined by probing the current directory for likely
// candidates.
//
// The vector of indices specifies the mapping from sequence frame-index to
// filename-index.  If not specified, the image directory is probed to
// determine the sequence start/end frames.
class vul_sequence_filename_map
{
 public:
  vul_sequence_filename_map ();
  vul_sequence_filename_map (std::string  seq_template,
                             std::vector<int>  indices);
  vul_sequence_filename_map (std::string  seq_template,
                             int start, int end, int step = 1);
  vul_sequence_filename_map (std::string  seq_template,
                             int step = 1);
  ~vul_sequence_filename_map ();

  //: returns frame name with no extension, e.g. "img.003", "img.003.004"
  std::string name(int frame);
  std::string pair_name(int i, int j);
  std::string triplet_name(int i, int j, int k);

  std::string image_name(int frame)
  { return image_dir_ + name(frame) + image_extension_; }

  //: returns the image directory e.g. "pgm/"
  std::string get_image_dir() const { return image_dir_; }

  //: returns the image extension e.g. ".pgm"
  std::string get_image_extension() const { return image_extension_; }

  //: returns the base name e.h. "img."
  std::string get_base_name() const { return basename_; }

  //: returns the actually on-disk index corresponding to frame N
  int get_real_index(int frame) const { return indices_[frame]; }
  std::vector<int> const& get_real_indices() const { return indices_; }

  //: returns the frame number corresp. to on-disk index N, or -1 if out-of-range
  int get_mapped_index(int real) const;

  int get_nviews() const { return int(indices_.size()); }


  //: pretty print
  std::ostream& print(std::ostream& s) const;

 protected:
  bool filter_dirent(char const* name, std::string const& extension);
  int extract_index(char const* name);

  std::string seq_template_;
  std::vector<int> indices_;
  std::string basename_;
  std::string index_format_;
  std::string image_dir_;
  std::string image_extension_;
  int start_;
  int step_;
  int end_;

  void parse();
};

std::ostream& operator<<(std::ostream &os, const vul_sequence_filename_map& s);

#endif // vul_sequence_filename_map_h_
