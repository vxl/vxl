#ifndef vidl_avicodec_h
#define vidl_avicodec_h

//:
// \file 
// \author Andy Molnar
// \date October 1998
//
// \verbatim
// Modifications
// Nicolas Dano, september 1999
//     - Transformed the basic AVI image reading into AVI Video
//     - Added the possibility to save video in AVI format
// Julien ESTEVE, June 2000
//     Ported from TargetJr
// 10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vcl_compiler.h>
#include <vidl/vidl_avicodec_sptr.h>
#ifdef VCL_VC
#include <windows.h>
#include <vfw.h>
#endif
#include <vidl/vidl_codec.h>
#include <vidl/vidl_frame_sptr.h>


//: Allows user to load Microsoft AVI movie files as vxl video.
// (Works only if compiled with a microsoft compiler)
// See also vidl_codec
class vidl_avicodec : public vidl_codec
{
public:
  vidl_avicodec();
  ~vidl_avicodec();

  //-----------------------------------------------------

  virtual bool   get_section(int position, void* ib, int x0, int y0, int xs, int ys) const;
  virtual int    put_section(int position, void* ib, int x0, int y0, int xs, int ys);

  //-----------------------------------------------------
  virtual bool probe(const char* fname);
  virtual vidl_codec_sptr load(const char* fname, char mode = 'r' );
  virtual bool save(vidl_movie* movie, const char* fname);
  virtual  const char* type() {return "AVI";}

private:
   PAVIFILE avi_file_;
   PAVISTREAM avi_stream_;
   PGETFRAME avi_get_frame_;

   AVIFILEINFO avi_file_info_;
   AVISTREAMINFO avi_stream_info_;

  // Helpers
  HANDLE  make_dib(vidl_frame_sptr frame, UINT bits);

protected:
   bool read_header();
   bool write_header();
};

#endif // vidl_avicodec_h
