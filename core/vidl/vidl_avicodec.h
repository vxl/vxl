#ifndef vidl_avicodec_h
#define vidl_avicodec_h
//-----------------------------------------------------------------------------
//
// .NAME vidl_avicodec
// .LIBRARY vidl
// .HEADER vidl package
// .INCLUDE vidl/vidl_avicodec.h
// .FILE vidl_avicodec.cxx
//
// .SECTION Description
//    Allows user to load Microsoft AVI movie files as vxl video.
//    (Works only if compiled with a microsoft compiler)
//
// .SECTION See also
//   vidl_codec
//
// .SECTION Author
//   Andy Molnar, october 1998
//
// .SECTION Modifications
//   Nicolas Dano, september 1999
//     - Transformed the basic AVI image reading into AVI Video
//     - Added the possibility to save video in AVI format
//   Julien ESTEVE, June 2000
//     Ported from TargetJr
//-----------------------------------------------------------------------------

#include <vidl/vidl_avicodec_sptr.h>
#include <vbl/vbl_ref_count.h>

#include <windows.h>
#include <vfw.h>

#include <vidl/vidl_codec.h>
#include <vidl/vidl_frame_sptr.h>


//=============================================================================
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
