//:
// \file

#include "vidl_ffmpeg_codec.h"
#include <vil/vil_crop.h>
#include <vul/vul_file.h>

// Constructor
vidl_ffmpeg_codec::vidl_ffmpeg_codec()
 : fmt_cxt_( NULL ),
   vid_index_( -1 ),
   vid_str_( NULL ),
   last_dts( 0 ),
   frame_( NULL ),
   deinterlace_( false ),
   frame_number_offset_( 0 )
{
  initialize();
}


// Destructor
vidl_ffmpeg_codec::~vidl_ffmpeg_codec()
{
  close();
}


void
vidl_ffmpeg_codec::initialize()
{
  static bool initialized = false;
  if ( ! initialized ) {
    av_register_all();
    av_log_set_level(AV_LOG_ERROR);
    initialized = true;
  }
}

//-----------------------------------------------------------------------------
//: Probe the file fname, open it as an AVI file. If it works, return true, false otherwise.

bool vidl_ffmpeg_codec::probe(vcl_string const& fname)
{
  // Close any currently opened file
  close();

  // Open the file
  int err;
  if ( ( err = av_open_input_file( &fmt_cxt_, fname.c_str(), NULL, 0, NULL ) ) != 0 ) {
    return false;
  }

  // Get the stream information by reading a bit of the file
  if ( av_find_stream_info( fmt_cxt_ ) < 0 ) {
    return false;
  }

  // Find a video stream. Use the first one we find.
  vid_index_ = -1;
  for ( int i = 0; i < fmt_cxt_->nb_streams; ++i ) {
    AVCodecContext *enc = &fmt_cxt_->streams[i]->codec;
    if ( enc->codec_type == CODEC_TYPE_VIDEO ) {
      vid_index_ = i;
      break;
    }
  }
  if ( vid_index_ == -1 ) {
    return false;
  }

  // Open the stream
  AVCodecContext* enc = &fmt_cxt_->streams[vid_index_]->codec;
  char buf[100];
  avcodec_string(buf, 100, enc, 0);
  vcl_cout<< "ffmpeg codec: " << buf<<vcl_endl;
  AVCodec* codec = avcodec_find_decoder(enc->codec_id);
  if ( !codec || avcodec_open( enc, codec ) < 0 ) {
    return false;
  }

  dump_format( fmt_cxt_, 0, fname.c_str(), 0 );
  close();

  return true;
}


//: Returns a clone of 'this' into which the given file is loaded.
//  This function creates a clone of 'this' (in order to allow
//  loading multiple videos at once) and loads the video
//  into the cloned codec. The cloned codec is the one that is returned
//  by this function.
vidl_codec_sptr
vidl_ffmpeg_codec::load(vcl_string const& fname, char mode)
{
  vidl_ffmpeg_codec *cloned_avi_codec = new vidl_ffmpeg_codec;

  if (!cloned_avi_codec->open(fname,mode)){
    delete cloned_avi_codec;
    return NULL;
  }

  return vidl_codec_sptr(cloned_avi_codec);
}


bool
vidl_ffmpeg_codec::open(vcl_string const& fname, char mode )
{
  // Close any currently opened file
  close();

  // Open the file
  int err;
  if ( ( err = av_open_input_file( &fmt_cxt_, fname.c_str(), NULL, 0, NULL ) ) != 0 ) {
    return false;
  }

  // Get the stream information by reading a bit of the file
  if ( av_find_stream_info( fmt_cxt_ ) < 0 ) {
    return false;
  }

  // Find a video stream. Use the first one we find.
  vid_index_ = -1;
  for ( int i = 0; i < fmt_cxt_->nb_streams; ++i ) {
    AVCodecContext *enc = &fmt_cxt_->streams[i]->codec;
    if ( enc->codec_type == CODEC_TYPE_VIDEO ) {
      vid_index_ = i;
      break;
    }
  }
  if ( vid_index_ == -1 ) {
    return false;
  }

  dump_format( fmt_cxt_, 0, fname.c_str(), 0 );

  // Open the stream
  AVCodecContext* enc = &fmt_cxt_->streams[vid_index_]->codec;
  AVCodec* codec = avcodec_find_decoder(enc->codec_id);
  if ( !codec || avcodec_open( enc, codec ) < 0 ) {
    return false;
  }

  if (enc->frame_rate>1000 && enc->frame_rate_base==1)
    enc->frame_rate_base=1000;

  vid_str_ = fmt_cxt_->streams[ vid_index_ ];
  frame_ = avcodec_alloc_frame();
  advance();

  //BITMAPINFOHEADER bh;
  //moviestream_->GetVideoFormat(&bh, sizeof(bh));
  this->set_width(vid_str_->codec.width);
  this->set_height(vid_str_->codec.height);
  this->set_bits_pixel(24); // always decodes to RGB byte images
  this->set_number_frames(count_frames());
  //this->set_format('L');
  //this->set_image_class('C');
  this->set_name(vul_file::basename(fname));
  this->set_description(fname);

  //frame_rate_=(double)moviestream_->GetLength()/moviestream_->GetLengthTime();

  return true;
}


void
vidl_ffmpeg_codec::close()
{
  if ( frame_ ) {
    av_free( frame_ );
    frame_ = 0;
  }

  vid_index_ = -1;
  if ( vid_str_ ) {
    avcodec_close( &vid_str_->codec );
    vid_str_ = 0;
  }
  if ( fmt_cxt_ ) {
    av_close_input_file( fmt_cxt_ );
    fmt_cxt_ = 0;
  }
}


vil_image_view_base_sptr
vidl_ffmpeg_codec::get_view( int position,
                         int x0, int xs,
                         int y0, int ys ) const
{
  int current = cur_frame_num();

  if (position == current);
  else if (position == current+1)
    advance();
  else
    seek(position);
  vil_image_view<vxl_byte> view = cur_frame();
  return new vil_image_view<vxl_byte>(vil_crop(view,x0,xs,y0,ys));
}


//: Read the current frame.
vil_image_view<vxl_byte>
vidl_ffmpeg_codec::cur_frame() const
{
  // If we've already converted this frame, try to convert it
  if ( !cur_img_ && frame_->data[0] != 0 ){
    int width = vid_str_->codec.width;
    int height = vid_str_->codec.height;

    // Deinterlace if requested
    if ( deinterlace_ ) {
      avpicture_deinterlace( (AVPicture*)frame_, (AVPicture*)frame_,
                             vid_str_->codec.pix_fmt, width, height );
    }

    // convert the current frame to RGB
    cur_img_ = vil_image_view<vxl_byte>( width, height, 1, 3 );
    AVPicture out_pict;
    out_pict.data[0] = cur_img_.top_left_ptr();
    out_pict.linesize[0] = cur_img_.ni() * 3;
    if ( img_convert( &out_pict, PIX_FMT_RGB24,
                     (AVPicture*)frame_, vid_str_->codec.pix_fmt,
                     width, height ) == -1 ) {
      cur_img_ = 0;
    }
  }

  // The MPEG 2 codec has a latency of 1 frame, so the dts of the last
  // packet (stored in last_dts) is actually the next frame's
  // dts.
  if ( vid_str_->codec.codec_id == CODEC_ID_MPEG2VIDEO &&
      vcl_string("avi") == fmt_cxt_->iformat->name ) {
    frame_number_offset_ = 1;
  }

  return cur_img_;
}


bool
vidl_ffmpeg_codec::put_view( int /*position*/,
                         const vil_image_view_base &/*im*/,
                         int /*x0*/, int /*y0*/)
{
  vcl_cerr << "vidl_ffmpeg_codec::put_view not implemented\n";
  return false;
}


//:
// \returns The frame number of the frame returned by cur_frame().
// \warning This number is meaningful only if cur_frame() is valid.
int
vidl_ffmpeg_codec::cur_frame_num() const
{
  return int( last_dts * vid_str_->r_frame_rate /
              vid_str_->r_frame_rate_base + AV_TIME_BASE/2 ) /
         AV_TIME_BASE - frame_number_offset_;
}


//: count the number of frames in the video.
// Scan through the video counting frames, but don't decode.
int
vidl_ffmpeg_codec::count_frames() const
{
  // remember the current video position
  int64_t timestamp = vid_str_->cur_dts;
  // seek back to the first frame
#if LIBAVFORMAT_BUILD <= 4616
  av_seek_frame( fmt_cxt_, vid_index_, 0);
#else
  av_seek_frame( fmt_cxt_, vid_index_, 0, 0 );
#endif
  AVPacket pkt;
  int frame_count=0;

  while ( av_read_frame( fmt_cxt_, &pkt ) >= 0) {
    if (pkt.stream_index==vid_index_)
      ++frame_count;
    av_free_packet( &pkt );
  }
  vcl_cout << "counted "<<frame_count<<" frames"<<vcl_endl;
  // seek back to last active position
#if LIBAVFORMAT_BUILD <= 4616
  av_seek_frame( fmt_cxt_, vid_index_, timestamp);
#else
  av_seek_frame( fmt_cxt_, vid_index_, timestamp, AVSEEK_FLAG_BACKWARD);
#endif

  return frame_count;
}


//:
// \return \c false if the end of the video is reached.
bool
vidl_ffmpeg_codec::advance() const
{
  // Quick return if the file isn't open.
  if ( !frame_ ) {
    return false;
  }

  AVPacket pkt;
  int got_picture = 0;

  while ( got_picture == 0 ) {
    if ( av_read_frame( fmt_cxt_, &pkt ) < 0 ) {
      break;
    }
    last_dts = pkt.dts;

    // Make sure that the packet is from the actual video stream.
    if (pkt.stream_index==vid_index_)
    {
      if ( vid_str_->codec.codec_id == CODEC_ID_RAWVIDEO ) {
        avpicture_fill( (AVPicture*)frame_, pkt.data,
          vid_str_->codec.pix_fmt,
          vid_str_->codec.width,
          vid_str_->codec.height );
        frame_->pict_type = FF_I_TYPE;
        got_picture = 1;
      } else {
        avcodec_decode_video( &vid_str_->codec,
          frame_, &got_picture,
          pkt.data, pkt.size );
      }
    }
    av_free_packet( &pkt );
  }

  // From ffmpeg apiexample.c: some codecs, such as MPEG, transmit the
  // I and P frame with a latency of one frame. You must do the
  // following to have a chance to get the last frame of the video.
  if ( !got_picture ) {
      avcodec_decode_video( &vid_str_->codec,
                            frame_, &got_picture,
                            NULL, 0 );
      last_dts += AV_TIME_BASE * vid_str_->r_frame_rate_base / vid_str_->r_frame_rate;
  }

  // The cached image is out of date, whether we managed to get a new
  // frame or not.
  cur_img_ = 0;

  if ( ! got_picture ) {
    frame_->data[0] = NULL;
  }

  return got_picture != 0;
}

//: The frame numbers are zero-based, so the first frame of the video is frame 0.
//
//  \param frame Seek so that the next cur_frame() returns frame \arg frame.
//  \return Returns \c false if the seek was unsuccessful.
//
bool
vidl_ffmpeg_codec::seek( unsigned frame ) const
{
  int64_t half_frame = int64_t(AV_TIME_BASE) * vid_str_->r_frame_rate_base / vid_str_->r_frame_rate / 2;
  int64_t req_timestamp = int64_t(frame)*AV_TIME_BASE * vid_str_->r_frame_rate_base / vid_str_->r_frame_rate + vid_str_->start_time;

  if ( req_timestamp > half_frame )
    req_timestamp -= half_frame;
  else
    req_timestamp = 0;
  
  // newer releases of ffmpeg may require a 4th argument to av_seek_frame
#if LIBAVFORMAT_BUILD <= 4616
  int seek = av_seek_frame( fmt_cxt_, vid_index_, frame );
#else
  int seek = av_seek_frame( fmt_cxt_, vid_index_, frame, 0 );
#endif
  if ( seek < 0 )
    return false;

  // We got to a key frame. Forward until we get to the frame we want.
  while ( true )
  {
    if ( ! advance() ) {
      return false;
    }
    if ( last_dts >= req_timestamp ) {
      if ( last_dts >= req_timestamp + 2*half_frame ) {
        vcl_cerr << "Warning: seek went into the future!\n";
        return false;
      }
      return true;
    }
  }
}

