// This is oxl/oxp/oxp_bunch_of_files.h
#ifndef oxp_bunch_of_files_h_
#define oxp_bunch_of_files_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// Created: 18 Dec 01

#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vxl_config.h> // for vxl_int_64

struct oxp_bunch_of_files
{
#if VXL_HAS_INT_64
  typedef vxl_int_64 offset_t;
#else // vcl_int_64 is typedef'd to void
  typedef long offset_t;
#endif

  oxp_bunch_of_files();
  oxp_bunch_of_files(char const* fmt);
  ~oxp_bunch_of_files();

  bool open(char const* fmt);
  bool open_1(char const* fmt);
  void close();

  bool seek(offset_t to);
  offset_t tell() const;
  int read(void*, unsigned int);
  bool ok() { return current_file_index != -1; }

 private:
  bool fix_sizes(); // filenames has been filled

  int current_file_index;
  vcl_vector<vcl_string> filenames;
  vcl_vector<vcl_FILE*> fps;
  vcl_vector<unsigned int> filesizes;
  vcl_vector<offset_t> start_byte;
};

#endif   // oxp_bunch_of_files_h_
