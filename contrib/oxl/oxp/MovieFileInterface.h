// This is oxl/oxp/MovieFileInterface.h
#ifndef MovieFileInterface_h_
#define MovieFileInterface_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 30 Dec 98
//
//-----------------------------------------------------------------------------

#include <vil/vil_image.h>

struct MovieFileInterface
{
  virtual ~MovieFileInterface();

  //: Return length of movie in frames
  virtual int GetLength() = 0;

  //: Return size of the frame \argfont{frame_index}.
  virtual int GetSizeX(int frame_index) = 0;
  virtual int GetSizeY(int frame_index) = 0;

  //: Return bits per pixel of images.
  // Assumed constant throughout the movie.
  virtual int GetBitsPixel() = 0;

  //: Return true if frames are stored as a pair of interlaced fields.
  virtual bool IsInterlaced() = 0;

  //: Return true if frame_index is a valid frame.
  virtual bool HasFrame(int frame_index) = 0;

  //: Grab frame into supplied buffer, which is GetBitsPixel * SizeX * SizeY
  virtual bool GetFrame(int frame_index, void* buffer);

  //: Grab field into supplied buffer, which is GetBitsPixel * SizeX * SizeY
  virtual bool GetField(int field_index, void* buffer);

  virtual vil_image GetImage(int frame_index) = 0;

  //: General verbosity flag for various movie-related operations.
  static bool verbose;
};

#endif // MovieFileInterface_h_
