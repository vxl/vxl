//-*- c++ -*-------------------------------------------------------------------
#ifndef POX_h_
#define POX_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : POX
//
// .SECTION Description
//    POX is a collection of functions which are waiting for namespaces.
//
// .NAME        POX - Oxford image processing
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/POX.h
// .FILE        POX.h
// .FILE        POX.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 13 Jul 98
//
//-----------------------------------------------------------------------------

#include <vil/vil_memory_image_of.h>

struct MedianReduceImage : public vil_memory_image_of<unsigned char> {
  MedianReduceImage(vil_memory_image_of<unsigned char> const& in, int SCALE);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS POX.

