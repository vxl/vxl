// This is oxl/oxp/ImageSequenceMovieFile.h
#ifndef ImageSequenceMovieFile_h_
#define ImageSequenceMovieFile_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
//  ImageSequenceMovieFile is a subclass of MovieFileInterface that
//  reads from a sequence of images.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Dec 98
//
//-----------------------------------------------------------------------------

#include <oxp/MovieFileInterface.h>

struct ImageSequenceMovieFilePrivates;

//: subclass of MovieFileInterface that reads from a sequence of images
class ImageSequenceMovieFile : public MovieFileInterface
{
  ImageSequenceMovieFilePrivates* p;
 public:
  ImageSequenceMovieFile(char const* filepattern, int frame_index_to_search_for_extension);
  ~ImageSequenceMovieFile();

  int GetLength();
  int GetSizeX(int);
  int GetSizeY(int);
  int GetBitsPixel();
  bool IsInterlaced();
  bool HasFrame(int);
  vil_image GetImage(int);

  bool GetFrame(int frame_index, void* buffer);
  bool GetField(int field_index, void* buffer);
};

#endif // ImageSequenceMovieFile_h_
