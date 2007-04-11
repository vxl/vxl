// This is brl/bbas/vidl2/vidl2_ffmpeg_ostream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   3 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_ffmpeg_ostream.h"
#include "vidl2_ffmpeg_init.h"
#include "vidl2_ffmpeg_convert.h"
#include "vidl2_frame.h"
#include "vidl2_convert.h"
#include <vcl_cstring.h>
#include <vil/vil_memory_chunk.h>
#include <ffmpeg/avformat.h>

//-----------------------------------------------------------------------------


struct vidl2_ffmpeg_ostream::pimpl
{
  pimpl()
  : fmt_cxt_( 0 ),
  file_opened_( false ),
  codec_opened_( false ),
  cur_frame_( 0 ),
  video_rc_eq_(NULL)
  { }


  AVFormatContext* fmt_cxt_;
  bool file_opened_;
  bool codec_opened_;
  vil_memory_chunk_sptr bit_buf_;
  unsigned int cur_frame_;
  char* video_rc_eq_;
};


//-----------------------------------------------------------------------------


//: Constructor
vidl2_ffmpeg_ostream::
vidl2_ffmpeg_ostream()
  : os_( new vidl2_ffmpeg_ostream::pimpl )
{
  vidl2_ffmpeg_init();
}


//: Destructor
vidl2_ffmpeg_ostream::
~vidl2_ffmpeg_ostream()
{
  close();
  delete os_;
}


//: Constructor - opens a stream
vidl2_ffmpeg_ostream::
vidl2_ffmpeg_ostream(const vcl_string& filename,
                     const vidl2_ffmpeg_ostream_params& params)
  : os_( new vidl2_ffmpeg_ostream::pimpl ),
    filename_(filename), params_(params)
{
  vidl2_ffmpeg_init();
}


//: Open the stream
bool
vidl2_ffmpeg_ostream::
open()
{
  // Close any open files
  close();

  // a raw video packet is the same size as the input image. Others
  // are smaller.
  os_->bit_buf_ = new vil_memory_chunk( params_.ni_ * params_.nj_ * 3, VIL_PIXEL_FORMAT_BYTE );

  os_->fmt_cxt_ = av_alloc_format_context();

  AVOutputFormat* file_oformat = 0;
  if ( params_.file_format_ == vidl2_ffmpeg_ostream_params::GUESS ) {
    file_oformat = guess_format(NULL, filename_.c_str(), NULL);
    if (!file_oformat) {
      vcl_cerr << "ffmpeg: Unable for find a suitable output format for "
               << filename_ << '\n';
      close();
      return false;
    }
  } else {
    close();
    return false;
  }

  os_->fmt_cxt_->oformat = file_oformat;
  os_->fmt_cxt_->nb_streams = 0;

  // Create stream
  AVStream* st = av_new_stream( os_->fmt_cxt_, 1 );
  if ( !st ) {
    vcl_cerr << "ffmpeg: could not alloc stream\n";
    close();
    return false;
  }

  os_->fmt_cxt_->nb_streams = 1;

#if LIBAVFORMAT_BUILD <= 4628
  AVCodecContext *video_enc = &st->codec;
#else
  AVCodecContext *video_enc = st->codec;
#endif

  if ( vcl_strcmp(file_oformat->name, "mp4") != 0 ||
      vcl_strcmp(file_oformat->name, "mov") != 0 ||
      vcl_strcmp(file_oformat->name, "3gp") != 0 )
    video_enc->flags |= CODEC_FLAG_GLOBAL_HEADER;

  switch ( params_.encoder_ )
  {
   case vidl2_ffmpeg_ostream_params::DEFAULT:
    video_enc->codec_id = file_oformat->video_codec;
    break;
   case vidl2_ffmpeg_ostream_params::MPEG4:
    video_enc->codec_id = CODEC_ID_MPEG4;
    break;
   case vidl2_ffmpeg_ostream_params::MPEG2VIDEO:
    video_enc->codec_id = CODEC_ID_MPEG2VIDEO;
    break;
   case vidl2_ffmpeg_ostream_params::MSMPEG4V2:
    video_enc->codec_id = CODEC_ID_MSMPEG4V2;
    break;
   case vidl2_ffmpeg_ostream_params::RAWVIDEO:
    video_enc->codec_id = CODEC_ID_RAWVIDEO;
    break;
   case vidl2_ffmpeg_ostream_params::LJPEG:
    video_enc->codec_id = CODEC_ID_LJPEG;
    break;
   case vidl2_ffmpeg_ostream_params::HUFFYUV:
    video_enc->codec_id = CODEC_ID_HUFFYUV;
    break;
   case vidl2_ffmpeg_ostream_params::DVVIDEO:
    video_enc->codec_id = CODEC_ID_DVVIDEO;
    break;
   default:
    vcl_cout << "ffmpeg: Unknown encoder type\n";
    return false;
  }

  AVCodec* codec = avcodec_find_encoder( video_enc->codec_id );
  if ( !codec )
  {
    vcl_cerr << "ffmpeg_writer:: couldn't find encoder for " << video_enc->codec_id << '\n';
    return false;
  }

  video_enc->bit_rate = params_.bit_rate_ * 1000;
  video_enc->bit_rate_tolerance = params_.video_bit_rate_tolerance_;
#if LIBAVCODEC_BUILD <= 4753
  video_enc->frame_rate      = int( params_.frame_rate_ * 1000 );
  video_enc->frame_rate_base = 1000;
#else
  video_enc->time_base.num = 1000;
  video_enc->time_base.den = int(params_.frame_rate_*1000);
#endif

  if ( codec && codec->supported_framerates )
  {
    AVRational const* p = codec->supported_framerates;
#if LIBAVCODEC_BUILD <= 4753
    AVRational req = { video_enc->frame_rate, video_enc->frame_rate_base };
#else
    AVRational req = { video_enc->time_base.den, video_enc->time_base.num };
#endif
    AVRational const* best = NULL;
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
#if LIBAVCODEC_BUILD <= 4753
    video_enc->frame_rate      = best->num;
    video_enc->frame_rate_base = best->den;
#else
    video_enc->time_base.den= best->num;
    video_enc->time_base.num= best->den;
#endif
  }

  video_enc->width  = params_.ni_;
  video_enc->height = params_.nj_;
  video_enc->sample_aspect_ratio = av_d2q(params_.frame_aspect_ratio_*params_.ni_/params_.nj_, 255);

  // Our source is packed RGB. Use that if possible.
  video_enc->pix_fmt = PIX_FMT_RGB24;
  if ( codec && codec->pix_fmts )
  {
    const enum PixelFormat* p= codec->pix_fmts;
    for ( ; *p != -1; p++ )
    {
      if ( *p == video_enc->pix_fmt )
        break;
    }
    if ( *p == -1 )
      video_enc->pix_fmt = codec->pix_fmts[0];
  }
  else if ( codec && ( codec->id == CODEC_ID_RAWVIDEO ||
                      codec->id == CODEC_ID_HUFFYUV ) )
  {
    // these formats only support the YUV input image formats
    video_enc->pix_fmt = PIX_FMT_YUV420P;
  }

  if (!params_.intra_only_)
    video_enc->gop_size = params_.gop_size_;
  else
    video_enc->gop_size = 0;
  if (params_.video_qscale_ || params_.same_quality_)
  {
    video_enc->flags |= CODEC_FLAG_QSCALE;
    st->quality = FF_QP2LAMBDA * params_.video_qscale_;
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
  video_enc->quantizer_noise_shaping= params_.qns_;

  if (params_.use_umv_)
  {
    video_enc->flags |= CODEC_FLAG_H263P_UMV;
  }
  if (params_.use_ss_)
  {
    video_enc->flags |= CODEC_FLAG_H263P_SLICE_STRUCT;
  }
  if (params_.use_aic_)
  {
    video_enc->flags |= CODEC_FLAG_H263P_AIC;
  }
  if (params_.use_aiv_)
  {
    video_enc->flags |= CODEC_FLAG_H263P_AIV;
  }
  if (params_.use_4mv_)
  {
    video_enc->flags |= CODEC_FLAG_4MV;
  }
  if (params_.use_obmc_)
  {
    video_enc->flags |= CODEC_FLAG_OBMC;
  }
  if (params_.use_loop_)
  {
    video_enc->flags |= CODEC_FLAG_LOOP_FILTER;
  }

  if (params_.use_part_)
  {
    video_enc->flags |= CODEC_FLAG_PART;
  }
  if (params_.use_alt_scan_)
  {
    video_enc->flags |= CODEC_FLAG_ALT_SCAN;
  }
  if (params_.use_trell_)
  {
    video_enc->flags |= CODEC_FLAG_TRELLIS_QUANT;
  }
  if (params_.use_scan_offset_)
  {
    video_enc->flags |= CODEC_FLAG_SVCD_SCAN_OFFSET;
  }
  if (params_.closed_gop_)
  {
    video_enc->flags |= CODEC_FLAG_CLOSED_GOP;
  }
  if (params_.use_qpel_)
  {
    video_enc->flags |= CODEC_FLAG_QPEL;
  }
  if (params_.use_qprd_)
  {
    video_enc->flags |= CODEC_FLAG_QP_RD;
  }
  if (params_.use_cbprd_)
  {
    video_enc->flags |= CODEC_FLAG_CBP_RD;
  }
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
  video_enc->lmin = params_.video_lmin_;
  video_enc->lmax = params_.video_lmax_;
  video_enc->mb_qmin = params_.video_mb_qmin_;
  video_enc->mb_qmax = params_.video_mb_qmax_;
  video_enc->max_qdiff = params_.video_qdiff_;
  video_enc->qblur = params_.video_qblur_;
  video_enc->qcompress = params_.video_qcomp_;

  // delete when the stream is closed
  os_->video_rc_eq_ = new char[params_.video_rc_eq_.length()];
  vcl_strcpy(os_->video_rc_eq_, params_.video_rc_eq_.c_str());
  video_enc->rc_eq = os_->video_rc_eq_;

  video_enc->debug = params_.debug_;
  video_enc->debug_mv = params_.debug_mv_;
  video_enc->thread_count = 1;

  video_enc->rc_max_rate = params_.video_rc_max_rate_;
  video_enc->rc_min_rate = params_.video_rc_min_rate_;
  video_enc->rc_buffer_size = params_.video_rc_buffer_size_;
  video_enc->rc_buffer_aggressivity= params_.video_rc_buffer_aggressivity_;
  video_enc->rc_initial_cplx= params_.video_rc_initial_cplx_;
  video_enc->i_quant_factor = params_.video_i_qfactor_;
  video_enc->b_quant_factor = params_.video_b_qfactor_;
  video_enc->i_quant_offset = params_.video_i_qoffset_;
  video_enc->b_quant_offset = params_.video_b_qoffset_;
  video_enc->intra_quant_bias = params_.video_intra_quant_bias_;
  video_enc->inter_quant_bias = params_.video_inter_quant_bias_;
  video_enc->dct_algo = params_.dct_algo_;
  video_enc->idct_algo = params_.idct_algo_;
  video_enc->me_threshold= params_.me_threshold_;
  video_enc->mb_threshold= params_.mb_threshold_;
  video_enc->intra_dc_precision= params_.intra_dc_precision_ - 8;
  video_enc->strict_std_compliance = params_.strict_;
  video_enc->error_rate = params_.error_rate_;
  video_enc->noise_reduction= params_.noise_reduction_;
  video_enc->scenechange_threshold= params_.sc_threshold_;
  video_enc->me_range = params_.me_range_;
  video_enc->coder_type= params_.coder_;
  video_enc->context_model= params_.context_;
  video_enc->prediction_method= params_.predictor_;
#if 0
  // old versions of FFMPEG don't support these
  // so lets ignore them for now
  video_enc->profile= params_.video_profile_;
  video_enc->level= params_.video_level_;
#endif

  if (params_.packet_size_)
  {
    video_enc->rtp_mode= 1;
    video_enc->rtp_payload_size= params_.packet_size_;
  }

  if (params_.do_psnr_)
    video_enc->flags|= CODEC_FLAG_PSNR;

  video_enc->me_method = params_.me_method_;

  /* two pass mode */
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

  os_->fmt_cxt_->timestamp = 0;
  os_->fmt_cxt_->title[0] = '\0';
  os_->fmt_cxt_->author[0] = '\0';
  os_->fmt_cxt_->copyright[0] = '\0';
  os_->fmt_cxt_->comment[0] = '\0';

  vcl_strncpy( os_->fmt_cxt_->filename, filename_.c_str(), 1023 );

  if ( url_fopen( &os_->fmt_cxt_->pb, filename_.c_str(), URL_WRONLY) < 0 )
  {
    vcl_cerr << "ffmpeg: couldn't open " << filename_ << " for writing\n";
    close();
    return false;
  }
  os_->file_opened_ = true;

  AVFormatParameters fmt_param;
  vcl_memset( &fmt_param, 0, sizeof(fmt_param) );
  if ( av_set_parameters( os_->fmt_cxt_, &fmt_param ) < 0 )
  {
    vcl_cerr << "ffmpeg: invalid encoding parameter\n";
    close();
    return false;
  }

  //dump_format( os_->fmt_cxt_, 1, filename_, 1 );

  if ( avcodec_open( video_enc, codec ) < 0 )
  {
    vcl_cerr << "ffmpeg: couldn't open codec\n";
    close();
    return false;
  }
  os_->codec_opened_ = true;

  if ( av_write_header( os_->fmt_cxt_ ) < 0 )
  {
    vcl_cerr << "ffmpeg: couldn't write header\n";
    close();
    return false;
  }

  return true;
}


//: Close the stream
void
vidl2_ffmpeg_ostream::
close()
{
  delete os_->video_rc_eq_;
  os_->video_rc_eq_ = NULL;

  if ( os_->fmt_cxt_ ) {

    if ( os_->file_opened_ ) {
      av_write_trailer( os_->fmt_cxt_ );
      url_fclose( &os_->fmt_cxt_->pb );
      os_->file_opened_ = false;
    }

    if ( os_->fmt_cxt_->nb_streams > 0 ) {
      if ( os_->codec_opened_ ) {
        for ( int i = 0; i < os_->fmt_cxt_->nb_streams; ++i ) {
#if LIBAVFORMAT_BUILD <= 4628
          AVCodecContext* codec = &os_->fmt_cxt_->streams[i]->codec;
#else
          AVCodecContext* codec = os_->fmt_cxt_->streams[i]->codec;
#endif
          if ( codec->stats_in ) {
            av_freep( codec->stats_in );
          }
          avcodec_close( codec );
        }
      }
      os_->codec_opened_ = false;
      for ( int i = 0; i < os_->fmt_cxt_->nb_streams; ++i ) {
        av_free( os_->fmt_cxt_->streams[i] );
      }
    }

    av_free( os_->fmt_cxt_ );
    os_->fmt_cxt_ = 0;
  }
}


//: Return true if the stream is open for writing
bool
vidl2_ffmpeg_ostream::
is_open() const
{
  return os_->file_opened_;
}


//: Write and image to the stream
// \retval false if the image could not be written
bool
vidl2_ffmpeg_ostream::
write_frame(const vidl2_frame_sptr& frame)
{
  if (!is_open()){
    // resize to the first frame
    params_.size(frame->ni(),frame->nj());
    open();
  }

#if LIBAVFORMAT_BUILD <= 4628
  AVCodecContext* codec = &os_->fmt_cxt_->streams[0]->codec;
#else
  AVCodecContext* codec = os_->fmt_cxt_->streams[0]->codec;
#endif

  if ( unsigned( codec->width ) != frame->ni() ||
      unsigned( codec->height ) != frame->nj() ) {
    vcl_cerr << "ffmpeg: Input image has wrong size. Expecting ("
             << codec->width << 'x' << codec->height << "), got ("
             << frame->ni() << 'x' << frame->nj() << ")\n";
    return false;
  }


  PixelFormat fmt = vidl2_pixel_format_to_ffmpeg(frame->pixel_format());

  vidl2_pixel_format target_fmt = vidl2_pixel_format_from_ffmpeg(codec->pix_fmt);
  static vidl2_frame_sptr temp_frame = new vidl2_shared_frame(NULL,frame->ni(),frame->nj(),target_fmt);

  AVFrame out_frame;
  avcodec_get_frame_defaults( &out_frame );

  // The frame is in the correct format to encode directly
  if ( codec->pix_fmt == fmt )
  {
    avpicture_fill((AVPicture*)&out_frame, (uint8_t*) frame->data(),
                    fmt, frame->ni(), frame->nj());
  }
  else
  {
    if (!temp_frame->data()){
      unsigned ni = frame->ni();
      unsigned nj = frame->nj();
      unsigned out_size = vidl2_pixel_format_buffer_size(ni,nj,target_fmt);
      temp_frame = new vidl2_memory_chunk_frame(ni, nj, target_fmt,
                                                new vil_memory_chunk(out_size, VIL_PIXEL_FORMAT_BYTE));
    }
    // try conversion with FFMPEG functions
    if (!vidl2_ffmpeg_convert(frame, temp_frame)){
      // try conversion with vidl2 functions
      if (!vidl2_convert_frame(*frame, *temp_frame)){
        vcl_cout << "unable to convert " << frame->pixel_format() << " to "<<target_fmt<<vcl_endl;
        return false;
      }
    }
    avpicture_fill((AVPicture*)&out_frame, (uint8_t*) temp_frame->data(),
                    codec->pix_fmt, frame->ni(), frame->nj());
  }

  AVPacket pkt;
  av_init_packet( &pkt );
  pkt.stream_index = 0;

#if LIBAVCODEC_BUILD <= 4753
  out_frame.pts = av_rescale( os_->cur_frame_, AV_TIME_BASE*(int64_t)codec->frame_rate_base, codec->frame_rate );
#else
  out_frame.pts = os_->cur_frame_;
#endif

  int ret = avcodec_encode_video( codec, (uint8_t*)os_->bit_buf_->data(), os_->bit_buf_->size(), &out_frame );

  if ( ret ) {
    pkt.data = (uint8_t*)os_->bit_buf_->data();
    pkt.size = ret;
    if ( codec->coded_frame ) {
      pkt.pts = codec->coded_frame->pts;
    }
    if ( codec->coded_frame && codec->coded_frame->key_frame ) {
      pkt.flags |= PKT_FLAG_KEY;
    }
    av_interleaved_write_frame( os_->fmt_cxt_, &pkt );
  } else {
    return false;
  }

  ++os_->cur_frame_;
  return true;
}
