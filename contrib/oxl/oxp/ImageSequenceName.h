#ifndef ImageSequenceName_h_
#define ImageSequenceName_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	ImageSequenceName
// .LIBRARY	POX
// .HEADER	Oxford Package
// .INCLUDE	oxp/ImageSequenceName.h
// .FILE	ImageSequenceName.cxx
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

#endif // ImageSequenceName_h_
