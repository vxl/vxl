// This is oxl/oxp/oxp_vidl_moviefile.h
#ifndef oxp_vidl_moviefile_h_
#define oxp_vidl_moviefile_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// Created: 08 Dec 01

#include <vcl_iosfwd.h>

#include <oxp/MovieFileInterface.h>

struct oxp_vidl_moviefile_privates;

class oxp_vidl_moviefile : public MovieFileInterface
{
  oxp_vidl_moviefile_privates* p;
 public:
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
};

#endif // oxp_vidl_moviefile_h_
