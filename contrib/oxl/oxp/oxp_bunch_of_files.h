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

struct oxp_bunch_of_files
{
  typedef unsigned long offset_t;

  oxp_bunch_of_files() {}
  oxp_bunch_of_files(char const* fmt);
  ~oxp_bunch_of_files() { close(); }

  bool open(char const* fmt);
  bool open_1(char const* fmt);
  void close();

  bool seek(offset_t to);
  offset_t tell() const;
  offset_t read(void*, offset_t);
  bool ok() { return current_file_index != -1; }

 private:
  bool fix_sizes(); // filenames has been filled

  int current_file_index;
  vcl_vector<vcl_string> filenames;
  vcl_vector<vcl_FILE*> fps;
  vcl_vector<offset_t> filesizes;
  vcl_vector<offset_t> start_byte;
};

#endif   // oxp_bunch_of_files_h_
