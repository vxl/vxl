// This is core/vidl/vidl_ffmpeg_ostream_v56.hxx
#ifndef vidl_ffmpeg_ostream_v56_hxx_
#define vidl_ffmpeg_ostream_v56_hxx_
#pragma once

#include <cstring>
#include <utility>
#include "vidl_ffmpeg_ostream.h"
//:
// \file
// \author Johan Andruejol
// \author Gehua Yang
// \author Matt Leotta
// \author Amitha Perera
// \author David Law
// \date   6 April 2015
//
// Update implementation based on FFMPEG release version 2.8.4
//
//-----------------------------------------------------------------------------

#include "vidl_ffmpeg_init.h"
#include "vidl_ffmpeg_convert.h"
#include "vidl_ffmpeg_pixel_format.h"
#include "vidl_frame.h"
#include "vidl_convert.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <climits>
#include <vil/vil_memory_chunk.h>

extern "C" {
#if FFMPEG_IN_SEVERAL_DIRECTORIES
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>

// https://stackoverflow.com/questions/46884682/error-in-building-opencv-with-ffmpeg
#define CODEC_FLAG_GLOBAL_HEADER AV_CODEC_FLAG_GLOBAL_HEADER
#define CODEC_FLAG_QSCALE AV_CODEC_FLAG_QSCALE
#define CODEC_FLAG_4MV AV_CODEC_FLAG_4MV
#define CODEC_FLAG_LOOP_FILTER AV_CODEC_FLAG_LOOP_FILTER

#define CODEC_FLAG_CLOSED_GOP AV_CODEC_FLAG_CLOSED_GOP
#define CODEC_FLAG_QPEL       AV_CODEC_FLAG_QPEL
#define CODEC_FLAG_INTERLACED_DCT AV_CODEC_FLAG_INTERLACED_DCT
#define CODEC_FLAG_INTERLACED_ME AV_CODEC_FLAG_INTERLACED_ME
#define CODEC_FLAG_PSNR AV_CODEC_FLAG_PSNR
#define CODEC_FLAG_PASS1 AV_CODEC_FLAG_PASS1
#define CODEC_FLAG_PASS2 AV_CODEC_FLAG_PASS2

#else
#include <ffmpeg/avformat.h>
#include <ffmpeg/opt.h>
#endif
}

//-----------------------------------------------------------------------------


struct vidl_ffmpeg_ostream::pimpl
{
  pimpl()
  : fmt_cxt_( nullptr ),
  file_opened_( false ),
  codec_opened_( false ),
  cur_frame_( 0 ),
  video_rc_eq_( nullptr )
  { }

  AVFormatContext* fmt_cxt_;
  bool file_opened_;
  bool codec_opened_;
  unsigned int cur_frame_;
  char* video_rc_eq_;
};


//-----------------------------------------------------------------------------


//: Constructor
vidl_ffmpeg_ostream::
vidl_ffmpeg_ostream()
  : os_( new vidl_ffmpeg_ostream::pimpl )
{
  vidl_ffmpeg_init();
}


//: Destructor
vidl_ffmpeg_ostream::
~vidl_ffmpeg_ostream()
{
  close();
  delete os_;
}


//: Constructor - opens a stream
vidl_ffmpeg_ostream::
vidl_ffmpeg_ostream(const std::string  & filename,
                    const vidl_ffmpeg_ostream_params  & params)
  : os_( new vidl_ffmpeg_ostream::pimpl ),
    filename_(std::move(filename)), params_(std::move(params))
{
  vidl_ffmpeg_init();
}


//: Open the stream
bool
vidl_ffmpeg_ostream::
open()
{
  // Close any open files
  close();

  os_->fmt_cxt_ = avformat_alloc_context();

  AVOutputFormat* file_oformat = nullptr;
  if ( params_.file_format_ == vidl_ffmpeg_ostream_params::GUESS ) {
    file_oformat = av_guess_format(nullptr, filename_.c_str(), nullptr);
    if (!file_oformat) {
      std::cerr << "ffmpeg: Unable for find a suitable output format for "
               << filename_ << '\n';
      close();
      return false;
    }
  }
  else {
    close();
    return false;
  }

  os_->fmt_cxt_->oformat = file_oformat;
  os_->fmt_cxt_->nb_streams = 0;

  // Create stream
  AVStream* st = avformat_new_stream( os_->fmt_cxt_, nullptr );
  if ( !st ) {
    std::cerr << "ffmpeg: could not alloc stream\n";
    close();
    return false;
  }

  //os_->fmt_cxt_->nb_streams = 1;

  AVCodecContext *video_enc = st->codec;

  if (std::strcmp(file_oformat->name, "mp4") != 0 ||
      std::strcmp(file_oformat->name, "mov") != 0 ||
      std::strcmp(file_oformat->name, "3gp") != 0 )
    video_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;

  video_enc->codec_type = AVMEDIA_TYPE_VIDEO;

  switch ( params_.encoder_ )
  {
   case vidl_ffmpeg_ostream_params::DEFAULT:
    video_enc->codec_id = file_oformat->video_codec;
    break;
   case vidl_ffmpeg_ostream_params::MPEG4:
    video_enc->codec_id = AV_CODEC_ID_MPEG4;
    break;
   case vidl_ffmpeg_ostream_params::MPEG2VIDEO:
    video_enc->codec_id = AV_CODEC_ID_MPEG2VIDEO;
    break;
   case vidl_ffmpeg_ostream_params::MSMPEG4V2:
    video_enc->codec_id = AV_CODEC_ID_MSMPEG4V2;
    break;
   case vidl_ffmpeg_ostream_params::RAWVIDEO:
    video_enc->codec_id = AV_CODEC_ID_RAWVIDEO;
    break;
   case vidl_ffmpeg_ostream_params::LJPEG:
    video_enc->codec_id = AV_CODEC_ID_LJPEG;
    break;
   case vidl_ffmpeg_ostream_params::HUFFYUV:
    video_enc->codec_id = AV_CODEC_ID_HUFFYUV;
    break;
   case vidl_ffmpeg_ostream_params::DVVIDEO:
    video_enc->codec_id = AV_CODEC_ID_DVVIDEO;
    break;
   default:
    std::cout << "ffmpeg: Unknown encoder type\n";
    return false;
  }

  AVCodec* codec = avcodec_find_encoder( video_enc->codec_id );
  if ( !codec )
  {
    std::cerr << "ffmpeg_writer:: couldn't find encoder for " << video_enc->codec_id << '\n';
    return false;
  }

  video_enc->bit_rate = params_.bit_rate_ * 1000;
  video_enc->bit_rate_tolerance = params_.video_bit_rate_tolerance_;
  video_enc->time_base.num = 1000;
  video_enc->time_base.den = int(params_.frame_rate_*1000);

  if ( codec && codec->supported_framerates )
  {
    AVRational const* p = codec->supported_framerates;
    AVRational req = { video_enc->time_base.den, video_enc->time_base.num };
    AVRational const* best = nullptr;
    AVRational best_error = { INT_MAX, 1 };
    for (; p->den!=0; p++)
    {
      AVRational error = av_sub_q(req, *p);
      if ( error.num < 0 )   error.num *= -1;
      if ( av_cmp_q( error, best_error ) < 0 )
      {
        best_error= error;
        best= p;
      }
    }
    video_enc->time_base.den= best->num;
    video_enc->time_base.num= best->den;
  }

  video_enc->width  = params_.ni_;
  video_enc->height = params_.nj_;
  video_enc->sample_aspect_ratio = av_d2q(params_.frame_aspect_ratio_*params_.ni_/params_.nj_, 255);

  // Our source is packed RGB. Use that if possible.
  video_enc->pix_fmt = AV_PIX_FMT_RGB24;
  if ( codec && codec->pix_fmts )
  {
    const enum AVPixelFormat* p= codec->pix_fmts;
    for ( ; *p != -1; p++ )
    {
      if ( *p == video_enc->pix_fmt )
        break;
    }
    if ( *p == -1 )
      video_enc->pix_fmt = codec->pix_fmts[0];
  }
  else if ( codec && ( codec->id == AV_CODEC_ID_RAWVIDEO ||
                      codec->id == AV_CODEC_ID_HUFFYUV ) )
  {
    // these formats only support the YUV input image formats
    video_enc->pix_fmt = AV_PIX_FMT_YUV420P;
  }

  if (!params_.intra_only_)
    video_enc->gop_size = params_.gop_size_;
  else
    video_enc->gop_size = 0;
  if (params_.video_qscale_ || params_.same_quality_)
  {
    video_enc->flags |= CODEC_FLAG_QSCALE;
    video_enc->global_quality = FF_QP2LAMBDA * params_.video_qscale_;
  }
  // if (bitexact)
  //   video_enc->flags |= CODEC_FLAG_BITEXACT;

  video_enc->mb_decision = params_.mb_decision_;
  video_enc->mb_cmp = params_.mb_cmp_;
  video_enc->ildct_cmp = params_.ildct_cmp_;
  video_enc->me_sub_cmp = params_.sub_cmp_;
  video_enc->me_cmp = params_.cmp_;
  video_enc->me_pre_cmp = params_.pre_cmp_;
  video_enc->pre_me = params_.pre_me_;
  video_enc->lumi_masking = params_.lumi_mask_;
  video_enc->dark_masking = params_.dark_mask_;
  video_enc->spatial_cplx_masking = params_.scplx_mask_;
  video_enc->temporal_cplx_masking = params_.tcplx_mask_;
  video_enc->p_masking = params_.p_mask_;
  // This parameter is no longer supported
  //video_enc->quantizer_noise_shaping= params_.qns_;

  if (params_.use_umv_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_umv_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_H263P_UMV;
  }
  if (params_.use_ss_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_ss_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_H263P_SLICE_STRUCT;
  }
  if (params_.use_aiv_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_aiv_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_H263P_AIV;
  }
  if (params_.use_4mv_)
  {
    video_enc->flags |= CODEC_FLAG_4MV;
  }
  if (params_.use_obmc_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_obmc_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_OBMC;
  }
  if (params_.use_loop_)
  {
    video_enc->flags |= CODEC_FLAG_LOOP_FILTER;
  }

  if (params_.use_part_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_part_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_PART;
  }
  if (params_.use_alt_scan_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_alt_scan_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_ALT_SCAN;
  }
  if (params_.use_scan_offset_)
  {
    // This flag is no longer supported
    std::cerr << "WARNING: global parameter 'use_scan_offset_' no longer support by FFMPEG" << std::endl;
    //video_enc->flags |= CODEC_FLAG_SVCD_SCAN_OFFSET;
  }
  if (params_.closed_gop_)
  {
    video_enc->flags |= CODEC_FLAG_CLOSED_GOP;
  }
  if (params_.use_qpel_)
  {
    video_enc->flags |= CODEC_FLAG_QPEL;
  }
  //if (params_.use_qprd_)
  //{
  //  video_enc->flags |= CODEC_FLAG_QP_RD;
  //}
  //if (params_.use_cbprd_)
  //{
  //  video_enc->flags |= CODEC_FLAG_CBP_RD;
  //}
  if (params_.b_frames_)
  {
    video_enc->max_b_frames = params_.b_frames_;
    video_enc->b_frame_strategy = 0;
    video_enc->b_quant_factor = 2.0;
  }
  if (params_.do_interlace_dct_)
  {
    video_enc->flags |= CODEC_FLAG_INTERLACED_DCT;
  }
  if (params_.do_interlace_me_)
  {
    video_enc->flags |= CODEC_FLAG_INTERLACED_ME;
  }
  video_enc->qmin = params_.video_qmin_;
  video_enc->qmax = params_.video_qmax_;
  av_opt_set_int(video_enc, "lmin", params_.video_lmin_, AV_OPT_SEARCH_CHILDREN);
  av_opt_set_int(video_enc, "lmax", params_.video_lmax_, AV_OPT_SEARCH_CHILDREN);
  video_enc->max_qdiff = params_.video_qdiff_;
  video_enc->qblur = params_.video_qblur_;
  video_enc->qcompress = params_.video_qcomp_;

  // delete when the stream is closed
  os_->video_rc_eq_ = new char[params_.video_rc_eq_.length() + 1];
  av_opt_set(video_enc, "video_rc_eq_", params_.video_rc_eq_.c_str(), AV_OPT_SEARCH_CHILDREN);

  video_enc->debug = params_.debug_;
  video_enc->debug_mv = params_.debug_mv_;
  video_enc->thread_count = 1;

  video_enc->rc_max_rate = params_.video_rc_max_rate_;
  video_enc->rc_min_rate = params_.video_rc_min_rate_;
  av_opt_set_double(video_enc, "rc_buffer_aggressivity", params_.video_rc_buffer_aggressivity_, AV_OPT_SEARCH_CHILDREN);
  av_opt_set_double(video_enc, "rc_initial_cplx", params_.video_rc_initial_cplx_, AV_OPT_SEARCH_CHILDREN);
  video_enc->i_quant_factor = params_.video_i_qfactor_;
  video_enc->b_quant_factor = params_.video_b_qfactor_;
  video_enc->i_quant_offset = params_.video_i_qoffset_;
  video_enc->b_quant_offset = params_.video_b_qoffset_;
  //DEPRECATED https://www.ffmpeg.org/doxygen/3.1/structAVCodecContext.html video_enc->intra_quant_bias = params_.video_intra_quant_bias_;
  //DEPRECATED https://www.ffmpeg.org/doxygen/3.1/structAVCodecContext.html video_enc->inter_quant_bias = params_.video_inter_quant_bias_;
  video_enc->dct_algo = params_.dct_algo_;
  video_enc->idct_algo = params_.idct_algo_;
  video_enc->intra_dc_precision = params_.intra_dc_precision_ - 8;
  video_enc->strict_std_compliance = params_.strict_;
  av_opt_set_double(video_enc, "error_rate", params_.error_rate_, AV_OPT_SEARCH_CHILDREN);
  video_enc->noise_reduction = params_.noise_reduction_;
  video_enc->scenechange_threshold = params_.sc_threshold_;
  video_enc->me_range = params_.me_range_;
  video_enc->coder_type = params_.coder_;
  video_enc->context_model = params_.context_;
  video_enc->prediction_method = params_.predictor_;

  if (params_.do_psnr_)
    video_enc->flags|= CODEC_FLAG_PSNR;

  //DEPRECATED:  This option does nothing "https://www.ffmpeg.org/doxygen/3.1/structAVCodecContext.html#aa71b3450f1a508330e907db117ae410e" video_enc->me_method = params_.me_method_;

  // two pass mode
  if (params_.do_pass_)
  {
    if (params_.do_pass_ == 1)
    {
      video_enc->flags |= CODEC_FLAG_PASS1;
    }
    else
    {
      video_enc->flags |= CODEC_FLAG_PASS2;
    }
  }

  std::strncpy( os_->fmt_cxt_->filename, filename_.c_str(), 1023 );

  if ( avio_open( &os_->fmt_cxt_->pb, filename_.c_str(), AVIO_FLAG_WRITE ) < 0 )
  {
    std::cerr << "ffmpeg: couldn't open " << filename_ << " for writing\n";
    close();
    return false;
  }
  os_->file_opened_ = true;

  //dump_format( os_->fmt_cxt_, 1, filename_, 1 );

  if ( avcodec_open2( video_enc, codec, nullptr ) < 0 )
  {
    std::cerr << "ffmpeg: couldn't open codec\n";
    close();
    return false;
  }
  os_->codec_opened_ = true;

  if ( avformat_write_header( os_->fmt_cxt_, nullptr ) < 0 )
  {
    std::cerr << "ffmpeg: couldn't write header\n";
    close();
    return false;
  }

  return true;
}


//: Close the stream
void
vidl_ffmpeg_ostream::
close()
{
  //delete os_->video_rc_eq_;
  //os_->video_rc_eq_ = NULL;

  if ( os_->fmt_cxt_ ) {

    // flush out remaining packets refer to
    //   http://stackoverflow.com/questions/17816532/creating-a-video-from-images-using-ffmpeg-libav-and-libx264
    AVCodecContext* codec = os_->fmt_cxt_->streams[0]->codec;
    int got_packet = 1;
    while (got_packet)
    {
      AVPacket pkt;
      av_init_packet(&pkt);
      pkt.data = nullptr;
      pkt.size = 0;
      pkt.stream_index = 0;

      int ret = avcodec_encode_video2(codec, &pkt, nullptr, &got_packet);

      if (ret < 0)
      {
        std::cerr << "FFMPEG video encoding failed" << std::endl;
        break;
      }
      if (got_packet) {
        if (codec->coded_frame) {
          pkt.pts = codec->coded_frame->pts;
        }
        if (codec->coded_frame && codec->coded_frame->key_frame) {
          pkt.flags |= AV_PKT_FLAG_KEY;
        }
        av_interleaved_write_frame(os_->fmt_cxt_, &pkt);
        av_packet_unref(&pkt);

      }
    }

    if ( os_->file_opened_ ) {
      av_write_trailer( os_->fmt_cxt_ );
      avio_close( os_->fmt_cxt_->pb );
      os_->file_opened_ = false;
    }

    if ( os_->fmt_cxt_->nb_streams > 0 ) {
      if ( os_->codec_opened_ ) {
        for ( unsigned i = 0; i < os_->fmt_cxt_->nb_streams; ++i ) {
          AVCodecContext* codec = os_->fmt_cxt_->streams[i]->codec;
          if ( codec->stats_in ) {
            av_freep( codec->stats_in );
          }
          avcodec_close( codec );
        }
      }
      os_->codec_opened_ = false;
      for ( unsigned i = 0; i < os_->fmt_cxt_->nb_streams; ++i ) {
        av_free( os_->fmt_cxt_->streams[i] );
      }
    }

    av_free( os_->fmt_cxt_ );
    os_->fmt_cxt_ = nullptr;
  }
}


//: Return true if the stream is open for writing
bool
vidl_ffmpeg_ostream::
is_open() const
{
  return os_->file_opened_;
}


//: Write and image to the stream
// \retval false if the image could not be written
bool
vidl_ffmpeg_ostream::
write_frame(const vidl_frame_sptr& frame)
{
  if (!is_open()) {
    // resize to the first frame
    params_.size(frame->ni(),frame->nj());
    open();
  }

  AVCodecContext* codec = os_->fmt_cxt_->streams[0]->codec;

  if (unsigned( codec->width ) != frame->ni() ||
      unsigned( codec->height) != frame->nj() ) {
    std::cerr << "ffmpeg: Input image has wrong size. Expecting ("
             << codec->width << 'x' << codec->height << "), got ("
             << frame->ni() << 'x' << frame->nj() << ")\n";
    return false;
  }

  AVPixelFormat fmt = vidl_pixel_format_to_ffmpeg(frame->pixel_format());

  vidl_pixel_format target_fmt = vidl_pixel_format_from_ffmpeg(codec->pix_fmt);
  static vidl_frame_sptr temp_frame = new vidl_shared_frame(nullptr,frame->ni(),frame->nj(),target_fmt);

  AVFrame* out_frame = av_frame_alloc();
  //avcodec_get_frame_defaults( &out_frame );

  // The frame is in the correct format to encode directly
  if ( codec->pix_fmt == fmt )
  {
    avpicture_fill((AVPicture*)out_frame, (uint8_t*) frame->data(),
                   fmt, frame->ni(), frame->nj());
    //int ret = av_image_fill_arrays(out_frame->data, out_frame->linesize, (uint8_t*)frame->data(),
    //                     fmt, frame->ni(), frame->nj(), 1);
  }
  else
  {
    if (!temp_frame->data()) {
      unsigned ni = frame->ni();
      unsigned nj = frame->nj();
      unsigned out_size = vidl_pixel_format_buffer_size(ni,nj,target_fmt);
      temp_frame = new vidl_memory_chunk_frame(ni, nj, target_fmt,
                                               new vil_memory_chunk(out_size, VIL_PIXEL_FORMAT_BYTE));
    }
    // try conversion with FFMPEG functions
    if (!vidl_ffmpeg_convert(frame, temp_frame)) {
      // try conversion with vidl functions
      if (!vidl_convert_frame(*frame, *temp_frame)) {
        std::cout << "unable to convert " << frame->pixel_format() << " to "<<target_fmt<<std::endl;
        return false;
      }
    }
    avpicture_fill((AVPicture*)out_frame, (uint8_t*) temp_frame->data(),
                   codec->pix_fmt, frame->ni(), frame->nj());
    //int ret = av_image_fill_arrays(out_frame->data, out_frame->linesize, (uint8_t*)temp_frame->data(),
    //                     fmt, temp_frame->ni(), temp_frame->nj(), 1);
    //ret = 0;
  }

  AVPacket pkt;
  av_init_packet( &pkt );
  pkt.data = nullptr;
  pkt.size = 0;
  pkt.stream_index = 0;

  out_frame->pts = os_->cur_frame_;
  out_frame->width = codec->width;
  out_frame->height = codec->height;
  out_frame->format = codec->pix_fmt;

  int got_packet;
  int ret = avcodec_encode_video2( codec, &pkt, out_frame, &got_packet );

  if ( ret < 0 )
  {
    std::cerr << "FFMPEG video encoding failed" <<std::endl;
    return false;
  }
  // the encoder may not produce a packet on every frame
  // it may accumulate several frames before releasing a packet
  // contain the data from all of those frames
  if ( got_packet ) {
    if ( codec->coded_frame ) {
      pkt.pts = codec->coded_frame->pts;
    }
    if ( codec->coded_frame && codec->coded_frame->key_frame ) {
      pkt.flags |= AV_PKT_FLAG_KEY;
    }
    av_interleaved_write_frame( os_->fmt_cxt_, &pkt );
    av_packet_unref(&pkt);
  }

  ++os_->cur_frame_;
  return true;
}

#endif // vidl_ffmpeg_ostream_v56_hxx_
