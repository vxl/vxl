//-*- c++ -*-------------------------------------------------------------------
#ifndef ImageSequenceMovieFile_h_
#define ImageSequenceMovieFile_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : ImageSequenceMovieFile
//
// .SECTION Description
//    ImageSequenceMovieFile is a subclass of MovieFileInterface that
//    reads from a sequence of images.
//
// .NAME        ImageSequenceMovieFile - MovieFile subclass
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/ImageSequenceMovieFile.h
// .FILE        ImageSequenceMovieFile.h
// .FILE        ImageSequenceMovieFile.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Dec 98
//
//-----------------------------------------------------------------------------

#include <oxp/MovieFileInterface.h>

struct ImageSequenceMovieFilePrivates;

struct ImageSequenceMovieFile : public MovieFileInterface {
  
  ImageSequenceMovieFile(char const* filepattern, int frame_index_to_search_for_extension);
  ~ImageSequenceMovieFile();
  
  int GetLength();
  int GetSizeX(int);
  int GetSizeY(int);
  int GetBitsPixel();
  bool IsInterlaced();
  bool HasFrame(int);
  
  bool GetFrame(int frame_index, void* buffer);
  bool GetField(int field_index, void* buffer);

private:
  ImageSequenceMovieFilePrivates* p;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS ImageSequenceMovieFile.

