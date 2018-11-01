// This is core/vul/vul_sequence_filename_map.cxx

// Author: David Capel, Oxford RRG
// Created: 15 April 2000
//
//-----------------------------------------------------------------------------

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include "vul_sequence_filename_map.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_sprintf.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_file_iterator.h>

constexpr bool debug = false;

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

vul_sequence_filename_map::vul_sequence_filename_map (std::string  seq_template, std::vector<int>  indices)
  : seq_template_(std::move(seq_template)), indices_(std::move(indices)), start_(-1), step_(-1), end_(-1)
{
  parse();
}

vul_sequence_filename_map::vul_sequence_filename_map (std::string  seq_template, int start, int end, int step)
  : seq_template_(std::move(seq_template)), start_(start), step_(step), end_(end)
{
  for (int i=start; i <= end; i+=step)
    indices_.push_back(i);
  parse();
}

vul_sequence_filename_map::vul_sequence_filename_map (std::string  seq_template, int step)
  : seq_template_(std::move(seq_template)), start_(-1), step_(step), end_(-1)
{
  parse();
}

vul_sequence_filename_map::~vul_sequence_filename_map() = default;

std::string vul_sequence_filename_map::name(int frame)
{
  std::string index_str = vul_sprintf(index_format_.c_str(), indices_[frame]);
  return basename_ + index_str;
}

std::string vul_sequence_filename_map::pair_name (int i, int j)
{
  std::string index_str = vul_sprintf((index_format_ + "." + index_format_).c_str(), indices_[i], indices_[j]);
  return basename_ + index_str;
}

std::string vul_sequence_filename_map::triplet_name (int i, int j, int k)
{
  std::string index_str = vul_sprintf((index_format_ + "." +
    index_format_ + "." + index_format_).c_str(), indices_[i],
    indices_[j], indices_[k]);
  return basename_ + index_str;
}

void vul_sequence_filename_map::parse()
{
  std::string temp = seq_template_;

  // Search for trailing index spec -   "img.%03d.pgm,0:1:10" , "ppm/img*;:5:"
  {
    vul_reg_exp re("[,;]([0-9]+)?(:[0-9]+)?:([0-9]+)?$");
    if (re.find(temp.c_str()))
    {
      std::string match_start = re.match(1);
      std::string match_step = re.match(2);
      std::string match_end = re.match(3);

      temp.erase(re.start(0));

      if (match_start.length() > 0)
        start_ = std::atoi(match_start.c_str());

      if (match_step.length() > 0)
        step_ = std::atoi(match_step.c_str()+1);

      if (match_end.length() > 0)
        end_ = std::atoi(match_end.c_str());
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
    std::string bt;
    if (re.find(temp.c_str())) {
      bt = re.match(0);
      temp.erase(re.start(0));
    }
    // This should have the form "img.%03d" or "img.###". Split it into basename and index format
    std::size_t pos;
    if ( (pos = bt.find('%')) != std::string::npos)
      index_format_ = bt.substr(pos);
    else if ( (pos = bt.find('#')) != std::string::npos) {
      std::size_t last_pos = bt.rfind('#');
      index_format_ = vul_sprintf("0%id",last_pos - pos + 1);
      index_format_ = "%" + index_format_;
    }
    else
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
  if (image_dir_ == "" && image_extension_ == "")
  {
    bool found_match = false;
    {
      vul_file_iterator fn("./*");
      for (;!found_match && bool(fn); ++fn)
        for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
          if (filter_dirent(fn(), dir_ext_pairs[i].extension)) {
            image_dir_ = "./";
            image_extension_ = dir_ext_pairs[i].extension;
            found_match = true;
          }
    }
    if (!found_match)
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
        std::string glob(dir_ext_pairs[i].image_dir);
        glob += "/*";
        vul_file_iterator fn(glob);
        for (;!found_match && bool(fn);++fn)
          if (filter_dirent(fn(), dir_ext_pairs[i].extension)) {
            image_dir_ = dir_ext_pairs[i].image_dir;
            image_extension_ = dir_ext_pairs[i].extension;
            found_match = true;
          }
      }
    if (!found_match) {
      std::cerr << __FILE__ << " : Can't find files matching " << basename_
               << index_format_ << " in common locations with common format!\n";
      std::abort();
    }
  }

  // Only image dir is blank :
  //  1 - Look for basename-compatible files in cwd
  //  2 - Look for basename-compatible files in dir corresponding to given image extension
  //  3 - Look for basename-compatible files in common image dirs
  else if (image_dir_ == "")
  {
    bool found_match = false;
    {
      for (vul_file_iterator fn("./*"); !found_match && bool(fn); ++fn)
        if (filter_dirent(fn.filename(), image_extension_)) {
          image_dir_ = "./";
          found_match = true;
        }
    }

    if (!found_match) {
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
        if (std::string(dir_ext_pairs[i].extension) == image_extension_) {
          std::string glob(dir_ext_pairs[i].image_dir); glob += "*";
          for (vul_file_iterator fn(glob); !found_match && bool(fn); ++fn)
            if (filter_dirent(fn.filename(), image_extension_)) {
              image_dir_ = dir_ext_pairs[i].image_dir;
              found_match = true;
            }
        }
    }

    if (!found_match) {
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
        std::string glob(dir_ext_pairs[i].image_dir); glob += "*";
        for (vul_file_iterator fn(glob); !found_match && bool(fn); ++fn)
          if (filter_dirent(fn.filename(), image_extension_)) {
            image_dir_ = dir_ext_pairs[i].image_dir;
            found_match = true;
          }
      }
    }

    if (!found_match) {
      std::cerr << __FILE__ << " : Can't find files matching " << basename_
               << index_format_<<image_extension_ << " in common locations!\n";
      std::abort();
    }
  }

  // Only extension is blank :
  //  1 - Look in image dir for basename-compatible files with extension corresponding to the image dir
  //  2 - Look in image dir for basename-compatible files with common image extensions
  else if (image_extension_ == "")
  {
    bool found_match = false;
    {
      std::string glob(image_dir_ + "*");
      vul_file_iterator fn(glob);
      if (fn) {
        for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
          if (std::string(dir_ext_pairs[i].image_dir) == image_dir_) {
            for (;!found_match && bool(fn);++fn)
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
          for (;!found_match && bool(fn); ++fn)
            if (filter_dirent(fn.filename(), dir_ext_pairs[i].extension)) {
              image_extension_ = dir_ext_pairs[i].extension;
              found_match = true;
            }
        }
      }
    }

    if (!found_match) {
      std::cerr << __FILE__ << " : Can't find files matching " << image_dir_
               << basename_ << index_format_ << " with common extension!\n";
      std::abort();
    }
  }

  // Start and/or end is not specified :
  //   Find all basename-compatible files and set sequence indices accordingly
  if (indices_.size() == 0)
  {
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
        std::cerr << "vul_sequence_filename_map: WARNING: no files in " << image_dir_ << std::endl;
      }

      if (start_ == -1) start_ = min;
      if (end_ == -1) end_ = max;
    }
    for (int i=start_; i <=  end_; i = i+step_)
      indices_.push_back(i);
  }

  if (debug)
    std::cerr << seq_template_ << std::endl
             << "    image dir : " << image_dir_ << std::endl
             << "    basename  : " << basename_ << std::endl
             << " index format : " << index_format_ << std::endl
             << "    extension : " << image_extension_ << std::endl
             << "    indices   : " << start_ << ':' << step_ << ':' << end_
             << std::endl << std::endl;
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


std::ostream& vul_sequence_filename_map::print (std::ostream& s) const
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

bool vul_sequence_filename_map::filter_dirent(char const* name_string, std::string const& extension)
{
  static std::size_t expected_length = 0L;
  if (expected_length == 0L)
    expected_length = basename_.size() +
                      (std::string(vul_sprintf(index_format_.c_str(),0)) + extension).size();

  std::string name_str(name_string);

  return name_str.size() == expected_length
      && name_str.substr(0,basename_.size()) == basename_
      && name_str.substr(expected_length-extension.size(), std::string::npos) == extension;
}

int vul_sequence_filename_map::extract_index(char const* name_string)
{
  std::string name_str(name_string);
  std::string index_str = name_str.substr(basename_.size(), name_str.size() - image_extension_.size());
  return std::atoi(index_str.c_str());
}

std::ostream& operator<<(std::ostream &os, const vul_sequence_filename_map& s)
{
  return s.print(os);
}
