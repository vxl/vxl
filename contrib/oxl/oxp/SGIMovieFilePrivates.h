// This is oxl/oxp/SGIMovieFilePrivates.h
#ifndef SGIMovieFilePrivates_h_
#define SGIMovieFilePrivates_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
//    SGIMovieFilePrivates is deliberately undocumented.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Dec 98
//
//-----------------------------------------------------------------------------

#include <vcl_functional.h>
#include <vcl_utility.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>

#include <vcl_iosfwd.h>

struct SGIMV_Variables
{
  typedef vcl_map<vcl_string, vcl_string, vcl_less<vcl_string> > VarData;
  VarData data;

  SGIMV_Variables(vcl_istream& f) {
    read(f);
  }
  SGIMV_Variables() {}

  int get_int(char const* s) { return get_int(vcl_string(s)); }
  int get_int(vcl_string const& s);

  double get_double(char const* s) { return get_double(vcl_string(s)); }
  double get_double(vcl_string const& s);

  void read(vcl_istream& f);
  vcl_ostream& print(vcl_ostream& s) const;
};

struct SGIMV_FrameIndex
{
  int offset;
  int size;
};

struct SGIMV_FrameIndexArray : public vcl_vector<SGIMV_FrameIndex>
{
  SGIMV_FrameIndexArray(vcl_istream& f, int n);
  SGIMV_FrameIndexArray() {}
};

struct  SGIMovieFilePrivates
{
  SGIMovieFilePrivates(char const* f);
  ~SGIMovieFilePrivates() {
    delete glob;
  }

  vcl_string filename;

  int version;
  int width;
  int height;
  int interlaced;
  vcl_string compression;

  SGIMV_Variables* glob;
  vcl_vector<SGIMV_Variables> audio;
  vcl_vector<SGIMV_Variables> video;

  vcl_vector<SGIMV_FrameIndexArray> video_indices;
  vcl_vector<SGIMV_FrameIndexArray> audio_indices;
  vcl_vector<vcl_vector<int> > field_indices;
};

#endif // SGIMovieFilePrivates_h_
