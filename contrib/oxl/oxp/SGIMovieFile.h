#ifndef SGIMovieFile_h_
#define SGIMovieFile_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    SGIMovieFile - Read SGI movie files
// .LIBRARY oxp
// .HEADER  Oxford Package
// .INCLUDE oxp/SGIMovieFile.h
// .FILE    SGIMovieFile.cxx
//
// .SECTION Description
//    SGIMovieFile is an implementation of MovieFileInterface that reads
//    SGI movie files.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 30 Dec 98
//
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

#include <oxp/MovieFileInterface.h>

struct SGIMovieFilePrivates;

struct SGIMovieFile : public MovieFileInterface {

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

private:
  SGIMovieFilePrivates* p;
};

#endif // SGIMovieFile_h_
