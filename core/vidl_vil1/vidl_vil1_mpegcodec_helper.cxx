// This is core/vidl_vil1/vidl_vil1_mpegcodec_helper.cxx
#include "vidl_vil1_mpegcodec_helper.h"
#include <vidl_vil1/vidl_vil1_file_sequence.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h> // for memcpy
#include <vcl_cstdlib.h> // for exit()

#ifdef HAVE_IO_H
#include <fcntl.h>
#include <io.h>
#endif

vidl_vil1_mpegcodec_helper::vidl_vil1_mpegcodec_helper(vo_open_t * vopen,
                                             vcl_string filename,
                                             frame_buffer * buffers) :
  filename_(filename),output_open_(vopen)
{
  demux_track_ = 0;
  demux_pid_ = 0;
  disable_accel_ = 0;

  //use our passed function pointer to make
  //our homemade video out driver
  output_ = (vidl_vil1_mpegcodec_data*) vo_open (output_open_);
  output_->buffers = buffers;
  output_->output_format = vidl_vil1_mpegcodec_data::GREY;
  output_->last_frame_decoded = -2;

  in_file_ = 0;
  init_ = false;
  mpeg2dec_ = new mpeg2dec_t;
  decoder_routine = 0;
}

vidl_vil1_mpegcodec_helper::~vidl_vil1_mpegcodec_helper()
{
  vcl_cout << "vidl_vil1_mpegcodec_helper::~vidl_vil1_mpegcodec_helper. entering.\n";
  vo_close (output_);
  if (in_file_) 
  {
    in_file_->close();
    delete in_file_;
  }
  //mpeg2_close (mpeg2dec_);
  delete mpeg2dec_;

  vcl_cout << "vidl_vil1_mpegcodec_helper::~vidl_vil1_mpegcodec_helper. exiting.\n";
}

bool
vidl_vil1_mpegcodec_helper::init()
{
  if (init_) return true;

  //set up file pointer to mpeg file
  in_file_ = new vidl_vil1_file_sequence();
  in_file_->open(filename_.c_str());

  //set the acceleration. this doesn't work
  //right now.
  uint32_t accel;

  //need to find header file that defines mm_accel
  //FIX ME!!!
  //accel = disable_accel_ ? 0 : (mm_accel () | MM_ACCEL_MLIB);
  accel = disable_accel_ ? 0 : MM_ACCEL_MLIB;

  vo_accel (accel);

  mpeg2_init (mpeg2dec_, accel,output_);

  if (demux_pid_)
  {
    //transport stream
    chunk_size_ = 188;
    chunk_number_ = PACKETS;
    decoder_routine = &vidl_vil1_mpegcodec_helper::decode_ts;
  }
  else if (demux_track_)
  {
    //program stream
    chunk_size_ = 1;
    chunk_number_ = BUFFER_SIZE;
    decoder_routine = &vidl_vil1_mpegcodec_helper::decode_ps;
  }
  else
  {
    //elementary stream
    chunk_size_ = 1;
    chunk_number_ = BUFFER_SIZE;
    decoder_routine = &vidl_vil1_mpegcodec_helper::decode_es;
  }

  init_ = true;

  return true;
}

//this method acts on p, changing the state of its buf.
//returns a -1 if eof,0 is AOK, 1 otherwise.
int
vidl_vil1_mpegcodec_helper::execute(decode_request * p)
{
  if (!init_) init();

  if (p->rt == decode_request::REWIND)
  {
    in_file_->seek(0);
    output_->framenum = -2;
    output_->last_frame_decoded = -2;
    return 0;
  }

  output_->pending_decode = p;

  do
  {
    int reads = in_file_->read(buffer_, (chunk_size_*chunk_number_));
    if (reads != (chunk_number_*chunk_size_)) return -1;
    (this->*decoder_routine)(reads);
  } while ((!p->done) &&
           (p->rt != decode_request::FILE_GRAB) &&
           (p->rt != decode_request::SKIP));

  return 0;
}

void
vidl_vil1_mpegcodec_helper::decode_mpeg2 (uint8_t * buf, uint8_t * endb)
{
  int num_frames;

  num_frames = mpeg2_decode_data (mpeg2dec_, buf, endb);

  output_->last_frame_decoded += num_frames;
}

#define DEMUX_PAYLOAD_START 1
int
vidl_vil1_mpegcodec_helper::demux (uint8_t * buf, uint8_t * endb, int flags)
{
  static int mpeg1_skip_table[16] = {
    0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  // the demuxer keeps some state between calls:
  // if "state" = DEMUX_HEADER, then "head_buf" contains the first
  //   "bytes" bytes from some header.
  // if "state" == DEMUX_DATA, then we need to copy "bytes" bytes
  //   of ES data before the next header.
  // if "state" == DEMUX_SKIP, then we need to skip "bytes" bytes
  //   of data before the next header.
  //
  // NEEDBYTES makes sure we have the requested number of bytes for a
  // header. If we don't, it copies what we have into head_buf and returns,
  // so that when we come back with more data we finish decoding this header.
  //
  // DONEBYTES updates "buf" to point after the header we just parsed.

#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
  static int state = DEMUX_SKIP;
  static long int state_bytes = 0;
  static uint8_t head_buf[264];

  uint8_t * header;
  long int bytes;
  int len;

#define NEEDBYTES(x)                                 \
  do {                                               \
    long int missing = (x) - bytes;                  \
    if (missing > 0) {                               \
      if (header == head_buf) {                      \
        if (missing <= endb - buf) {                 \
          vcl_memcpy(header + bytes, buf, missing);  \
          buf += missing;                            \
          bytes = (x);                               \
        } else {                                     \
          vcl_memcpy(header + bytes, buf, endb-buf); \
          state_bytes = bytes + (endb - buf);        \
          return 0;                                  \
        }                                            \
      } else {                                       \
        vcl_memcpy(head_buf, header, bytes);         \
        state = DEMUX_HEADER;                        \
        state_bytes = bytes;                         \
        return 0;                                    \
      }                                              \
    }                                                \
  } while (false)

#define DONEBYTES(x)      \
  do {            \
    if (header != head_buf) \
      buf = header + (x); \
  } while (false)

  if (flags & DEMUX_PAYLOAD_START)
    goto payload_start;
  switch (state) {
  case DEMUX_HEADER:
    if (state_bytes > 0) {
      header = head_buf;
      bytes = state_bytes;
      goto continue_header;
    }
    break;
  case DEMUX_DATA:
    if (demux_pid_ || (state_bytes > endb - buf)) {
      decode_mpeg2 (buf, endb);
      state_bytes -= endb - buf;
      return 0;
    }
    decode_mpeg2 (buf, buf + state_bytes);
    buf += state_bytes;
    break;
  case DEMUX_SKIP:
    if (demux_pid_ || (state_bytes > endb - buf)) {
      state_bytes -= endb - buf;
      return 0;
    }
    buf += state_bytes;
    break;
  }

  while (1) {
    if (demux_pid_) {
      state = DEMUX_SKIP;
      return 0;
    }
  payload_start:
    header = buf;
    bytes = endb - buf;
  continue_header:
    NEEDBYTES (4);
    if (header[0] || header[1] || (header[2] != 1)) {
      if (demux_pid_) {
        state = DEMUX_SKIP;
        return 0;
      } else if (header != head_buf) {
        buf++;
        goto payload_start;
      } else {
        header[0] = header[1];
        header[1] = header[2];
        header[2] = header[3];
        bytes = 3;
        goto continue_header;
      }
    }
    if (demux_pid_) {
      if ((header[3] >= 0xe0) && (header[3] <= 0xef))
        goto pes;
      vcl_cerr << "bad stream id : " << header[3] << '\n';
      vcl_exit(1);
    }
    switch (header[3]) {
    case 0xb9: // program end code
      return 1;
    case 0xba: // pack header
      NEEDBYTES (12);
      if ((header[4] & 0xc0) == 0x40) { // mpeg2
        NEEDBYTES (14);
        len = 14 + (header[13] & 7);
        NEEDBYTES (len);
        DONEBYTES (len);
        // header points to the mpeg2 pack header
      } else if ((header[4] & 0xf0) == 0x20) { // mpeg1
        DONEBYTES (12);
        // header points to the mpeg1 pack header
      } else {
        vcl_cerr << "weird pack header\n";
        vcl_exit(1);
      }
      break;
    default:
      if (header[3] == demux_track_) {
      pes:
        NEEDBYTES (7);
        if ((header[6] & 0xc0) == 0x80) { // mpeg2
          NEEDBYTES (9);
          len = 9 + header[8];
          NEEDBYTES (len);
          // header points to the mpeg2 pes header
          if (header[7] & 0x80) {
            uint32_t pts;

            pts = (((buf[9] >> 1) << 30) |
                 (buf[10] << 22) | ((buf[11] >> 1) << 15) |
                 (buf[12] << 7) | (buf[13] >> 1));
            mpeg2_pts (mpeg2dec_, pts);
          }
        } else { // mpeg1
          int len_skip;
          uint8_t * ptsbuf;

          len = 7;
          while (header[len - 1] == 0xff) {
            len++;
            NEEDBYTES (len);
            if (len == 23) {
              vcl_cerr << "too much stuffing.\n";
              break;
            }
          }
          if ((header[len - 1] & 0xc0) == 0x40) {
            len += 2;
            NEEDBYTES (len);
          }
          len_skip = len;
          len += mpeg1_skip_table[header[len - 1] >> 4];
          NEEDBYTES (len);
          // header points to the mpeg1 pes header
          ptsbuf = header + len_skip;
          if (ptsbuf[-1] & 0x20) {
            uint32_t pts;

            pts = (((ptsbuf[-1] >> 1) << 30) |
                 (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
                 (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
            mpeg2_pts (mpeg2dec_, pts);
          }
        }
        DONEBYTES (len);
        bytes = 6 + (header[4] << 8) + header[5] - len;
        if (demux_pid_ || (bytes > endb - buf)) {
          decode_mpeg2 (buf, endb);
          state = DEMUX_DATA;
          state_bytes = bytes - (endb - buf);
          return 0;
        } else if (bytes > 0) {
          decode_mpeg2 (buf, buf + bytes);
          buf += bytes;
        }
      } else if (header[3] < 0xb9) {
        vcl_cerr << "looks like a video stream, not system stream\n";
        vcl_exit(1);
      } else {
        NEEDBYTES (6);
        DONEBYTES (6);
        bytes = (header[4] << 8) + header[5];
        if (bytes > endb - buf) {
          state = DEMUX_SKIP;
          state_bytes = bytes - (endb - buf);
          return 0;
        }
        buf += bytes;
      }
    }
  }
}

bool
vidl_vil1_mpegcodec_helper::decode_ps(int reads)
{
  return 0 != demux (buffer_, buffer_+reads, 0);
}

bool
vidl_vil1_mpegcodec_helper::decode_ts(int packets)
{
  uint8_t * buf;
  uint8_t * data;
  uint8_t * endb;
  int pid;

  for (int i = 0; i < packets; i++)
  {
    buf = buffer_ + i * 188;
    endb = buf + 188;
    if (buf[0] != 0x47)
    {
      vcl_cerr << "bad sync byte\n";
      return false;
    }
    pid = ((buf[1] << 8) + buf[2]) & 0x1fff;
    if (pid != demux_pid_)
      continue;
    data = buf + 4;
    if (buf[3] & 0x20)
    { // buf contains an adaptation field
      data = buf + 5 + buf[4];
      if (data > endb)
        continue;
    }
    if (buf[3] & 0x10)
      demux (data, endb, (buf[1] & 0x40) ? DEMUX_PAYLOAD_START : 0);
  }
  return true;
}

bool
vidl_vil1_mpegcodec_helper::decode_es(int reads)
{
  decode_mpeg2 (buffer_, buffer_+reads);
  return true;
}

void
vidl_vil1_mpegcodec_helper::print()
{
  vcl_cout << "about to print out decoder members.\n"
           << "frmt is: " << int(this->get_format()) << '\n'
           << "the last frame decoded is: " << output_->last_frame_decoded
           << "\nvidl_vil1_mpegcodec_helper::print. end.\n";
}
