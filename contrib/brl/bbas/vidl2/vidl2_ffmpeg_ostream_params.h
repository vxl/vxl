// This is brl/bbas/vidl2/vidl2_ffmpeg_ostream_params.h
#ifndef vidl2_ffmpeg_ostream_params_h_
#define vidl2_ffmpeg_ostream_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A parameters struct for vidl2_ffmpeg_ostream
//
// \author Matt Leotta
// \date 3 Jan 2006
//
// \verbatim
//  Modifications
//   Matt Leotta   3 Jan 2006   Adapted from code by Amitha Perera
// \endverbatim

#include <vcl_string.h>

//: A parameters struct for vidl2_ffmpeg_ostream
//
// The setter functions are provided for convenience, so that the
// parameters can be set through an anonynous object.
// \code
//   ostream.open( filename,
//                 vidl2_ffmpeg_ostream_params()
//                 .encoder( vidl2_ffmpeg_ostream_params::DVVIDEO )
//                 .frame_rate( 29.95 ) );
// \endcode
struct vidl2_ffmpeg_ostream_params
{
  //: Available video encoders
  enum encoder_type { DEFAULT,
                      MPEG4,
                      MSMPEG4V2,
                      MPEG2VIDEO,
                      DVVIDEO,
                      LJPEG,
                      RAWVIDEO,
                      HUFFYUV,
                      ENCODER_END_MARKER };

  //: Supported video types
  enum file_format_type { GUESS // < Guess based on file extension
    };

  //: The type of encoder to use (default DEFAULT)
  encoder_type encoder_;

  //: The file format to use (default GUESS)
  file_format_type file_format_;

  //: Frame rate in frames/second. (default 25)
  float frame_rate_;

  //: Bit rate in bits/second. (Default 5000)
  unsigned bit_rate_;

  //: Width of a frame (default 640)
  unsigned ni_;

  //: Height of a frame (default 480)
  unsigned nj_;

  //: static parameters from ffmpeg.c
  // There are many other FFMPEG parameters available
  // I don't know what most of these do, but using the
  // default values should work most of the time
  int video_bit_rate_tolerance_;
  float frame_aspect_ratio_;
  int intra_only_;
  int gop_size_;
  float video_qscale_;
  int same_quality_;
  int mb_decision_;
  int mb_cmp_;
  int ildct_cmp_;
  int sub_cmp_;
  int cmp_;
  int pre_cmp_;
  int pre_me_;
  float lumi_mask_;
  float dark_mask_;
  float scplx_mask_;
  float tcplx_mask_;
  float p_mask_;
  int qns_;
  int use_4mv_;
  int use_obmc_;
  int use_loop_;
  int use_aic_;
  int use_aiv_;
  int use_umv_;
  int use_ss_;
  int use_alt_scan_;
  int use_trell_;
  int use_scan_offset_;
  int use_part_;
  int closed_gop_;
  int use_qpel_;
  int use_qprd_;
  int use_cbprd_;
  int b_frames_;
  int do_interlace_dct_;
  int do_interlace_me_;
  int video_qmin_;
  int video_qmax_;
  int video_lmin_;
  int video_lmax_;
  int video_mb_qmin_;
  int video_mb_qmax_;
  int video_qdiff_;
  float video_qblur_;
  float video_qcomp_;
  vcl_string video_rc_eq_;
  int debug_;
  int debug_mv_;
  int video_rc_buffer_size_;
  float video_rc_buffer_aggressivity_;
  int video_rc_max_rate_;
  int video_rc_min_rate_;
  float video_rc_initial_cplx_;
  float video_b_qfactor_;
  float video_b_qoffset_;
  float video_i_qfactor_;
  float video_i_qoffset_;
  int video_intra_quant_bias_;
  int video_inter_quant_bias_;
  int dct_algo_;
  int idct_algo_;
  int me_threshold_;
  int mb_threshold_;
  int intra_dc_precision_;
  int strict_;
  int error_rate_;
  int noise_reduction_;
  int sc_threshold_;
  int coder_;
  int context_;
  int predictor_;
#if 0
  // not supported by older versions of FFMPEG
  int video_profile_;
  int video_level_;
#endif
  int me_range_;
  int do_psnr_;
  int packet_size_;
  int me_method_;
  int do_pass_;

  //-------------------------------------------------------

  //: Construct to default values
  vidl2_ffmpeg_ostream_params();

  //: Set the file format
  vidl2_ffmpeg_ostream_params& file_format( file_format_type t )
  { file_format_ = t; return *this; }

  //: Set the video encoder
  vidl2_ffmpeg_ostream_params& encoder( encoder_type t )
  { encoder_ = t; return *this; }

  //: Set the frame rate
  vidl2_ffmpeg_ostream_params& frame_rate( float r )
  { frame_rate_ = r; return *this; }

  //: Set the bit rate
  vidl2_ffmpeg_ostream_params& bit_rate( unsigned r )
  { bit_rate_ = r; return *this; }

  //: Set the size of the frames
  vidl2_ffmpeg_ostream_params& size( unsigned ni, unsigned nj )
  { ni_ = ni; nj_ = nj; return *this; }
};

#endif // vidl2_ffmpeg_ostream_params_h_
