#ifndef SGIMovieFilePrivates_h_
#define SGIMovieFilePrivates_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	SGIMovieFilePrivates - Undocumented
// .LIBRARY	POX
// .HEADER	Oxford Package
// .INCLUDE	oxp/SGIMovieFilePrivates.h
// .FILE	SGIMovieFilePrivates.cxx
//
// .SECTION Description
//    SGIMovieFilePrivates is deliberately undocumented.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Dec 98
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>

#include <vcl_iosfwd.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

struct SGIMV_Variables {

  typedef vcl_map<vcl_string, vcl_string, vcl_less<vcl_string> > VarData;
  VarData data;

  SGIMV_Variables(istream& f) {
    read(f);
  }
  SGIMV_Variables() {}

  int get_int(char const* s) { return get_int(vcl_string(s)); }
  int get_int(vcl_string const& s);

  double get_double(char const* s) { return get_double(vcl_string(s)); }
  double get_double(vcl_string const& s);

  void read(istream& f);
  ostream& print(ostream& s) const;
};

struct SGIMV_FrameIndex {
  int offset;
  int size;
};

struct SGIMV_FrameIndexArray : public vcl_vector<SGIMV_FrameIndex> {
  SGIMV_FrameIndexArray(istream& f, int n);
  SGIMV_FrameIndexArray() {}
};

struct  SGIMovieFilePrivates {
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
