#ifndef MedianReduceImage_h_
#define MedianReduceImage_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    MedianReduceImage - Oxford image processing
// .LIBRARY oxp
// .HEADER  Oxford Package
// .INCLUDE oxp/MedianReduceImage.h
// .FILE    MedianReduceImage.cxx
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

#endif // MedianReduceImage_h_
