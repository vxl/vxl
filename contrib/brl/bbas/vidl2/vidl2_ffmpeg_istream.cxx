// This is brl/bbas/vidl2/vidl2_ffmpeg_istream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   21 Dec 2005
//
//-----------------------------------------------------------------------------

#include "vidl2_ffmpeg_istream.h"
#include "vidl2_ffmpeg_init.h"
#include "vidl2_frame.h"
#include "vidl2_ffmpeg_convert.h"

#include <vcl_algorithm.h>
#include <vil/vil_new.h>

#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

//--------------------------------------------------------------------------------

struct vidl2_ffmpeg_istream::pimpl
{
  pimpl()
  : fmt_cxt_( NULL ),
  vid_index_( -1 ),
  vid_str_( NULL ),
  last_dts( 0 ),
  frame_( NULL ),
  cur_frame_( NULL ),
  deinterlace_( false ),
  frame_number_offset_( 0 )
  {
  }

  AVFormatContext* fmt_cxt_;
  int vid_index_;
  AVStream* vid_str_;

  //: Decode time of last frame.
  int64_t last_dts;

  //: The last successfully read frame.
  //
  // If frame_->data[0] is not NULL, then the frame corresponds to
  // the codec state, so that codec.width and so on apply to the
  // frame data.
  AVFrame* frame_;

  //: A contiguous memory buffer to store the current image data
  vil_memory_chunk_sptr contig_memory_;

  //: The last successfully decoded frame.
  mutable vidl2_frame_sptr cur_frame_;

  //: Apply deinterlacing on the frames?
  bool deinterlace_;

  //: Some codec/file format combinations need a frame number offset.
  // These codecs have a delay between reading packets and generating frames.
  unsigned frame_number_offset_;
};


//--------------------------------------------------------------------------------

//: Constructor
vidl2_ffmpeg_istream::
vidl2_ffmpeg_istream()
  : is_( new vidl2_ffmpeg_istream::pimpl )
{
  vidl2_ffmpeg_init();
}


//: Constructor - from a filename
vidl2_ffmpeg_istream::
vidl2_ffmpeg_istream(const vcl_string& filename)
  : is_( new vidl2_ffmpeg_istream::pimpl )
{
  vidl2_ffmpeg_init();
  open(filename);
}


//: Destructor
vidl2_ffmpeg_istream::
~vidl2_ffmpeg_istream()
{
  close();
  delete is_;
}

//: Open a new stream using a filename
bool
vidl2_ffmpeg_istream::
open(const vcl_string& filename)
{
  // Close any currently opened file
  close();

  // Open the file
  int err;
  if ( ( err = av_open_input_file( &is_->fmt_cxt_, filename.c_str(), NULL, 0, NULL ) ) != 0 ) {
    return false;
  }

  // Get the stream information by reading a bit of the file
  if ( av_find_stream_info( is_->fmt_cxt_ ) < 0 ) {
    return false;
  }

  // Find a video stream. Use the first one we find.
  is_->vid_index_ = -1;
  for ( int i = 0; i < is_->fmt_cxt_->nb_streams; ++i ) {
#if LIBAVFORMAT_BUILD <= 4628
    AVCodecContext *enc = &is_->fmt_cxt_->streams[i]->codec;
#else
    AVCodecContext *enc = is_->fmt_cxt_->streams[i]->codec;
#endif
    if ( enc->codec_type == CODEC_TYPE_VIDEO ) {
  is_->vid_index_ = i;
  break;
    }
  }
  if ( is_->vid_index_ == -1 ) {
    return false;
  }

  dump_format( is_->fmt_cxt_, 0, filename.c_str(), 0 );
#if LIBAVFORMAT_BUILD <= 4628
  AVCodecContext *enc = &is_->fmt_cxt_->streams[is_->vid_index_]->codec;
#else
  AVCodecContext *enc = is_->fmt_cxt_->streams[is_->vid_index_]->codec;
#endif
  // Open the stream
  AVCodec* codec = avcodec_find_decoder(enc->codec_id);
  if ( !codec || avcodec_open( enc, codec ) < 0 ) {
    return false;
  }

#if LIBAVFORMAT_BUILD <= 4623
  if (enc->frame_rate>1000 && enc->frame_rate_base==1)
    enc->frame_rate_base=1000;
#endif

  is_->vid_str_ = is_->fmt_cxt_->streams[ is_->vid_index_ ];
  is_->frame_ = avcodec_alloc_frame();

  advance();

  return true;
}


//: Close the stream
void
vidl2_ffmpeg_istream::
close()
{
  if ( is_->frame_ ) {
    av_free( is_->frame_ );
    is_->frame_ = 0;
  }

  is_->contig_memory_ = 0;
  is_->vid_index_ = -1;
  if ( is_->vid_str_ ) {
#if LIBAVFORMAT_BUILD <= 4628
    avcodec_close( &is_->vid_str_->codec );
#else
    avcodec_close( is_->vid_str_->codec );
#endif
    is_->vid_str_ = 0;
  }
  if ( is_->fmt_cxt_ ) {
    av_close_input_file( is_->fmt_cxt_ );
    is_->fmt_cxt_ = 0;
  }
}


//: Return true if the stream is open for reading
bool
vidl2_ffmpeg_istream::
is_open() const
{
  return is_->frame_;
}


//: Return true if the stream is in a valid state
bool
vidl2_ffmpeg_istream::
is_valid() const
{
  return is_open();
}


//: Return true if the stream support seeking
bool
vidl2_ffmpeg_istream::
is_seekable() const
{
  return true;
}


//: Return the current frame number
unsigned int
vidl2_ffmpeg_istream::
frame_number() const
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return 0;
  }

  return ((is_->last_dts - is_->vid_str_->start_time)
#if LIBAVFORMAT_BUILD <= 4623
      * is_->vid_str_->r_frame_rate / is_->vid_str_->r_frame_rate_base
      + AV_TIME_BASE/2) / AV_TIME_BASE
#else
      * is_->vid_str_->r_frame_rate.num / is_->vid_str_->r_frame_rate.den
      * is_->vid_str_->time_base.num + is_->vid_str_->time_base.den/2)
          / is_->vid_str_->time_base.den
#endif
      - is_->frame_number_offset_;
}


//: Advance to the next frame (but don't acquire an image)
bool
vidl2_ffmpeg_istream::
advance()
{
  // Quick return if the file isn't open.
  if ( !is_open() ) {
    return false;
  }

#if LIBAVFORMAT_BUILD <= 4628
  AVCodecContext* codec = &is_->fmt_cxt_->streams[is_->vid_index_]->codec;
#else
  AVCodecContext* codec = is_->fmt_cxt_->streams[is_->vid_index_]->codec;
#endif

  AVPacket pkt;
  int got_picture = 0;

  while ( got_picture == 0 ) {
    if ( av_read_frame( is_->fmt_cxt_, &pkt ) < 0 ) {
      break;
    }
    is_->last_dts = pkt.dts;

    // Make sure that the packet is from the actual video stream.
    if (pkt.stream_index==is_->vid_index_)
    {
      if ( codec->codec_id == CODEC_ID_RAWVIDEO ) {
        avpicture_fill( (AVPicture*)is_->frame_, pkt.data,
                         codec->pix_fmt,
                         codec->width,
                         codec->height );
        is_->frame_->pict_type = FF_I_TYPE;
        got_picture = 1;
      } else {
        avcodec_decode_video( codec,
                              is_->frame_, &got_picture,
                              pkt.data, pkt.size );
      }
    }
    av_free_packet( &pkt );
  }

  // From ffmpeg apiexample.c: some codecs, such as MPEG, transmit the
  // I and P frame with a latency of one frame. You must do the
  // following to have a chance to get the last frame of the video.
  if ( !got_picture ) {
    avcodec_decode_video( codec,
                          is_->frame_, &got_picture,
                          NULL, 0 );
#if LIBAVFORMAT_BUILD <= 4623
      is_->last_dts += AV_TIME_BASE * is_->vid_str_->r_frame_rate_base / is_->vid_str_->r_frame_rate;
#else
      is_->last_dts += int64_t(is_->vid_str_->time_base.den) * is_->vid_str_->r_frame_rate.den
                  / is_->vid_str_->time_base.num / is_->vid_str_->r_frame_rate.num;
#endif
  }

  // The cached frame is out of date, whether we managed to get a new
  // frame or not.
  if (is_->cur_frame_)
    is_->cur_frame_->invalidate();
  is_->cur_frame_ = 0;

  if ( ! got_picture ) {
    is_->frame_->data[0] = NULL;
  }

  return got_picture != 0;
}


//: Read the next frame from the stream
vidl2_frame_sptr
vidl2_ffmpeg_istream::read_frame()
{
  if (advance())
    return current_frame();
  return NULL;
}


//: Return the current frame in the stream
vidl2_frame_sptr
vidl2_ffmpeg_istream::current_frame()
{
  // Quick return if the stream isn't valid
  if ( !is_valid() ) {
    return NULL;
  }
#if LIBAVFORMAT_BUILD <= 4628
  AVCodecContext* enc = &is_->fmt_cxt_->streams[is_->vid_index_]->codec;
#else
  AVCodecContext* enc = is_->fmt_cxt_->streams[is_->vid_index_]->codec;
#endif
  // If we have not already converted this frame, try to convert it
  if ( !is_->cur_frame_ && is_->frame_->data[0] != 0 )
  {
    int width = enc->width;
    int height = enc->height;

    // Deinterlace if requested
    if ( is_->deinterlace_ ) {
      avpicture_deinterlace( (AVPicture*)is_->frame_, (AVPicture*)is_->frame_,
                             enc->pix_fmt, width, height );
    }

    // If the pixel format is not recognized by vidl2 then convert the data into RGB_24
    vidl2_pixel_format fmt = vidl2_pixel_format_from_ffmpeg(enc->pix_fmt);
    if (fmt == VIDL2_PIXEL_FORMAT_UNKNOWN)
    {
      int size = width*height*3;
      if (!is_->contig_memory_)
        is_->contig_memory_ = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
      else
        is_->contig_memory_->set_size(size, VIL_PIXEL_FORMAT_BYTE);

      AVPicture rgb_frame;
      avpicture_fill(&rgb_frame, (uint8_t*)is_->contig_memory_->data(), PIX_FMT_RGB24, width, height);
      img_convert(&rgb_frame, PIX_FMT_RGB24, (AVPicture*)is_->frame_, enc->pix_fmt, width, height);
      is_->cur_frame_ = new vidl2_shared_frame(is_->contig_memory_->data(),width,height,
                                               VIDL2_PIXEL_FORMAT_RGB_24);
    }
    else
    {
      // Test for contiguous memory.  Sometimes FFMPEG uses scanline buffers larger
      // than the image width.  The extra memory is used in optimized decoding routines.
      // This leads to a segmented image buffer, not supported by vidl2. 
      AVPicture test_frame;
      avpicture_fill(&test_frame, is_->frame_->data[0], enc->pix_fmt, width, height);
      if (test_frame.data[1] == is_->frame_->data[1] &&
         test_frame.data[2] == is_->frame_->data[2] &&
         test_frame.linesize[0] == is_->frame_->linesize[0] &&
         test_frame.linesize[1] == is_->frame_->linesize[1] &&
         test_frame.linesize[2] == is_->frame_->linesize[2] )
      {
        is_->cur_frame_ = new vidl2_shared_frame(is_->frame_->data[0], width, height, fmt);
      }
      // Copy the image into contiguous memory.
      else
      {
        if (!is_->contig_memory_){
          int size = avpicture_get_size( enc->pix_fmt, width, height );
          is_->contig_memory_ = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
        }
        avpicture_fill(&test_frame, (uint8_t*)is_->contig_memory_->data(), enc->pix_fmt, width, height);
        img_copy(&test_frame, (AVPicture*)is_->frame_, enc->pix_fmt, width, height);
        // use a shared frame because the vil_memory_chunk is reused for each frame
        is_->cur_frame_ = new vidl2_shared_frame(is_->contig_memory_->data(),width,height,fmt);
      }
    }
  }

  // The MPEG 2 codec has a latency of 1 frame, so the dts of the last
  // packet (stored in last_dts) is actually the next frame's
  // dts.
  if ( enc->codec_id == CODEC_ID_MPEG2VIDEO &&
       vcl_string("avi") == is_->fmt_cxt_->iformat->name ) {
    is_->frame_number_offset_ = 1;
  }

  return is_->cur_frame_;
}


//: Seek to the given frame number
// \returns true if successful
bool
vidl2_ffmpeg_istream::
seek_frame(unsigned int frame)
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return false;
  }

#if LIBAVFORMAT_BUILD <= 4623
  int64_t frame_size = int64_t(AV_TIME_BASE) * is_->vid_str_->r_frame_rate_base
                       / is_->vid_str_->r_frame_rate;
  int64_t req_timestamp = int64_t(AV_TIME_BASE) * frame * is_->vid_str_->r_frame_rate_base
                       / is_->vid_str_->r_frame_rate + is_->vid_str_->start_time;
#else
  int64_t frame_size = int64_t(is_->vid_str_->time_base.den) * is_->vid_str_->r_frame_rate.den
                       / is_->vid_str_->time_base.num / is_->vid_str_->r_frame_rate.num;
  int64_t req_timestamp = int64_t(is_->vid_str_->time_base.den) * frame * is_->vid_str_->r_frame_rate.den
                       / is_->vid_str_->time_base.num / is_->vid_str_->r_frame_rate.num + is_->vid_str_->start_time;
#endif

  if ( req_timestamp > frame_size/2 )
    req_timestamp -= frame_size/2;
  else
    req_timestamp = 0;

  // newer releases of ffmpeg may require a 4th argument to av_seek_frame
#if LIBAVFORMAT_BUILD <= 4616
  int seek = av_seek_frame( is_->fmt_cxt_, is_->vid_index_, req_timestamp );
#else
  int seek = av_seek_frame( is_->fmt_cxt_, is_->vid_index_, req_timestamp, AVSEEK_FLAG_BACKWARD );
#endif

  if ( seek < 0 )
    return false;
  // We got to a key frame. Forward until we get to the frame we want.
  while ( true )
  {
    if ( ! advance() ) {
      return false;
    }
    if ( is_->last_dts >= req_timestamp ) {
      if ( is_->last_dts >= req_timestamp + frame_size ) {
        vcl_cerr << "Warning: seek went into the future!\n";
        return false;
      }
      return true;
    }
  }
}

