// This is core/vidl/vidl_avicodec.h
#ifndef vidl_avicodec_h
#define vidl_avicodec_h
//:
// \file 
// \author Andy Molnar
// \date October 1998
//
// \verbatim
//  Modifications
//   Nicolas Dano, september 1999
//     - Transformed the basic AVI image reading into AVI Video
//     - Added the possibility to save video in AVI format
//   Julien ESTEVE, June 2000
//     Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   10/7/2003 Matt Leotta (Brown) Converted vil1 to vil
// \endverbatim

#include <vcl_compiler.h>
#include <vidl/vidl_codec.h>
#include <vidl/vidl_avicodec_sptr.h>
#include <vidl/vidl_frame_sptr.h>
#if defined(VCL_VC) || defined(VCL_BORLAND)
# include <windows.h>
# undef min
# undef max
# include <vfw.h>
#endif


//: Allows user to load Microsoft AVI movie files as vxl video.
// (Works only if compiled with a microsoft compiler)
// See also vidl_codec
class vidl_avicodec : public vidl_codec
{
 public:
  vidl_avicodec();
 ~vidl_avicodec();

  //-----------------------------------------------------

  virtual vil_image_view_base_sptr  get_view(int position, 
                                             int x0, int xs, 
                                             int y0, int ys ) const;
  virtual bool put_view(int position, 
                        const vil_image_view_base &im, 
                        int x0, int y0 );

  //-----------------------------------------------------
  virtual bool probe(const char* fname);
  virtual vidl_codec_sptr load(const char* fname, char mode = 'r' );
  virtual bool save(vidl_movie* movie, const char* fname);
  virtual  const char* type() {return "AVI";}
  virtual vidl_avicodec* castto_vidl_avicodec(){return this;}

  // Set of encoders that this class knows how to configure by itself,
  // without having to open a windows dialog in which the user
  // has to select the encoder.
  enum AVIEncoderType { ASKUSER, USEPREVIOUS, UNCOMPRESSED, CINEPACK };

  /// Type of encoder to use.
  AVIEncoderType encoder_type;

  /// Video for windows compressor options data structure.
  AVICOMPRESSOPTIONS opts;

  bool encoder_options_valid;

  // This function sets the encoder that is internally used to create the
  // AVI. Using this function avoids the windows dialog asking
  // the user for the compressor.
  // Depending on the choosen encoder, the parameters of opts are set by this
  // function.
  void choose_encoder(AVIEncoderType encoder);

 private:
   PAVIFILE avi_file_;
   PAVISTREAM avi_stream_;
   PGETFRAME avi_get_frame_;

   AVIFILEINFO avi_file_info_;
   AVISTREAMINFO avi_stream_info_;

  // Helpers
  HANDLE  make_dib(vidl_frame_sptr frame, UINT bits);

  bool load_avi(const char* fname, char mode);

protected:
   bool read_header();
   bool write_header();
};


#endif // vidl_avicodec_h
