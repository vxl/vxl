// This is core/vidl1/vidl1_win_avicodec.h
#ifndef vidl1_win_avicodec_h
#define vidl1_win_avicodec_h
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
//   05/11/2003 Matt Leotta (Brown) Moved vidl1_avicodec to vidl1_win_avicodec
// \endverbatim

#include <vcl_compiler.h>
#include <vidl1/vidl1_codec.h>
#include <vidl1/vidl1_frame_sptr.h>
#ifdef VCL_WIN32
# include <windows.h>
# undef min
# undef max
# include <vfw.h>
#endif


//: Allows user to load Microsoft AVI movie files as vxl video.
// (Works only if compiled with a microsoft compiler)
// See also vidl1_codec
class vidl1_avicodec : public vidl1_codec
{
 public:
  vidl1_avicodec();
 ~vidl1_avicodec();

  //-----------------------------------------------------

  virtual vil_image_view_base_sptr  get_view(int position,
                                             int x0, int xs,
                                             int y0, int ys ) const;
  virtual bool put_view(int position,
                        const vil_image_view_base &im,
                        int x0, int y0 );

  //-----------------------------------------------------
  virtual bool probe(vcl_string const& fname);
  virtual vidl1_codec_sptr load(vcl_string const& fname, char mode = 'r' );
  virtual bool save(vidl1_movie* movie, vcl_string const& fname);
  virtual vcl_string type() const { return "AVI"; }
  virtual vidl1_avicodec* castto_vidl1_avicodec() { return this; }

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
  // Depending on the chosen encoder, the parameters of opts are set by this
  // function.
  void choose_encoder(AVIEncoderType encoder);

 private:
  PAVIFILE avi_file_;
  PAVISTREAM avi_stream_;
  PGETFRAME avi_get_frame_;

  AVIFILEINFO avi_file_info_;
  AVISTREAMINFO avi_stream_info_;

  // Helpers
  HANDLE  make_dib(vidl1_frame_sptr frame, UINT bits);

  bool load_avi(vcl_string const& fname, char mode);

 protected:
  bool read_header();
  bool write_header();
};

#endif // vidl1_win_avicodec_h
