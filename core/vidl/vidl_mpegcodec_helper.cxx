

#include "vidl_mpegcodec_helper.h"
#include <errno.h>
#include <getopt.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>

#ifdef HAVE_IO_H
#include <fcntl.h>
#include <io.h>
#endif

vidl_mpegcodec_helper::vidl_mpegcodec_helper(vo_open_t * vopen, 
				     vcl_string filename,
				     frame_buffer * buffers) : 
  _output_open(vopen),_filename(filename)
{
  _demux_track = 0;
  _demux_pid = 0;
  _disable_accel = 0;

  //use our passed function pointer to make
  //our homemade video out driver
  _output = (vidl_mpegcodec_data*) vo_open (_output_open);
  _output->buffers = buffers;
  _output->output_format = vidl_mpegcodec_data::GREY;
  _output->last_frame_decoded = -2;

  _in_file = 0;
  _init = false;
  _mpeg2dec = new mpeg2dec_t;
  decoder_routine = 0;

}
  
vidl_mpegcodec_helper::~vidl_mpegcodec_helper()
{
  vcl_cout << "vidl_mpegcodec_helper::~vidl_mpegcodec_helper. entering." << vcl_endl;
  vo_close (_output);
  vcl_fclose(_in_file);
  //mpeg2_close (_mpeg2dec);
  delete _mpeg2dec;
 
  vcl_cout << "vidl_mpegcodec_helper::~vidl_mpegcodec_helper. exiting." << vcl_endl;
}

bool
vidl_mpegcodec_helper::init()
{
  if (_init) return true;

  //set up file pointer to mpeg file
  _in_file = vcl_fopen(_filename.c_str(),"rb");
  
  //set the acceleration. this doesn't work
  //right now.
  uint32_t accel;

  //need to find header file that defines mm_accel
  //FIX ME!!!
  //accel = _disable_accel ? 0 : (mm_accel () | MM_ACCEL_MLIB);
  accel = _disable_accel ? 0 : MM_ACCEL_MLIB;

  vo_accel (accel);

  mpeg2_init (_mpeg2dec, accel,_output);

  if (_demux_pid)
    {
      //transport stream
      _chunk_size = 188;
      _chunk_number = PACKETS;
      decoder_routine = &vidl_mpegcodec_helper::decode_ts;
    }
  else if (_demux_track)
    {
      //program stream
      _chunk_size = 1;
      _chunk_number = BUFFER_SIZE;
      decoder_routine = &vidl_mpegcodec_helper::decode_ps;
    }
  else
    {
      //elementary stream
      _chunk_size = 1;
      _chunk_number = BUFFER_SIZE;
      decoder_routine = &vidl_mpegcodec_helper::decode_es;
    };

  _init = true;

  return true;
}

//this method acts on p, changing the state of its buf.
//returns a -1 if eof,0 is AOK, 1 otherwise.
int 
vidl_mpegcodec_helper::execute(decode_request * p)
{
  if (!_init) init();

  if (p->rt == decode_request::REWIND)
    {
      vcl_rewind(_in_file);
      _output->framenum = -2;
      _output->last_frame_decoded = -2;
      return 0;
    }

  _output->pending_decode = p;

  int reads;
  do 
    {
      reads = vcl_fread (_buffer, _chunk_size, _chunk_number, _in_file);
      if(reads != _chunk_number) return -1;;
      (this->*decoder_routine)(reads);
    } while ((!p->done) && 
	     (p->rt != decode_request::FILE_GRAB) &&
	     (p->rt != decode_request::SKIP));

  return 0;
}

void 
vidl_mpegcodec_helper::decode_mpeg2 (uint8_t * buf, uint8_t * end)
{
  int num_frames;

  num_frames = mpeg2_decode_data (_mpeg2dec, buf, end);

  _output->last_frame_decoded += num_frames;
}

#define DEMUX_PAYLOAD_START 1
int 
vidl_mpegcodec_helper::demux (uint8_t * buf, uint8_t * end, int flags)
{
  static int mpeg1_skip_table[16] = {
    0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

    /*
     * the demuxer keeps some state between calls:
     * if "state" = DEMUX_HEADER, then "head_buf" contains the first
     *     "bytes" bytes from some header.
     * if "state" == DEMUX_DATA, then we need to copy "bytes" bytes
     *     of ES data before the next header.
     * if "state" == DEMUX_SKIP, then we need to skip "bytes" bytes
     *     of data before the next header.
     *
     * NEEDBYTES makes sure we have the requested number of bytes for a
     * header. If we dont, it copies what we have into head_buf and returns,
     * so that when we come back with more data we finish decoding this header.
     *
     * DONEBYTES updates "buf" to point after the header we just parsed.
     */

#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
    static int state = DEMUX_SKIP;
    static int state_bytes = 0;
    static uint8_t head_buf[264];

    uint8_t * header;
    int bytes;
    int len;

#define NEEDBYTES(x)						\
    do {							\
	int missing;						\
								\
	missing = (x) - bytes;					\
	if (missing > 0) {					\
	    if (header == head_buf) {				\
		if (missing <= end - buf) {			\
		    memcpy (header + bytes, buf, missing);	\
		    buf += missing;				\
		    bytes = (x);				\
		} else {					\
		    memcpy (header + bytes, buf, end - buf);	\
		    state_bytes = bytes + end - buf;		\
		    return 0;					\
		}						\
	    } else {						\
		memcpy (head_buf, header, bytes);		\
		state = DEMUX_HEADER;				\
		state_bytes = bytes;				\
		return 0;					\
	    }							\
	}							\
    } while (0)

#define DONEBYTES(x)		\
    do {			\
	if (header != head_buf)	\
	    buf = header + (x);	\
    } while (0)

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
	if (_demux_pid || (state_bytes > end - buf)) {
	    decode_mpeg2 (buf, end);
	    state_bytes -= end - buf;
	    return 0;
	}
	decode_mpeg2 (buf, buf + state_bytes);
	buf += state_bytes;
	break;
    case DEMUX_SKIP:
	if (_demux_pid || (state_bytes > end - buf)) {
	    state_bytes -= end - buf;
	    return 0;
	}
	buf += state_bytes;
	break;
    }

    while (1) {
	if (_demux_pid) {
	    state = DEMUX_SKIP;
	    return 0;
	}
    payload_start:
	header = buf;
	bytes = end - buf;
    continue_header:
	NEEDBYTES (4);
	if (header[0] || header[1] || (header[2] != 1)) {
	    if (_demux_pid) {
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
	if (_demux_pid) {
	    if ((header[3] >= 0xe0) && (header[3] <= 0xef))
		goto pes;
	    vcl_cerr << "bad stream id : " << header[3] << vcl_endl;
	    exit (1);
	}
	switch (header[3]) {
	case 0xb9:	/* program end code */
	    /* DONEBYTES (4); */
	    /* break;         */
	    return 1;
	case 0xba:	/* pack header */
	    NEEDBYTES (12);
	    if ((header[4] & 0xc0) == 0x40) {	/* mpeg2 */
		NEEDBYTES (14);
		len = 14 + (header[13] & 7);
		NEEDBYTES (len);
		DONEBYTES (len);
		/* header points to the mpeg2 pack header */
	    } else if ((header[4] & 0xf0) == 0x20) {	/* mpeg1 */
		DONEBYTES (12);
		/* header points to the mpeg1 pack header */
	    } else {
		vcl_cerr << "weird pack header" << vcl_endl;
		exit (1);
	    }
	    break;
	default:
	    if (header[3] == _demux_track) {
	    pes:
		NEEDBYTES (7);
		if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
		    NEEDBYTES (9);
		    len = 9 + header[8];
		    NEEDBYTES (len);
		    /* header points to the mpeg2 pes header */
		    if (header[7] & 0x80) {
			uint32_t pts;

			pts = (((buf[9] >> 1) << 30) |
			       (buf[10] << 22) | ((buf[11] >> 1) << 15) |
			       (buf[12] << 7) | (buf[13] >> 1));
			mpeg2_pts (_mpeg2dec, pts);
		    }
		} else {	/* mpeg1 */
		    int len_skip;
		    uint8_t * ptsbuf;

		    len = 7;
		    while (header[len - 1] == 0xff) {
			len++;
			NEEDBYTES (len);
			if (len == 23) {
			    vcl_cerr << "too much stuffing." << vcl_endl;
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
		    /* header points to the mpeg1 pes header */
		    ptsbuf = header + len_skip;
		    if (ptsbuf[-1] & 0x20) {
			uint32_t pts;

			pts = (((ptsbuf[-1] >> 1) << 30) |
			       (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
			       (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
			mpeg2_pts (_mpeg2dec, pts);
		    }
		}
		DONEBYTES (len);
		bytes = 6 + (header[4] << 8) + header[5] - len;
		if (_demux_pid || (bytes > end - buf)) {
		    decode_mpeg2 (buf, end);
		    state = DEMUX_DATA;
		    state_bytes = bytes - (end - buf);
		    return 0;
		} else if (bytes > 0) {
		    decode_mpeg2 (buf, buf + bytes);
		    buf += bytes;
		}
	    } else if (header[3] < 0xb9) {
		vcl_cerr << "looks like a video stream, not system stream" << vcl_endl;
		exit (1);
	    } else {
		NEEDBYTES (6);
		DONEBYTES (6);
		bytes = (header[4] << 8) + header[5];
		if (bytes > end - buf) {
		    state = DEMUX_SKIP;
		    state_bytes = bytes - (end - buf);
		    return 0;
		}
		buf += bytes;
	    }
	}
    }
}

bool
vidl_mpegcodec_helper::decode_ps(int reads)
{
  return demux (_buffer, _buffer+reads, 0);
}

bool
vidl_mpegcodec_helper::decode_ts(int packets)
{
  uint8_t * buf;
  uint8_t * data;
  uint8_t * end;
  int pid;

  for (int i = 0; i < packets; i++) 
    {
      buf = _buffer + i * 188;
      end = buf + 188;
      if (buf[0] != 0x47) 
	{
	  vcl_cerr << "bad sync byte" << vcl_endl;
	  return false;
	}
      pid = ((buf[1] << 8) + buf[2]) & 0x1fff;
      if (pid != _demux_pid)
	continue;
      data = buf + 4;
      if (buf[3] & 0x20) 
	{	/* buf contains an adaptation field */
	  data = buf + 5 + buf[4];
	  if (data > end)
	    continue;
	}
      if (buf[3] & 0x10)
	demux (data, end, (buf[1] & 0x40) ? DEMUX_PAYLOAD_START : 0);
    }
  return true;
}

bool
vidl_mpegcodec_helper::decode_es(int reads)
{
  decode_mpeg2 (_buffer, _buffer+reads);
  return true;
}

void 
vidl_mpegcodec_helper::print()
{
  vcl_cout << "about to print out decoder members." << vcl_endl;
  vcl_cout << "frmt is: " << this->get_format() << vcl_endl;
  vcl_cout << "the last frame decoded is: " << _output->last_frame_decoded << vcl_endl;
  vcl_cout << "vidl_mpegcodec_helper::print. end." << vcl_endl;
}
