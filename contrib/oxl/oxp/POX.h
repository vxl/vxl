#ifndef POX_h_
#define POX_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    POX - Oxford image processing
// .LIBRARY oxp
// .HEADER  Oxford Package
// .INCLUDE oxp/POX.h
// .FILE    POX.cxx
// .SECTION Description
//    POX is a collection of functions which are waiting for namespaces.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 13 Jul 98
//
//-----------------------------------------------------------------------------

#include <vil/vil_memory_image_of.h>

struct MedianReduceImage : public vil_memory_image_of<unsigned char> {
  MedianReduceImage(vil_memory_image_of<unsigned char> const& in, int SCALE);
};

#endif // POX_h_
