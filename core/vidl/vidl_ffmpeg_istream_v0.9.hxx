// This is core/vidl/vidl_ffmpeg_istream_v0.9.hxx
#ifndef vidl_ffmpeg_istream_v0_9_hxx_
#define vidl_ffmpeg_istream_v0_9_hxx_
#include <string>
#include <iostream>
#include "vidl_ffmpeg_istream.h"
//:
// \file
// \author Matt Leotta
// \author Amitha Perera
// \date   5 Feb 2013
//
// Update implementation based on FFMPEG release version 0.9
// ffmpeg git hash f8d8c57fbb780d6e6e630f16d530f392b31faae

//-----------------------------------------------------------------------------

#include "vidl_ffmpeg_init.h"
#include "vidl_frame.h"
#include "vidl_ffmpeg_convert.h"
#include "vidl_ffmpeg_pixel_format.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

extern "C" {
#if FFMPEG_IN_SEVERAL_DIRECTORIES
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#else
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#include <ffmpeg/swscale.h>
#endif
}

//--------------------------------------------------------------------------------

struct vidl_ffmpeg_istream::pimpl
{
  pimpl()
  : fmt_cxt_( NULL ),
    vid_index_( -1 ),
    data_index_( -1 ),
    vid_str_( NULL ),
    last_dts( 0 ),
    frame_( NULL ),
    num_frames_( -2 ), // sentinel value to indicate not yet computed
    sws_context_( NULL ),
    cur_frame_( NULL ),
    metadata_( 0 ),
    deinterlace_( false ),
    pts_( 0 ),
    frame_number_offset_( 0 ),
    video_enc_( 0 )
  {
    packet_.data = NULL;
  }

  AVFormatContext* fmt_cxt_;
  int vid_index_;
  int data_index_;
  AVStream* vid_str_;

  AVCodecContext *video_enc_;

  //: Decode time of last frame.
  int64_t last_dts;

  //: Start time of the stream, to offset the dts when computing the frame number.
  int64_t start_time;

  //: The last successfully read frame.
  //
  // If frame_->data[0] is not NULL, then the frame corresponds to
  // the codec state, so that codec.width and so on apply to the
  // frame data.
  AVFrame* frame_;

  //: The last successfully read packet (before decoding).
  //  This must not be freed if the packet contains the raw image,
  //  in which case the frame_ will have only a shallow copy
  AVPacket packet_;

  //: number of counted frames
  int num_frames_;

  //: A software scaling context
  //
  // This is the context used for the software scaling and colour
  // conversion routines. Since the conversion is likely to be the
  // same for each frame, we save the context to avoid re-creating it
  // every time.
  SwsContext* sws_context_;

  //: A contiguous memory buffer to store the current image data
  vil_memory_chunk_sptr contig_memory_;

  //: The last successfully decoded frame.
  mutable vidl_frame_sptr cur_frame_;

  //: the buffer of metadata from the data stream
  std::deque<vxl_byte> metadata_;

  //: Apply deinterlacing on the frames?
  bool deinterlace_;

  //: Some codec/file format combinations need a frame number offset.
  // These codecs have a delay between reading packets and generating frames.
  unsigned frame_number_offset_;

  //: Presentation timestamp
  double pts_;
};


//--------------------------------------------------------------------------------

//: Constructor
vidl_ffmpeg_istream::
vidl_ffmpeg_istream()
  : is_( new vidl_ffmpeg_istream::pimpl )
{
  vidl_ffmpeg_init();
}


//: Constructor - from a filename
vidl_ffmpeg_istream::
vidl_ffmpeg_istream(const std::string& filename)
  : is_( new vidl_ffmpeg_istream::pimpl )
{
  vidl_ffmpeg_init();
  open(filename);
}


//: Destructor
vidl_ffmpeg_istream::
~vidl_ffmpeg_istream()
{
  close();
  delete is_;
}

/* These are called whenever we allocate a frame
 * buffer. We use this to store the global_pts in
 * a frame at the time it is allocated.
 */
int get_buffer(struct AVCodecContext *c, AVFrame *pic)
{
  int ret = avcodec_default_get_buffer(c, pic);
  uint64_t *pts = (uint64_t *)av_malloc(sizeof(uint64_t));
  *pts = *((uint64_t *)c->opaque);
  pic->opaque = pts;
  return ret;
}

void release_buffer(struct AVCodecContext *c, AVFrame *pic)
{
  if(pic) av_freep(&pic->opaque);
  avcodec_default_release_buffer(c, pic);
}

//: Open a new stream using a filename
bool
vidl_ffmpeg_istream::
open(const std::string& filename)
{
  // Close any currently opened file
  close();

  // Open the file
  int err;
  if ( ( err = avformat_open_input( &is_->fmt_cxt_, filename.c_str(), NULL, NULL ) ) != 0 ) {
    return false;
  }

  // Get the stream information by reading a bit of the file
  if ( avformat_find_stream_info( is_->fmt_cxt_, NULL ) < 0 ) {
    return false;
  }

  // Find a video stream, and optionally a data stream.
  // Use the first ones we find.
  is_->vid_index_ = -1;
  is_->data_index_ = -1;
  for ( unsigned i = 0; i < is_->fmt_cxt_->nb_streams; ++i ) {
    AVCodecContext *enc = is_->fmt_cxt_->streams[i]->codec;
    if ( enc->codec_type == AVMEDIA_TYPE_VIDEO && is_->vid_index_ < 0) {
      is_->vid_index_ = i;
    }
    else if( enc->codec_type == AVMEDIA_TYPE_DATA && is_->data_index_ < 0) {
      is_->data_index_ = i;
    }
  }
  if ( is_->vid_index_ == -1 ) {
    return false;
  }

  av_dump_format( is_->fmt_cxt_, 0, filename.c_str(), 0 );
  is_->video_enc_ = is_->fmt_cxt_->streams[is_->vid_index_]->codec;

  //Tell ffmpeg to use our get/release buffers for pts management
  is_->video_enc_->get_buffer = get_buffer;
  is_->video_enc_->release_buffer = release_buffer;

  uint64_t *video_pkt_pts = (uint64_t *)av_malloc(sizeof(uint64_t));
  *video_pkt_pts = AV_NOPTS_VALUE;
  is_->video_enc_->opaque = video_pkt_pts;

  // Open the stream
  AVCodec* codec = avcodec_find_decoder(is_->video_enc_->codec_id);
  if ( !codec || avcodec_open2( is_->video_enc_, codec, NULL ) < 0 ) {
    return false;
  }

  is_->vid_str_ = is_->fmt_cxt_->streams[ is_->vid_index_ ];
  is_->frame_ = avcodec_alloc_frame();

  if ( is_->vid_str_->start_time == int64_t(1)<<63 ) {
    is_->start_time = 0;
  }
  else {
    is_->start_time = is_->vid_str_->start_time;
  }

  // The MPEG 2 codec has a latency of 1 frame when encoded in an AVI
  // stream, so the dts of the last packet (stored in last_dts) is
  // actually the next frame's dts.
  if ( is_->vid_str_->codec->codec_id == CODEC_ID_MPEG2VIDEO &&
       std::string("avi") == is_->fmt_cxt_->iformat->name ) {
    is_->frame_number_offset_ = 1;
  }

  // Not sure if this does anything, but no harm either
  av_init_packet(&is_->packet_);
  is_->packet_.data = 0;
  is_->packet_.size = 0;

  return true;
}


//: Close the stream
void
vidl_ffmpeg_istream::
close()
{
  if( is_->packet_.data ) {
    av_free_packet( &is_->packet_ );  // free last packet
  }

  if ( is_->frame_ ) {
    av_freep( &is_->frame_ );
  }

  if (is_->video_enc_ && is_->video_enc_->opaque) {
    av_freep( &is_->video_enc_->opaque );
  }

  is_->num_frames_ = -2;
  is_->contig_memory_ = 0;
  is_->vid_index_ = -1;
  is_->data_index_ = -1;
  is_->metadata_.clear();
  if ( is_->vid_str_ ) {
    avcodec_close( is_->vid_str_->codec );
    is_->vid_str_ = 0;
  }
  if ( is_->fmt_cxt_ ) {
    avformat_close_input( &is_->fmt_cxt_ );
    is_->fmt_cxt_ = 0;
  }

  is_->video_enc_ = 0;
}


//: Return true if the stream is open for reading
bool
vidl_ffmpeg_istream::
is_open() const
{
  return ! ! is_->frame_;
}


//: Return true if the stream is in a valid state
bool
vidl_ffmpeg_istream::
is_valid() const
{
  return is_open() && is_->frame_->data[0] != 0;
}


//: Return true if the stream support seeking
bool
vidl_ffmpeg_istream::
is_seekable() const
{
  return true;
}


//: Return the number of frames if known
//  returns -1 for non-seekable streams
int
vidl_ffmpeg_istream::num_frames() const
{
  // to get an accurate frame count, quickly run through the entire
  // video.  We'll only do this if the user hasn't read any frames,
  // because we have no guarantee that we can successfully seek back
  // to anywhere but the beginning.  There is logic in advance() to
  // ensure this.
  vidl_ffmpeg_istream* mutable_this = const_cast<vidl_ffmpeg_istream*>(this);
  if ( mutable_this->is_->num_frames_ == -2 ) {
    mutable_this->is_->num_frames_ = 0;
    while (mutable_this->advance()) {
      ++mutable_this->is_->num_frames_;
    }
    av_seek_frame( mutable_this->is_->fmt_cxt_,
                   mutable_this->is_->vid_index_,
                   0,
                   AVSEEK_FLAG_BACKWARD );
  }

  return is_->num_frames_;
}


//: Return the current frame number
unsigned int
vidl_ffmpeg_istream::
frame_number() const
{
  // Quick return if the stream isn't open.
  if ( !is_valid() ) {
    return static_cast<unsigned int>(-1);
  }

  return (unsigned int)( ((is_->last_dts - is_->start_time)
                          * is_->vid_str_->r_frame_rate.num / is_->vid_str_->r_frame_rate.den
                          * is_->vid_str_->time_base.num + is_->vid_str_->time_base.den/2)
                           / is_->vid_str_->time_base.den
                           - int(is_->frame_number_offset_) );
}


//: Return the width of each frame
unsigned int
vidl_ffmpeg_istream
::width() const
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return 0;
  }

  return is_->fmt_cxt_->streams[is_->vid_index_]->codec->width;
}


//: Return the height of each frame
unsigned int
vidl_ffmpeg_istream
::height() const
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return 0;
  }

  return is_->fmt_cxt_->streams[is_->vid_index_]->codec->height;
}


//: Return the pixel format
vidl_pixel_format
vidl_ffmpeg_istream
::format() const
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return VIDL_PIXEL_FORMAT_UNKNOWN;
  }

  AVCodecContext* enc = is_->fmt_cxt_->streams[is_->vid_index_]->codec;
  vidl_pixel_format fmt = vidl_pixel_format_from_ffmpeg(enc->pix_fmt);
  if (fmt == VIDL_PIXEL_FORMAT_UNKNOWN)
    return VIDL_PIXEL_FORMAT_RGB_24;
  return fmt;
}


//: Return the frame rate (0.0 if unspecified)
double
vidl_ffmpeg_istream
::frame_rate() const
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return 0.0;
  }

  return static_cast<double>(is_->vid_str_->r_frame_rate.num) / is_->vid_str_->r_frame_rate.den;
}


//: Return the duration in seconds (0.0 if unknown)
double
vidl_ffmpeg_istream
::duration() const
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return 0.0;
  }
  return static_cast<double>(is_->vid_str_->time_base.num)/is_->vid_str_->time_base.den
         * static_cast<double>(is_->vid_str_->duration);
}


//: Advance to the next frame (but don't acquire an image)
bool
vidl_ffmpeg_istream::
advance()
{
  // Quick return if the file isn't open.
  if ( !is_open() ) {
    return false;
  }

  // See the comment in num_frames().  This is to make sure that once
  // we start reading frames, we'll never try to march to the end to
  // figure out how many frames there are.
  if ( is_->num_frames_ == -2 ) {
    is_->num_frames_ = -1;
  }

  AVCodecContext* codec = is_->fmt_cxt_->streams[is_->vid_index_]->codec;

  if( is_->packet_.data )
    av_free_packet( &is_->packet_ );  // free previous packet

  int got_picture = 0;
  uint64_t pts;

  // clear the metadata from the previous frame
  is_->metadata_.clear();

  while ( av_read_frame( is_->fmt_cxt_, &is_->packet_ ) >= 0 && got_picture == 0 )
  {
    is_->last_dts = is_->packet_.dts;

    pts = 0;

    // Make sure that the packet is from the actual video stream.
    if (is_->packet_.stream_index==is_->vid_index_)
    {

      *(uint64_t *)is_->video_enc_->opaque = is_->packet_.pts;
      if ( avcodec_decode_video2( codec,
                                  is_->frame_, &got_picture,
                                  &is_->packet_ ) < 0 ) {
        std::cerr << "vidl_ffmpeg_istream: Error decoding packet!\n";
        av_free_packet( &is_->packet_ );
        return false;
      }

      if (is_->packet_.dts == AV_NOPTS_VALUE &&
               is_->frame_->opaque &&
               *(uint64_t *)is_->frame_->opaque != AV_NOPTS_VALUE) {
        pts = *(uint64_t *)is_->frame_->opaque;
      }
      else if (is_->packet_.dts != AV_NOPTS_VALUE) {
        pts = is_->packet_.dts;
      }
      else {
        pts = 0;
      }

      is_->pts_ = pts * av_q2d(is_->vid_str_->time_base);
      break; // without freeing the packet
    }
    // grab the metadata from this packet if from the metadata stream
    else if (is_->packet_.stream_index==is_->data_index_)
    {
      is_->metadata_.insert( is_->metadata_.end(), is_->packet_.data,
                             is_->packet_.data+is_->packet_.size);
    }

    av_free_packet( &is_->packet_ );
  }

  // From ffmpeg apiexample.c: some codecs, such as MPEG, transmit the
  // I and P frame with a latency of one frame. You must do the
  // following to have a chance to get the last frame of the video.
  if ( !got_picture ) {
    av_init_packet(&is_->packet_);
    is_->packet_.data = NULL;
    is_->packet_.size = 0;
    if ( avcodec_decode_video2( codec,
                                is_->frame_, &got_picture,
                                &is_->packet_ ) >= 0 ) {
      is_->last_dts += int64_t(is_->vid_str_->time_base.den) * is_->vid_str_->r_frame_rate.den
        / is_->vid_str_->time_base.num / is_->vid_str_->r_frame_rate.num;
    }
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
vidl_frame_sptr
vidl_ffmpeg_istream::read_frame()
{
  if (advance())
    return current_frame();
  return NULL;
}


//: Return the current frame in the stream
vidl_frame_sptr
vidl_ffmpeg_istream::current_frame()
{
  // Quick return if the stream isn't valid
  if ( !is_valid() ) {
    return NULL;
  }
  AVCodecContext* enc = is_->fmt_cxt_->streams[is_->vid_index_]->codec;
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

    // If the pixel format is not recognized by vidl then convert the data into RGB_24
    vidl_pixel_format fmt = vidl_pixel_format_from_ffmpeg(enc->pix_fmt);
    if (fmt == VIDL_PIXEL_FORMAT_UNKNOWN)
    {
      int size = width*height*3;
      if (!is_->contig_memory_)
        is_->contig_memory_ = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
      else
        is_->contig_memory_->set_size(size, VIL_PIXEL_FORMAT_BYTE);

      // Reuse the previous context if we can.
      is_->sws_context_ = sws_getCachedContext(
        is_->sws_context_,
        width, height, enc->pix_fmt,
        width, height, PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL, NULL, NULL );

      if ( is_->sws_context_ == NULL ) {
        std::cerr << "vidl_ffmpeg_istream: couldn't create conversion context\n";
        return vidl_frame_sptr();
      }

      AVPicture rgb_frame;
      avpicture_fill(&rgb_frame, (uint8_t*)is_->contig_memory_->data(), PIX_FMT_RGB24, width, height);

      sws_scale( is_->sws_context_,
                 is_->frame_->data, is_->frame_->linesize,
                 0, height,
                 rgb_frame.data, rgb_frame.linesize );

      is_->cur_frame_ = new vidl_shared_frame(is_->contig_memory_->data(),width,height,
                                              VIDL_PIXEL_FORMAT_RGB_24);
    }
    else
    {
      // Test for contiguous memory.  Sometimes FFMPEG uses scanline buffers larger
      // than the image width.  The extra memory is used in optimized decoding routines.
      // This leads to a segmented image buffer, not supported by vidl.
      AVPicture test_frame;
      avpicture_fill(&test_frame, is_->frame_->data[0], enc->pix_fmt, width, height);
      if (test_frame.data[1] == is_->frame_->data[1] &&
          test_frame.data[2] == is_->frame_->data[2] &&
          test_frame.linesize[0] == is_->frame_->linesize[0] &&
          test_frame.linesize[1] == is_->frame_->linesize[1] &&
          test_frame.linesize[2] == is_->frame_->linesize[2] )
      {
        is_->cur_frame_ = new vidl_shared_frame(is_->frame_->data[0], width, height, fmt);
      }
      // Copy the image into contiguous memory.
      else
      {
        if (!is_->contig_memory_) {
          int size = avpicture_get_size( enc->pix_fmt, width, height );
          is_->contig_memory_ = new vil_memory_chunk(size, VIL_PIXEL_FORMAT_BYTE);
        }
        avpicture_fill(&test_frame, (uint8_t*)is_->contig_memory_->data(), enc->pix_fmt, width, height);
        av_picture_copy(&test_frame, (AVPicture*)is_->frame_, enc->pix_fmt, width, height);
        // use a shared frame because the vil_memory_chunk is reused for each frame
        is_->cur_frame_ = new vidl_shared_frame(is_->contig_memory_->data(),width,height,fmt);
      }
    }
  }

  return is_->cur_frame_;
}


//: Seek to the given frame number
// \returns true if successful
bool
vidl_ffmpeg_istream::
seek_frame(unsigned int frame)
{
  // Quick return if the stream isn't open.
  if ( !is_open() ) {
    return false;
  }

  // We rely on the initial cast to make sure all the operations happen in int64.
  int64_t req_timestamp =
    int64_t(frame + is_->frame_number_offset_)
    * is_->vid_str_->time_base.den
    * is_->vid_str_->r_frame_rate.den
    / is_->vid_str_->time_base.num
    / is_->vid_str_->r_frame_rate.num
    + is_->start_time;

  // Seek to a keyframe before the timestamp that we want.
  int seek = av_seek_frame( is_->fmt_cxt_, is_->vid_index_, req_timestamp, AVSEEK_FLAG_BACKWARD );

  if ( seek < 0 )
    return false;

  avcodec_flush_buffers( is_->vid_str_->codec );

  // We got to a key frame. Forward until we get to the frame we want.
  while ( true )
  {
    if ( ! advance() ) {
      return false;
    }
    if ( is_->last_dts >= req_timestamp ) {
      if ( is_->last_dts > req_timestamp ) {
        std::cerr << "Warning: seek went into the future!\n";
        return false;
      }
      return true;
    }
  }
}


//: Return the raw metadata bytes obtained while reading the current frame.
//  This deque will be empty if there is no metadata stream
//  Metadata is often encoded as KLV,
//  but no attempt to decode KLV is made here
std::deque<vxl_byte>
vidl_ffmpeg_istream::
current_metadata()
{
  return is_->metadata_;
}


//: Return true if the video also has a metadata stream
bool
vidl_ffmpeg_istream::
has_metadata() const
{
  return is_open() && is_->data_index_ >= 0;
}

double
vidl_ffmpeg_istream::
current_pts() const
{
  return is_->pts_;
}

//: Return the current video packet's data, is used to get
//  video stream embeded metadata.
std::vector<vxl_byte>
vidl_ffmpeg_istream::
current_packet_data() const
{
  return std::vector<vxl_byte>(is_->packet_.data, is_->packet_.data + is_->packet_.size);
}

#endif // vidl_ffmpeg_istream_v0_9_hxx_
