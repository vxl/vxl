//-*- c++ -*-------------------------------------------------------------------
#ifndef ImageSequenceName_h_
#define ImageSequenceName_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : ImageSequenceName
//
// .SECTION Description
//    ImageSequenceName is a class that awf hasn't documented properly. FIXME
//
// .NAME        ImageSequenceName - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/ImageSequenceName.h
// .FILE        ImageSequenceName.h
// .FILE        ImageSequenceName.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Dec 98
//
//-----------------------------------------------------------------------------

#include "SequenceFileName.h"

struct ImageSequenceName : public SequenceFileName {
  ImageSequenceName(char const* s, char const* read_or_write = "r", char const* default_extension = 0);
  ImageSequenceName(char const* s, int start_frame, int step, char const* read_or_write = "r", char const* default_extension = 0);
  void init(char const* read_or_write = "r", char const* default_extension = 0);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS ImageSequenceName.

