// This is core/vul/vul_sequence_filename_map.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

// Author: David Capel, Oxford RRG
// Created: 15 April 2000
//
//-----------------------------------------------------------------------------

#include "vul_sequence_filename_map.h"
#include <vcl_cstddef.h> // for vcl_size_t
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <vul/vul_sprintf.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_file_iterator.h>

const bool debug = 0;

static struct
{
  char const * image_dir;
  char const * extension;
} dir_ext_pairs[] = { {"pgm/",".pgm"},
                      {"ppm/",".ppm"},
                      {"jpg/",".jpg"},
                      {"jpg/",".jpeg"},
                      {"jpeg/",".jpg"},
                      {"jpeg/",".jpeg"},
                      {"tiff/",".tiff"},
                      {"mit/",".mit"},
                      {"viff/",".viff"},
                      {"rgb/",".rgb"} };

const int num_dir_ext_pairs = sizeof(dir_ext_pairs) / sizeof(dir_ext_pairs[0]);

// Empty constructor to allow the operator= to be used later.
vul_sequence_filename_map::vul_sequence_filename_map ()
  : start_(-1), step_(-1), end_(-1)
{
}

vul_sequence_filename_map::vul_sequence_filename_map (vcl_string const & seq_template, vcl_vector<int> const & indices)
  : seq_template_(seq_template), indices_(indices), start_(-1), step_(-1), end_(-1)
{
  parse();
}

vul_sequence_filename_map::vul_sequence_filename_map (vcl_string const & seq_template, int start, int end, int step)
  : seq_template_(seq_template), start_(start), step_(step), end_(end)
{
  for (int i=start; i <= end; i+=step)
    indices_.push_back(i);
  parse();
}

vul_sequence_filename_map::vul_sequence_filename_map (vcl_string const & seq_template, int step)
  : seq_template_(seq_template), start_(-1), step_(step), end_(-1)
{
  parse();
}

vul_sequence_filename_map::~vul_sequence_filename_map()
{
}

vcl_string vul_sequence_filename_map::name(int frame)
{
  vcl_string index_str = vul_sprintf(index_format_.c_str(), indices_[frame]);
  return basename_ + index_str;
}

vcl_string vul_sequence_filename_map::pair_name (int i, int j)
{
  vcl_string index_str = vul_sprintf((index_format_ + "." + index_format_).c_str(), indices_[i], indices_[j]);
  return basename_ + index_str;
}

vcl_string vul_sequence_filename_map::triplet_name (int i, int j, int k)
{
  vcl_string index_str = vul_sprintf((index_format_ + "." +
    index_format_ + "." + index_format_).c_str(), indices_[i],
    indices_[j], indices_[k]);
  return basename_ + index_str;
}

void vul_sequence_filename_map::parse()
{
  vcl_string temp = seq_template_;

  // Search for trailing index spec -   "img.%03d.pgm,0:1:10" , "ppm/img*;:5:"
  {
    vul_reg_exp re("[,;]([0-9]+)?(:[0-9]+)?:([0-9]+)?$");
    if (re.find(temp.c_str())) {
      vcl_string match_start = re.match(1);
      vcl_string match_step = re.match(2);
      vcl_string match_end = re.match(3);

      temp.erase(re.start(0));

      if (match_start.length() > 0)
        start_ = vcl_atoi(match_start.c_str());

      if (match_step.length() > 0)
        step_ = vcl_atoi(match_step.c_str()+1);

      if (match_end.length() > 0)
        end_ = vcl_atoi(match_end.c_str());
    }
  }
  // Search for image extension
  {
    vul_reg_exp re("\\.([a-zA-Z_0-9]+)$");
    if (re.find(temp.c_str())) {
      image_extension_ = re.match(0);
      temp.erase(re.start(0));
    }
  }
  // Search for basename template
  {
    vul_reg_exp re("([a-zA-Z0-9_%#\\.]+)$");
    vcl_string bt;
    if (re.find(temp.c_str())) {
      bt = re.match(0);
      temp.erase(re.start(0));
    }
    // This should have the form "img.%03d" or "img.###". Split it into basename and index format
    vcl_size_t pos;
    if ( (pos = bt.find('%')) != vcl_string::npos)
      index_format_ = bt.substr(pos);
    else if ( (pos = bt.find('#')) != vcl_string::npos) {
      vcl_size_t last_pos = bt.rfind('#');
      index_format_ = vul_sprintf("0%id",last_pos - pos + 1);
      index_format_ = "%" + index_format_;
    } else
      index_format_ = "%03d";
    basename_ = bt.substr(0,pos);
  }
  // What remains must be the directory
  image_dir_ = temp;

  // Now to fill in any blanks
  //
  // Image dir and extension both blank :
  //  1 - Look in cwd for basename-compatible files with common image extensions
  //  2 - Look for basename-compatible files in common image dirs with corresponding image extensions
  if (image_dir_ == "" && image_extension_ == "") {
    bool found_match = false;
    {
      vul_file_iterator fn("./*");
      for (;!found_match && fn; ++fn)
        for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
          if (filter_dirent(fn(), dir_ext_pairs[i].extension)) {
            image_dir_ = "./";
            image_extension_ = dir_ext_pairs[i].extension;
            found_match = true;
          }
    }
    if (!found_match)
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
        vcl_string glob(dir_ext_pairs[i].image_dir);
        glob += "/*";
        vul_file_iterator fn(glob);
        for (;!found_match && fn;++fn)
            if (filter_dirent(fn(), dir_ext_pairs[i].extension)) {
              image_dir_ = dir_ext_pairs[i].image_dir;
              image_extension_ = dir_ext_pairs[i].extension;
              found_match = true;
            }
      }
    if (!found_match) {
      vcl_cerr << __FILE__ << " : Can't find files matching " << basename_
               << index_format_ << " in common locations with common format!\n";
      vcl_abort();
    }
  }

  // Only image dir is blank :
  //  1 - Look for basename-compatible files in cwd
  //  2 - Look for basename-compatible files in dir corresponding to given image extension
  //  3 - Look for basename-compatible files in common image dirs
  else if (image_dir_ == "") {
    bool found_match = false;
    {
      for (vul_file_iterator fn("./*"); !found_match && fn; ++fn)
        if (filter_dirent(fn.filename(), image_extension_)) {
          image_dir_ = "./";
          found_match = true;
        }
    }

    if (!found_match) {
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
        if (vcl_string(dir_ext_pairs[i].extension) == image_extension_) {
          vcl_string glob(dir_ext_pairs[i].image_dir); glob += "*";
          for (vul_file_iterator fn(glob); !found_match && fn; ++fn)
            if (filter_dirent(fn.filename(), image_extension_)) {
              image_dir_ = dir_ext_pairs[i].image_dir;
              found_match = true;
            }
        }
    }

    if (!found_match) {
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
        vcl_string glob(dir_ext_pairs[i].image_dir); glob += "*";
        for (vul_file_iterator fn(glob); !found_match && fn; ++fn)
          if (filter_dirent(fn.filename(), image_extension_)) {
            image_dir_ = dir_ext_pairs[i].image_dir;
            found_match = true;
          }
      }
    }

    if (!found_match) {
      vcl_cerr << __FILE__ << " : Can't find files matching " << basename_
               << index_format_<<image_extension_ << " in common locations!\n";
      vcl_abort();
    }
  }

  // Only extension is blank :
  //  1 - Look in image dir for basename-compatible files with extension corresponding to the image dir
  //  2 - Look in image dir for basename-compatible files with common image extensions
  else if (image_extension_ == "") {
    bool found_match = false;
    {
      vcl_string glob(image_dir_ + "*");
      vul_file_iterator fn(glob);
      if (fn) {
        for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
          if (vcl_string(dir_ext_pairs[i].image_dir) == image_dir_) {
            for (;!found_match && fn;++fn)
              if (filter_dirent(fn.filename(), dir_ext_pairs[i].extension)) {
                image_extension_ = dir_ext_pairs[i].extension;
                found_match = true;
              }
          }
      }
    }

    if (!found_match) {
      vul_file_iterator fn(image_dir_);
      if (fn) {
        for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
          for (;!found_match && fn; ++fn)
            if (filter_dirent(fn.filename(), dir_ext_pairs[i].extension)) {
              image_extension_ = dir_ext_pairs[i].extension;
              found_match = true;
            }
        }
      }
    }

    if (!found_match) {
      vcl_cerr << __FILE__ << " : Can't find files matching " << image_dir_
               << basename_ << index_format_ << " with common extension!\n";
      vcl_abort();
    }
  }

  // Start and/or end is not specified :
  //   Find all basename-compatible files and set sequence indices accordingly
  if (indices_.size() == 0) {
    // See if we need to scan the image directory to get the sequence start/end
    if (start_ == -1 || end_ == -1) {
      int max = -1000000;
      int min = 1000000;
      for (vul_file_iterator fn(image_dir_ + "*");fn;++fn)
        if (filter_dirent(fn.filename(), image_extension_)) {
          int index = extract_index(fn.filename());
          max = (index > max) ? index : max;
          min = (index < min) ? index : min;
        }
      if (max < min) {
        vcl_cerr << "vul_sequence_filename_map: WARNING: no files in " << image_dir_ << vcl_endl;
      }

      if (start_ == -1) start_ = min;
      if (end_ == -1) end_ = max;
    }
    for (int i=start_; i <=  end_; i = i+step_)
      indices_.push_back(i);
  }

  if (debug)
    vcl_cerr << seq_template_ << vcl_endl
             << "    image dir : " << image_dir_ << vcl_endl
             << "    basename  : " << basename_ << vcl_endl
             << " index format : " << index_format_ << vcl_endl
             << "    extension : " << image_extension_ << vcl_endl
             << "    indices   : " << start_ << ':' << step_ << ':' << end_
             << vcl_endl << vcl_endl;
}

int vul_sequence_filename_map::get_mapped_index(int real) const
{
  int idx = -1;
  for (int i=0; i < int(indices_.size()); ++i)
    if (indices_[i] == real) {
      idx = i;
      break;
    }
  return idx;
}


vcl_ostream& vul_sequence_filename_map::print (vcl_ostream& s) const
{
  s << "vul_sequence_filename_map : " << image_dir_ << basename_
    << index_format_ << image_extension_ << " [" << indices_[0] << ':'
    << indices_[1] - indices_[0] << ':' << indices_.back() << "]\n";

#if 0
  s << vul_sprintf("vul_sequence_filename_map : %s%s%s [%i:%i:%i]",
                   image_dir_.c_str(), basename_.c_str(), index_format_.c_str(), image_extension_.c_str(),
                   indices_[0], indices_[1] - indices_[0], indices_.back());
#endif
  return s;
}

bool vul_sequence_filename_map::filter_dirent(char const* name_string, vcl_string const& extension)
{
  static unsigned int expected_length = 0;
  if (expected_length == 0)
    expected_length = basename_.size() +
    (vcl_string(vul_sprintf(index_format_.c_str(),0)) + extension).size();

  vcl_string name_str(name_string);

  if (name_str.size() != expected_length) return false;

  if (name_str.substr(0,basename_.size()) == basename_ &&
      name_str.substr(expected_length-extension.size(), vcl_string::npos) == extension) return true;

  return false;
}

int vul_sequence_filename_map::extract_index(char const* name_string)
{
  vcl_string name_str(name_string);
  vcl_string index_str = name_str.substr(basename_.size(), name_str.size() - image_extension_.size());
  return vcl_atoi(index_str.c_str());
}

vcl_ostream& operator<<(vcl_ostream &os, const vul_sequence_filename_map& s)
{
  return s.print(os);
}
