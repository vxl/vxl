// This is oxl/oxp/SGIMovieFile.h
#ifndef SGIMovieFile_h_
#define SGIMovieFile_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Read SGI movie files
//    SGIMovieFile is an implementation of MovieFileInterface that reads
//    SGI movie files.
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 30 Dec 98
//
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

#include <oxp/MovieFileInterface.h>

struct SGIMovieFilePrivates;

//: an implementation of MovieFileInterface that reads SGI movie files
class SGIMovieFile : public MovieFileInterface
{
  SGIMovieFilePrivates* p;
 public:
  SGIMovieFile(char const* filename);
  ~SGIMovieFile();

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

#endif // SGIMovieFile_h_
