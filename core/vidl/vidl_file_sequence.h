// This is core/vidl/vidl_file_sequence.h
#ifndef vidl_file_sequence_h_
#define vidl_file_sequence_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 18 Dec 01
// copied by l.e.galup from oxl/oxp/oxp_bunch_of_files
// 10-18-02

#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vcl_string.h>

class vidl_file_sequence
{
  typedef unsigned long offset_t;
 public:
  vidl_file_sequence() {}
  vidl_file_sequence(vcl_string const& fmt);
  ~vidl_file_sequence() { close(); }

  bool open(vcl_string const& fmt);
  void close();

  bool seek(offset_t to);
  offset_t tell() const;
  offset_t read(void*, offset_t);
  bool ok() const { return current_file_index != -1; }

 private:
  int current_file_index;
  vcl_vector<vcl_string> filenames;
  vcl_vector<vcl_FILE*> fps;
  vcl_vector<offset_t> filesizes;
  vcl_vector<offset_t> start_byte;
};

#endif   // vidl_file_sequence_h_
