//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vbl_sequence_filename_map
// Author: David Capel, Oxford RRG
// Created: 15 April 2000
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vbl_sequence_filename_map.h"

#include <dirent.h>

#include <vcl/vcl_string.h>
#include <vcl/vcl_iostream.h>

#include <vbl/vbl_sprintf.h>
#include <vbl/vbl_file.h>
#include <vbl/vbl_reg_exp.h>

static bool debug = 1;

static
struct {
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
vbl_sequence_filename_map::vbl_sequence_filename_map ()
{
}

vbl_sequence_filename_map::vbl_sequence_filename_map (vcl_string const & seq_template, vcl_vector<int> const & indices)
{
  seq_template_ = seq_template;
  indices_ = indices;
  start_ = -1;
  step_ = -1;
  end_ = -1;
  parse();
}

vbl_sequence_filename_map::vbl_sequence_filename_map (vcl_string const & seq_template, int start, int end, int step)
{
  seq_template_ = seq_template;
  for (int i=start; i <= end; i+=step)
    indices_.push_back(i);
  start_ = start;
  step_ = step;
  end_ = end;
  parse();
}

vbl_sequence_filename_map::vbl_sequence_filename_map (vcl_string const & seq_template, int step)
{
  seq_template_ = seq_template;
  start_ = -1;
  step_ = step;
  end_ = -1;
  parse();
}

vbl_sequence_filename_map::~vbl_sequence_filename_map()
{
}

vcl_string vbl_sequence_filename_map::name (int frame)
{
  vcl_string index_str = vbl_sprintf(index_format_.c_str(), indices_[frame]); 
  return basename_ + index_str;
}

vcl_string vbl_sequence_filename_map::pair_name (int i, int j)
{
  vcl_string index_str = vbl_sprintf((index_format_ + "." + index_format_).c_str(), indices_[i], indices_[j]); 
  return basename_ + index_str;
}
vcl_string vbl_sequence_filename_map::triplet_name (int i, int j, int k)
{
  vcl_string index_str = vbl_sprintf((index_format_ + "." + index_format_ + "." + index_format_).c_str(), indices_[i], indices_[j], indices_[k]); 
  return basename_ + index_str;
}

void vbl_sequence_filename_map::parse()
{
  vcl_string temp = seq_template_;

  // Search for trailing index spec -   "img.%03d.pgm,0:1:10" , "ppm/img*;:5:"
  {
    vbl_reg_exp re("[,;]([0-9]+)?(:[0-9]+)?:([0-9]+)?$");
    if (re.find(temp.c_str())) {
      vcl_string match_start = re.match(1);
      vcl_string match_step = re.match(2);
      vcl_string match_end = re.match(3);
      
      temp.erase(re.start(0));
      
      if (match_start.length() > 0)
	start_ = atoi(match_start.c_str());
      
      if (match_step.length() > 0)
	step_ = atoi(match_step.c_str()+1);
      
      if (match_end.length() > 0)
	end_ = atoi(match_end.c_str());
    }
  }
  // Search for image extension
  {
    vbl_reg_exp re("\\.([a-zA-Z_0-9]+)$");
    if (re.find(temp.c_str())) {
      image_extension_ = re.match(0);
      temp.erase(re.start(0));
    }
  }
  // Search for basename template
  {
    vbl_reg_exp re("([a-zA-Z0-9_%#\\.]+)$");
    vcl_string bt;
    if (re.find(temp.c_str())) {
      bt = re.match(0);
      temp.erase(re.start(0));
    }
    // This should have the form "img.%03d" or "img.###". Split it into basename and index format
    size_t pos;
    if ( (pos = bt.find('%')) != vcl_string::npos)
      index_format_ = bt.substr(pos);
    else if ( (pos = bt.find('#')) != vcl_string::npos) {
      size_t last_pos = bt.rfind('#');
      index_format_ = vbl_sprintf("0%id",last_pos - pos + 1);      
      index_format_ = "%" + index_format_;
    } else
      index_format_ = "%03d";
    basename_ = bt.substr(0,pos);
  }
  // What remains must be the directory 
  image_dir_ = temp;

  // Now to fill in any blanks

  // Image dir and extension both blank :
  //  1 - Look in cwd for basename-compatible files with common image extensions
  //  2 - Look for basename-compatible files in common image dirs with corresponding image extensions
  if (image_dir_ == "" && image_extension_ == "") {
    bool found_match = false;
    {    
      DIR* dir_handle = opendir("./");
      dirent* de;
      while (!found_match && (de = readdir(dir_handle)) != NULL)
	for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) 
	  if (filter_dirent(de->d_name, dir_ext_pairs[i].extension)) {
	    image_dir_ = "./";
	    image_extension_ = dir_ext_pairs[i].extension;
	    found_match = true;
	  }
      closedir(dir_handle);
    }
    if (!found_match)
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
	DIR* dir_handle = opendir(dir_ext_pairs[i].image_dir);
	if (dir_handle) {
	  dirent* de;
	  while (!found_match && (de = readdir(dir_handle)) != NULL)
	    if (filter_dirent(de->d_name, dir_ext_pairs[i].extension)) {
	      image_dir_ = dir_ext_pairs[i].image_dir;
	      image_extension_ = dir_ext_pairs[i].extension;
	      found_match = true;
	    } 
	  closedir(dir_handle);
	}
      }
    if (!found_match) {
      cerr << __FILE__ << " : Can't find files matching " << basename_ << index_format_ << " in common locations with common format!" << endl;
      abort();
    }      
  } 

  // Only image dir is blank :
  //  1 - Look for basename-compatible files in cwd
  //  2 - Look for basename-compatible files in dir corresponding to given image extension
  //  3 - Look for basename-compatible files in common image dirs
  else if (image_dir_ == "") {
    bool found_match = false;
    {
      DIR* dir_handle = opendir("./");
      dirent* de;
      while (!found_match && (de = readdir(dir_handle)) != NULL)
	if (filter_dirent(de->d_name, image_extension_)) {
	  image_dir_ = "./";
	  found_match = true;
	} 
      closedir(dir_handle);
    }
    if (!found_match)
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
	if (vcl_string(dir_ext_pairs[i].extension) == image_extension_) {
	  DIR* dir_handle = opendir(dir_ext_pairs[i].image_dir);
	  if (dir_handle) {
	    dirent* de;
	    while (!found_match && (de = readdir(dir_handle)) != NULL)
	      if (filter_dirent(de->d_name, image_extension_)) {
		image_dir_ = dir_ext_pairs[i].image_dir;
		found_match = true;
	      } 
	  }      
	  closedir(dir_handle);
	}
    if (!found_match)
      for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
	DIR* dir_handle = opendir(dir_ext_pairs[i].image_dir);
	if (dir_handle) {
	  dirent* de;
	  while (!found_match && (de = readdir(dir_handle)) != NULL)
	    if (filter_dirent(de->d_name, image_extension_)) {
	      image_dir_ = dir_ext_pairs[i].image_dir;
	      found_match = true;
	    } 
	}
	closedir(dir_handle);
      }
    if (!found_match) {
      cerr << __FILE__ << " : Can't find files matching " << basename_ << index_format_ << image_extension_ << " in common locations!" << endl;
      abort();
    }
  }

  // Only extension is blank :
  //  1 - Look in image dir for basename-compatible files with extension corresponding to the image dir
  //  2 - Look in image dir for basename-compatible files with common image extensions
  else if (image_extension_ == "") {
    bool found_match = false;
    {
      DIR* dir_handle = opendir(image_dir_.c_str());
      if (dir_handle) {
	for (int i=0; i < num_dir_ext_pairs && !found_match; ++i)
	  if (vcl_string(dir_ext_pairs[i].image_dir) == image_dir_) {
	    dirent* de;
	    while (!found_match && (de = readdir(dir_handle)) != NULL)
	      if (filter_dirent(de->d_name, dir_ext_pairs[i].extension)) {
		image_extension_ = dir_ext_pairs[i].extension;
		found_match = true;
	      } 
	  }      
      }
      closedir(dir_handle);
    }
    if (!found_match) {
      DIR * dir_handle = opendir(image_dir_.c_str());
      if (dir_handle) {
	for (int i=0; i < num_dir_ext_pairs && !found_match; ++i) {
	  dirent* de;
	  while (!found_match && (de = readdir(dir_handle)) != NULL)
	    if (filter_dirent(de->d_name, dir_ext_pairs[i].extension)) {
	      image_extension_ = dir_ext_pairs[i].extension;
	      found_match = true;
	    } 
	}      	  
      }
      closedir(dir_handle);
    }
    if (!found_match) {
      cerr << __FILE__ << " : Can't find files matching " << image_dir_ << basename_ << index_format_ << " with common extension!" << endl;
      abort();
    }
  }

  // Start and/or end is not specified :
  //   Find all basename-compatible files and set sequence indices accordingly
  if (indices_.size() == 0) {
    // See if we need to scan the image directory to get the sequence start/end
    if (start_ == -1 || end_ == -1) {
      int max = -1000000;
      int min = 1000000;
      DIR* dir_handle = opendir(image_dir_.c_str());
      dirent* de;
      while ((de = readdir(dir_handle)) != NULL)
	if (filter_dirent(de->d_name, image_extension_)) {
	  int index = extract_index(de->d_name);
	  max = (index > max) ? index : max;
	  min = (index < min) ? index : min;
	}
      closedir(dir_handle);

      if (start_ == -1) start_ = min;
      if (end_ == -1) end_ = max;
    }
    for (int i=start_; i <=  end_; i = i+step_)
      indices_.push_back(i);
  }

  if (debug) {
    cerr << seq_template_ << endl;
    cerr << "    image dir : " << image_dir_ << endl;
    cerr << "    basename  : " << basename_ << endl;
    cerr << " index format : " << index_format_ << endl;
    cerr << "    extension : " << image_extension_ << endl;
    cerr << "    indices   : " << start_ << ":" << step_ << ":" << end_ << endl;
    cerr << endl;
  }
}

ostream& vbl_sequence_filename_map::print (ostream& s) const
{
  s << vbl_sprintf("vbl_sequence_filename_map : %s%s%s [%i:%i:%i]",
		   image_dir_.c_str(), basename_.c_str(), index_format_.c_str(), image_extension_.c_str(),
		   indices_[0], indices_[1] - indices_[0], indices_.back());
  return s;
}

bool vbl_sequence_filename_map::filter_dirent(char const* name, vcl_string const& extension)
{
  static int expected_length = 0;
  if (expected_length == 0)
    expected_length = basename_.size() + 
    (vcl_string(vbl_sprintf(index_format_.c_str(),0)) + extension).size();

  vcl_string name_str(name);

  if (name_str.size() != expected_length) return false;

  if (name_str.substr(0,basename_.size()) == basename_ &&
      name_str.substr(expected_length-extension.size(), vcl_string::npos) == extension) return true;

  return false;
}

int vbl_sequence_filename_map::extract_index(char const* name)
{
  vcl_string name_str(name);
  vcl_string index_str = name_str.substr(basename_.size(), name_str.size() - image_extension_.size());
  return atoi(index_str.c_str());
}

ostream& operator<<(ostream &os, const vbl_sequence_filename_map& s)
{
  return s.print(os);
}
