// This is core/vidl_vil1/vidl_vil1_file_sequence.h
#ifndef vidl_vil1_file_sequence_h_
#define vidl_vil1_file_sequence_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// Created: 18 Dec 01
// copied by l.e.galup to vxl/vidl_vil1
// 10-18-02

#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vcl_string.h>

#include <vxl_config.h> // for vxl_int_64

class vidl_vil1_file_sequence
{
#ifdef vxl_int_64
  typedef vxl_int_64 offset_t;
#else // vcl_int_64 is typedef'd to void
  typedef long offset_t;
#endif
 public:
  vidl_vil1_file_sequence(){}
  vidl_vil1_file_sequence(char const* fmt);
  ~vidl_vil1_file_sequence(){}

  bool open(char const* fmt);
  void close();

  void seek(offset_t to);
  int tell() const;
  int read(void*, unsigned int);
  bool ok() { return current_file_index != -1; }

 private:
  int current_file_index;
  vcl_vector<vcl_string> filenames;
  vcl_vector<vcl_FILE*> fps;
  vcl_vector<unsigned int> filesizes;
  vcl_vector<offset_t> start_byte;
};

#endif   // vidl_vil1_file_sequence_h_
