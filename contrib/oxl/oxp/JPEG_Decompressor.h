// This is oxl/oxp/JPEG_Decompressor.h
#ifndef JPEG_Decompressor_h_
#define JPEG_Decompressor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME    JPEG_Decompressor - Simple interface to JPEG library
// .LIBRARY oxp
// .HEADER  Oxford Package
// .INCLUDE oxp/JPEG_Decompressor.h
// .FILE    JPEG_Decompressor.cxx
//
// .SECTION Description
//    JPEG_Decompressor is a simplified interface to the JPEG library.
//    Initialize using
//      JPEG_Decompressor jpeg_decompressor(some_ifstream);
//    and read rows using
//      char* row_ptr = (char*)jpeg_decompressor.GetNextScanLine();
//
//    If you need to read a sequence of concatenated JPEGS from a file,
//    you should keep the decompressor open, as it buffers its reads,
//    so the file pointer will not necessarily be pointing to the end
//    of the jpeg.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 30 Dec 98
//
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

struct JPEG_DecompressorPrivates;
class JPEG_Decompressor
{
 public:
#if defined(VCL_SGI_CC) || defined(VCL_SUNPRO_CC) || (defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3))
  JPEG_Decompressor(vcl_ifstream& s);
#endif
  JPEG_Decompressor(int fd);
  ~JPEG_Decompressor();

  int width();
  int height();
  int GetBitsPixel();

  void* GetNextScanLine();

  unsigned long GetFilePosition();

  void StartNextJPEG();

 private:
  JPEG_DecompressorPrivates* pd;
  void init(int fd);
};

#endif // JPEG_Decompressor_h_
