//-*- c++ -*-------------------------------------------------------------------
#ifndef ImageWarp_h_
#define ImageWarp_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : ImageWarp
//
// .SECTION Description
//    ImageWarp is a class that awf hasn't documented properly. FIXME
//
// .NAME        ImageWarp - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/ImageWarp.h
// .FILE        ImageWarp.h
// .FILE        ImageWarp.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 23 Jan 99
//
//-----------------------------------------------------------------------------

#include <vil/vil_memory_image_of.h>
#include <oxp/Mapping_2d_2d.h>

template <class PixelType>
class ImageWarp {
  
  ImageWarp();
public:
  // Constructors/Destructors--------------------------------------------------

  // -- The type of the return value of "bilinear" and "bicubic", which
  // will be a floating point version of the pixel type.
  typedef typename vnl_numeric_traits<PixelType>::real_t real_t;

  static void mean_nz_intensity(const vil_memory_image_of<PixelType>& in, int x, int y, int window_size, 
#if defined(VCL_GCC_27)
				double *out,
#else
				real_t* out, 
#endif
				int * nnzp = 0);
  static void gapfill(vil_memory_image_of<PixelType>& out, int ngaps);
  static void warp(Mapping_2d_2d& map, const vil_memory_image_of<PixelType>& in, vil_memory_image_of<PixelType>& out);
  static void warp_inverse(Mapping_2d_2d& map, const vil_memory_image_of<PixelType>& in, vil_memory_image_of<PixelType>& out);
    

protected:
  // Data Members--------------------------------------------------------------
  
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS ImageWarp.

