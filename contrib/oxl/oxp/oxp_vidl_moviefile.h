//-*- c++ -*-------------------------------------------------------------------
#ifndef oxp_vidl_moviefile_h_
#define oxp_vidl_moviefile_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 08 Dec 01

//: Not documented FIXME

#include <vcl_iosfwd.h>

#include <oxp/MovieFileInterface.h>

struct oxp_vidl_moviefile_privates;

struct oxp_vidl_moviefile : public MovieFileInterface {

  oxp_vidl_moviefile(char const* filename);
  ~oxp_vidl_moviefile();

  int GetLength();
  int GetSizeX(int);
  int GetSizeY(int);
  int GetBitsPixel();
  bool IsInterlaced();
  bool HasFrame(int);
  bool GetFrame(int frame_index, void* buffer);
  bool GetField(int field_index, void* buffer);
  vil_image GetImage(int);

  int GetFrameOffset(int frame_index);
  int GetFrameSize(int frame_index);

private:
  oxp_vidl_moviefile_privates* p;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS oxp_vidl_moviefile.

