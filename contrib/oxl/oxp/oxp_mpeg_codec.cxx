// This is oxl/oxp/oxp_mpeg_codec.cxx
#include "oxp_mpeg_codec.h"
//:
//  \file

extern "C" {
  typedef unsigned char uint8_t;
  typedef unsigned int uint32_t;
#define this c_this
#include <mpeg2dec/video_out.h>
#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mm_accel.h>
#undef this
}

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

#include <vul/vul_file.h>

#include <oxp/oxp_yuv_to_rgb.h>
#include <oxp/oxp_bunch_of_files.h>
#include <oxp/oxp_vob_frame_index.h>

static bool verbose = false;

// predeclare mpeg callbacks
static void my_draw(vo_frame_t *frame);
static void my_field(vo_frame_t *, int);
static vo_frame_t *my_get_frame(vo_instance_t *self, int flags);
static void my_close(vo_instance_t *self);
static int my_setup(vo_instance_t *self, int width, int height);

// and other statics
static int demux(oxp_mpeg_codec_data* impl, unsigned char const *buf, unsigned char const *end, int flags);

struct oxp_mpeg_codec_data : vo_instance_t {
  struct decode_request {
    int frame;
    int x0, y0, w, h;
    unsigned char* buf;
    bool done;
  };
  enum output_format_t {
    grey, rgb
  };
  struct frame_plus_index : public vo_frame_t {
    int frame;
  };

  oxp_bunch_of_files fp;
  oxp_vob_frame_index idx;
  int w;
  int h;
  int frame_number;
  int demux_track;
  int demux_pid;
  output_format_t output_format;
  int ring_buffer_pos;
  frame_plus_index *ring_buffer[8];
  uint32_t accel;    // acceleration?
  mpeg2dec_t decoder;
  unsigned char (*ppm_frame)[3];
  decode_request* pending_decode;

  // oxp_mpeg_codec_data();

  bool seek_to_iframe_before(int desired);
  frame_plus_index *make_frame();
  void convert_frame(frame_plus_index* frame, decode_request* p);
  int decode_mpeg2(unsigned char const *start, unsigned char const *end);
  void destroy_frame(vo_frame_t *frame);
  void draw_grey(unsigned char *frame);
  void draw(unsigned char (*frame)[3]);

  void decode_at_least_one();
  bool decode_until_desired(int f,
                            void* buf,
                            int bbox_x0,
                            int bbox_y0,
                            int bbox_w,
                            int bbox_h);
};

//-----------------------------------------------------------------------------
oxp_mpeg_codec::oxp_mpeg_codec()
{
  impl_ = new oxp_mpeg_codec_data;

  impl_->pending_decode = 0;
  impl_->vo_instance_t::setup = &my_setup;
  impl_->vo_instance_t::close = &my_close;
  impl_->vo_instance_t::get_frame = &my_get_frame;

  for (int i=0; i<8; ++i)
    impl_->ring_buffer[i] = 0;
  impl_->ring_buffer_pos = 0;

  impl_->accel = /* MM_ACCEL_X86_MMX | */ MM_ACCEL_MLIB;
  vo_accel(impl_->accel);

  // initialize decoder.
  mpeg2_init(&impl_->decoder, impl_->accel, impl_);

  impl_->ppm_frame = 0;
  impl_->demux_track = 0;
  impl_->demux_pid = 0;
  impl_->output_format = oxp_mpeg_codec_data::rgb;
}

bool oxp_mpeg_codec_data::seek_to_iframe_before(int frame)
{
  int start_frame_index;
  int lba = idx.frame_to_lba_of_prev_I_frame(frame, &start_frame_index);
  // --lba; // lba's start from 1?
  typedef oxp_bunch_of_files::offset_t index_t;
  index_t byte = index_t(lba) * 2048;

  if (verbose)
    vcl_cerr << __FILE__ << ": seek_to_iframe_before: Frame " << frame
             << " -> Start at closest frame " << start_frame_index
             << ", LBA " << lba << ", byte " << byte << '\n';
  if (lba < 0)
  {
    vcl_cerr << __FILE__ << ": ERROR!\n";
    return false;
  }
  if (!fp.seek(byte))
  {
    vcl_cerr << "oxp_mpeg_codec_data::seek_to_iframe_before: ERROR!\n";
    return false;
  }
  frame_number = start_frame_index-1; // This is the frame we have "just finished decoding"...
  return true;
}

//: Decode at least one frame.
// Will call static routines below:
//   my_setup
//   my_get_frame
//   my_draw
void oxp_mpeg_codec_data::decode_at_least_one()
{
  int starting_frame_number = frame_number;
  int n;
  unsigned char buf[2048];
  while ((n = fp.read(buf, sizeof buf)) > 0) {
    unsigned char* start = &buf[0];
    unsigned char* end = start + n;
    if (demux_track)
    {
      if (::demux(this, start, end, 0))
        return; // hit program_end_code
    }
    else if (demux_pid)
      vcl_abort();
    else
      // plain old
      this->decode_mpeg2(start, end);

    if (frame_number > starting_frame_number)
      return;
  }
}

//: Run mpeg till you find frame f.
// Return false if that ain't gonna happen.
bool oxp_mpeg_codec_data::decode_until_desired(int f,
                                               void* buf,
                                               int bbox_x0,
                                               int bbox_y0,
                                               int bbox_w,
                                               int bbox_h)
{
  decode_request r;
  r.frame = f;
  r.buf = (unsigned char*)buf;
  r.x0 = bbox_x0;
  r.y0 = bbox_y0;
  r.w = bbox_w;
  r.h = bbox_h;
  r.done = false;

  // Check the ring buffer to see if any in there...
  for (int i = 0; i < 8; ++i)
    if (ring_buffer[i] && ring_buffer[i]->frame == f)
    {
      if (verbose)
        vcl_cerr << __FILE__ << ": decode_until_desired: Found frame " << f << " in ring buffer\n";
      convert_frame(ring_buffer[i], &r);
      r.done = true;
      return true;
    }

  // Not in ring buffer, only possible if f < frame_number
  if (f <= frame_number)
  {
    if (verbose)
      vcl_cerr << __FILE__ << ": decode_until_desired: Need to seek for " << f << '\n';
    return false;
  }

  // Not decoded, seek for it
  pending_decode = &r;

  unsigned char fbuf[2048];
  int n;
  while ((n = fp.read(fbuf, sizeof fbuf)) > 0)
  {
    unsigned char* start = &fbuf[0];
    unsigned char* end = start + n;
    if (demux_track)
    {
      if (::demux(this, start, end, 0))
        return r.done;  // hit program_end_code
    }
    else if (demux_pid)
      vcl_abort();
    else
      // plain old
      this->decode_mpeg2(start, end);

    if (r.done)
    {
      pending_decode = 0;
      return true;
    }
  }

  return false;
}

int oxp_mpeg_codec_data::decode_mpeg2(unsigned char const *start, unsigned char const *end)
{
  return mpeg2_decode_data(&decoder, const_cast<uint8_t*>(start), const_cast<uint8_t*>(end));
}

////// Callbacks to place in the vo_instance_t's struct.

static
int my_setup(vo_instance_t *self, int width, int height)
{
  if (verbose)
    vcl_cerr << __FILE__ << ": setup(" << width << ", " << height << ")\n";
  oxp_mpeg_codec_data *impl = (oxp_mpeg_codec_data*)self;
  impl->w = width;
  impl->h = height;

  impl->frame_number = 0;

  impl->ppm_frame = (uint8_t (*)[3]) vcl_malloc(3 * width * height);

  return 0;
}

static
void my_close(vo_instance_t *self)
{
  if (verbose) vcl_cerr << __FILE__ << ": close()\n";
  oxp_mpeg_codec_data *impl = (oxp_mpeg_codec_data*)self;

  vcl_free(impl->ppm_frame);
}

static
vo_frame_t *
my_get_frame(vo_instance_t *self, int flags)
{
#if 0
#define VO_TOP_FIELD 1
#define VO_BOTTOM_FIELD 2
#define VO_BOTH_FIELDS (VO_TOP_FIELD | VO_BOTTOM_FIELD)
#define VO_PREDICTION_FLAG 4
#endif
  if (verbose)
    vcl_cerr << __FILE__ << ": get(" << flags << ")\n";
  oxp_mpeg_codec_data *impl = (oxp_mpeg_codec_data*)self;

  if (! impl->ring_buffer[impl->ring_buffer_pos])
    impl->ring_buffer[impl->ring_buffer_pos] = impl->make_frame();

  vo_frame_t *frame = impl->ring_buffer[impl->ring_buffer_pos ++];
  if (impl->ring_buffer_pos >= 8)
    impl->ring_buffer_pos = 0;

  return frame;
}

// called by my_get_frame callback to make space in the ring buffer
oxp_mpeg_codec_data::frame_plus_index*
oxp_mpeg_codec_data::make_frame()
{
  frame_plus_index *frame = (frame_plus_index*) vcl_malloc(sizeof(frame_plus_index));
  if (verbose)
    vcl_cerr << __FILE__ << ": make_frame() : frame = " << frame << '\n';
  frame->base[0] = (uint8_t*) vcl_malloc(    w * h    );
  frame->base[1] = (uint8_t*) vcl_malloc((w>>1)*(h>>1));
  frame->base[2] = (uint8_t*) vcl_malloc((w>>1)*(h>>1));
  frame->copy = 0; // my_copy;
  frame->field = my_field;
  frame->draw = my_draw;
  frame->instance = this;
  frame->frame = -1;
  return frame;
}

#if 0
static
void my_copy(vo_frame_t *f, uint8_t **b)
{
  if (verbose) vcl_printf("my_copy()\n");
}
#endif

static
void my_field(vo_frame_t *, int)
{
  if (verbose) vcl_cout << "my_field()\n";
}

static
void my_draw(vo_frame_t *frame_p)
{
  if (verbose)
    vcl_cout << "draw: frame = " << frame_p << "\nmy_draw()\n";
  oxp_mpeg_codec_data *impl = (oxp_mpeg_codec_data*)frame_p->instance;
  oxp_mpeg_codec_data::frame_plus_index* frame =
    static_cast<oxp_mpeg_codec_data::frame_plus_index*>(frame_p);

  ++impl->frame_number;
  frame->frame = impl->frame_number;

  oxp_mpeg_codec_data::decode_request* p = impl->pending_decode;
  if (!p)
    // nothing to decode, return.
    return;

  // got pending_decode from decode_until_desired
  if (p->frame == impl->frame_number)
  {
    if (verbose)
      vcl_cerr << __FILE__ << ": Found " << impl->frame_number << '\n';
    impl->convert_frame(frame, p);
    p->done = true;
  }
  else if (verbose)
  {
    if (impl->frame_number < p->frame)
      vcl_cerr << __FILE__ << ": Skipping " << impl->frame_number << " waiting for " << p->frame << '\n';
    else
      vcl_cerr << __FILE__ << ": Queuing " << impl->frame_number << " having got " << p->frame << '\n';
  }
}

void oxp_mpeg_codec_data::convert_frame(frame_plus_index* frame,
                                        decode_request* p)
{
  // base[0] : luminance Y
  // base[1] : chroma 1  U
  // base[2] : chroma 2  V
  uint8_t *Y = frame->base[0];
  uint8_t *U = frame->base[1];
  uint8_t *V = frame->base[2];

  if (this->output_format == oxp_mpeg_codec_data::grey)
  {
    // Recover in gray
    //int w = this->w;
    //int h = this->h;
    //unsigned char (*ppm_frame)[3] = this->ppm_frame;

    int c = 0;
    for (int i=p->y0; i<p->h; ++i)
      for (int j=p->x0; j<p->w; ++j, ++c)
        // this is assuming the chroma channels are half-size in each direction.
        p->buf[c]= Y[i*p->w+j];
  }
  else
  {
    // Recover in RGB
    //int w = this->w;
    //int h = this->h;

    int c = 0;
    for (int i=p->y0; i<p->h; ++i)
      for (int j=p->x0; j<p->w; ++j, c+=3)
        // this is assuming the chroma channels are half-size in each direction.
        oxp_yuv_to_rgb(Y[i*w+j],
                       U[(i>>1)*(p->w>>1)+(j>>1)],
                       V[(i>>1)*(p->w>>1)+(j>>1)],
                       &p->buf[c]);
  }
}


void oxp_mpeg_codec_data::destroy_frame(vo_frame_t *frame)
{
  assert(frame->instance == this);
  vcl_free(frame->base[0]); frame->base[0] = (uint8_t*)0xDEADBEEF;
  vcl_free(frame->base[1]); frame->base[1] = (uint8_t*)0xDEADBEEF;
  vcl_free(frame->base[2]); frame->base[2] = (uint8_t*)0xDEADBEEF;
  vcl_free(frame);
}

//-----------------------------------------------------------------------------
oxp_mpeg_codec::~oxp_mpeg_codec()
{
  if (impl_)
  {
    // close();
    vcl_cerr << "oxp_mpeg_codec: WARNING: deleting before close() was called\n";
    // You can't call close from within here because it may be being destroyed
    // statically, and the mpeg2_close call will segv.
    // So the options are segv or unflushed input.  segv is
    // incontrovertibly wrong, so we make sure that doesn't happen.
    for (int i=0; i<8; ++i)
      if (impl_->ring_buffer[i])
      {
        impl_->destroy_frame(impl_->ring_buffer[i]);
        impl_->ring_buffer[i] = 0;
      }

    delete impl_;
    impl_ = 0;
  }
}

//-----------------------------------------------------------------------------
void oxp_mpeg_codec::close()
{
  if (impl_)
  {
    // destroy decoder.
    mpeg2_close(&impl_->decoder);

    // close output object.
    vo_close(impl_);

    for (int i=0; i<8; ++i)
      if (impl_->ring_buffer[i])
      {
        impl_->destroy_frame(impl_->ring_buffer[i]);
        impl_->ring_buffer[i] = 0;
      }

    delete impl_;
    impl_ = 0;
  }
}


//-----------------------------------------------------------------------------
bool oxp_mpeg_codec::get_section(int position, // position of the frame in the stream
                                 void* ib, // To receive the datas
                                 int x0, // starting x
                                 int y0, // starting y
                                 int xs, // row size
                                 int ys) const // col size
{
  position += 2; //awf not sure if this is where the offset happens.

  const int FRAMES_TO_FFWD_RATHER_THAN_SEEK = 20;
  if (impl_->frame_number < position + 8 &&
      impl_->frame_number > position - FRAMES_TO_FFWD_RATHER_THAN_SEEK)
    if (impl_->decode_until_desired(position, ib, x0, y0, xs, ys))
      return true;

  // Didn't find the frame, seek and find.
  if (!impl_->seek_to_iframe_before(position))
    return false;

  return impl_->decode_until_desired(position, ib, x0, y0, xs, ys);
}


//: put_section not implemented yet.
// We may need to change make_dib to
// be able to put a section different
// of the entire frame.
int oxp_mpeg_codec::put_section(int position,
                                void* ib,
                                int x0, int y0,
                                int xs, int ys)
{
  vcl_cerr << "oxp_mpeg_codec::put_section not implemented\n";
  return -1;
}

//-----------------------------------------------------------------------------
//: probe the file fname, open it as an MPEG file. If this works, close it and return true. False otherwise.

bool oxp_mpeg_codec::probe(const char* fname)
{
  if (verbose)
    vcl_cerr << "oxp_mpeg_codec::probe[" << fname << "]\n";

  // 1st try to open
  if (vcl_FILE* fp = vcl_fopen(fname, "rb"))
  {
    unsigned int buf = 0xffffffffu;
    vcl_fread(&buf, 1, 4, fp);
    vcl_fclose(fp);

    bool ok = false;
    if (buf == 0x000001b3 || buf == 0xb3010000)
      ok=true; // mpeg
    if (buf == 0x000001ba || buf == 0xba010000)
      ok=true; // vob

    if (ok) // Try to find an idx
      return true;
  }

  vcl_string fn(fname);

  bool p = (vul_file::size((fn + ".lst").c_str()) > 0);
  if (verbose)
    vcl_cerr << "oxp_mpeg_codec::probe[" << fname << "] -> " << (p ? "true" : "false") << '\n';
  return p;
}

bool oxp_mpeg_codec::load(const char* fname, char mode)
{
  // 1st try to open
  bool is_mpeg = false;
  bool is_vob = false;
  {
    vcl_FILE* fp = vcl_fopen(fname, "rb");
    if (fp)
    {
      unsigned int buf = 0xffffffffu;
      vcl_fread(&buf, 1, 4, fp);
      vcl_fclose(fp);

      if (buf == 0x000001b3 || buf == 0xb3010000)
        is_mpeg = true; // mpeg
      if (buf == 0x000001ba || buf == 0xba010000)
        is_vob = true; // vob
    }
  }

  if (is_mpeg || is_vob)
  {
    impl_->fp.open_1(fname);
    impl_->decode_at_least_one();

    // Try to find an idx
    char buf[1024];
    vcl_strcpy(buf, fname);
    char* p = vcl_strrchr(buf, '.');
    if (!p) // No . in filename
      p = buf + vcl_strlen(buf)-1;
    vcl_strcpy(p, ".idx");
    vcl_cerr << "Trying index file [" << buf << "] ... ";
    if (vul_file::size(buf) > 0)
    {
      vcl_cerr << " loading ...";
      impl_->idx.load(buf);
    }
    else
    {
      vcl_cerr << " not present, will not be able to seek\n";
      impl_->idx.add(0, 0);
    }

    // Set demux if vob
    impl_->demux_track = is_vob ? 0xe0 : 0;
  }
  else
  {
    // Open fname, if a vob, set_demux
    vcl_string fn = fname;

    impl_->fp.open((fn + ".lst").c_str());

    impl_->decode_at_least_one();

    impl_->idx.load((fn + ".idx").c_str());

    impl_->demux_track = 0xe0;
  }

  return true;
}

//: Call this before calling decode on a multiplexed stream.
// Pulls the 0xe0 stream.
void oxp_mpeg_codec::set_demux_video()
{
  impl_->demux_track = 0xe0;
}

void oxp_mpeg_codec::set_output_format_grey()
{
  impl_->output_format = oxp_mpeg_codec_data::grey;
}

void oxp_mpeg_codec::set_output_format_rgb()
{
  impl_->output_format = oxp_mpeg_codec_data::rgb;
}

#if 0
void my_decoder::dummy_chunk()
{
  // I don't now what 0xB1 is for but libmpeg2 ignores it.
  static unsigned char const dummy[] = { 0, 0, 1, 0xB1 };
  decode(dummy, dummy + sizeof dummy);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#define DEMUX_PAYLOAD_START 1
static int demux(oxp_mpeg_codec_data* impl, unsigned char const *buf, unsigned char const *end, int flags)
{
  static int mpeg1_skip_table[16] = { 0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  // the demuxer keeps some state between calls:
  // if "state" = DEMUX_HEADER, then "head_buf" contains the first
  //     "bytes" bytes from some header.
  // if "state" == DEMUX_DATA, then we need to copy "bytes" bytes
  //     of ES data before the next header.
  // if "state" == DEMUX_SKIP, then we need to skip "bytes" bytes
  //     of data before the next header.
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
  static int state_bytes = 0;
  static uint8_t head_buf[264];

  uint8_t * header;
  int bytes;
  int len;

#define NEEDBYTES(x)                                  \
  do {                                                \
    int missing = (x) - bytes;                        \
    if (missing > 0) {                                \
      if (header == head_buf) {                       \
        if (missing <= end - buf) {                   \
          vcl_memcpy(header + bytes, buf, missing);   \
          buf += missing;                             \
          bytes = (x);                                \
        }                                             \
        else {                                        \
          vcl_memcpy(header + bytes, buf, end - buf); \
          state_bytes = bytes + end - buf;            \
          return 0;                                   \
        }                                             \
      }                                               \
      else {                                          \
        vcl_memcpy(head_buf, header, bytes);          \
        state = DEMUX_HEADER;                         \
        state_bytes = bytes;                          \
        return 0;                                     \
      }                                               \
    }                                                 \
  } while (false)

#define DONEBYTES(x)        \
  do {                      \
    if (header != head_buf) \
      buf = header + (x);   \
  } while (false)

  // demux routine starts here.  above is var and fn defns.
  if (flags & DEMUX_PAYLOAD_START)
    goto payload_start;
  switch (state) {
  case DEMUX_HEADER:
    if (state_bytes > 0)
    {
      header = head_buf;
      bytes = state_bytes;
      goto continue_header;
    }
    break;
  case DEMUX_DATA:
    if (impl->demux_pid || (state_bytes > end - buf))
    {
      impl->decode_mpeg2(buf, end);
      state_bytes -= end - buf;
      return 0;
    }
    impl->decode_mpeg2(buf, buf + state_bytes);
    buf += state_bytes;
    break;
  case DEMUX_SKIP:
    if (impl->demux_pid || (state_bytes > end - buf))
    {
      state_bytes -= end - buf;
      return 0;
    }
    buf += state_bytes;
    break;
  }

  while (true) {
    if (impl->demux_pid)
    {
      state = DEMUX_SKIP;
      return 0;
    }
  payload_start:
    header = const_cast<unsigned char*>(buf); // bletcherous const_cast -- it will write into the user's space.
    bytes = end - buf;
  continue_header:
    NEEDBYTES(4);
    if (header[0] || header[1] || (header[2] != 1))
    {
      if (impl->demux_pid)
      {
        state = DEMUX_SKIP;
        return 0;
      }
      else if (header != head_buf)
      {
        buf++;
        goto payload_start;
      }
      else
      {
        header[0] = header[1];
        header[1] = header[2];
        header[2] = header[3];
        bytes = 3;
        goto continue_header;
      }
    }
    if (impl->demux_pid)
    {
      if ((header[3] >= 0xe0) && (header[3] <= 0xef))
        goto pes;
      vcl_cerr << "bad stream id " << header[3] << '\n';
      vcl_exit(1);
    }
    switch (header[3]) {
    case 0xb9: // program end code
      // DONEBYTES(4);
      // break;
      return 1;
    case 0xba: // pack header
      NEEDBYTES(12);
      if ((header[4] & 0xc0) == 0x40) // mpeg2
      {
        NEEDBYTES(14);
        len = 14 + (header[13] & 7);
        NEEDBYTES(len);
        DONEBYTES(len);
        // header points to the mpeg2 pack header
      }
      else if ((header[4] & 0xf0) == 0x20) // mpeg1
      {
        DONEBYTES(12);
        // header points to the mpeg1 pack header
      }
      else
      {
        vcl_cerr << "weird pack header\n";
        vcl_exit(1);
      }
      break;
    default:
      if (header[3] == impl->demux_track)
      {
      pes:
        NEEDBYTES(7);
        if ((header[6] & 0xc0) == 0x80) // mpeg2
        {
          NEEDBYTES(9);
          len = 9 + header[8];
          NEEDBYTES(len);
          // header points to the mpeg2 pes header
        }
        else // mpeg1
        {
          len = 7;
          while ((header-1)[len] == 0xff) {
            len++;
            NEEDBYTES(len);
            if (len == 23)
            {
              vcl_cerr << "too much stuffing\n";
              break;
            }
          }
          if (((header-1)[len] & 0xc0) == 0x40)
          {
            len += 2;
            NEEDBYTES(len);
          }
          len += mpeg1_skip_table[(header - 1)[len] >> 4];
          NEEDBYTES(len);
          // header points to the mpeg1 pes header
        }
        DONEBYTES(len);
        bytes = 6 + (header[4] << 8) + header[5] - len;
        if (impl->demux_pid || (bytes > end - buf))
        {
          impl->decode_mpeg2(buf, end);
          state = DEMUX_DATA;
          state_bytes = bytes - (end - buf);
          return 0;
        }
        else if (bytes <= 0)
          continue;
        impl->decode_mpeg2(buf, buf + bytes);
        buf += bytes;
      }
      else if (header[3] < 0xb9)
      {
        vcl_cerr << "looks like a video stream, not system stream\n";
        vcl_exit(1);
      }
      else
      {
        NEEDBYTES(6);
        DONEBYTES(6);
        bytes = (header[4] << 8) + header[5];
        if (bytes > end - buf)
        {
          state = DEMUX_SKIP;
          state_bytes = bytes - (end - buf);
          return 0;
        }
        buf += bytes;
      }
    }
  }
}

#if 0
static void ts_loop(void)
{
#define PACKETS (BUFFER_SIZE / 188)
  int packets;
  do {
    packets = vcl_fread(buffer, 188, PACKETS, in_file);
    for (int i = 0; i < packets; i++)
    {
      uint8_t * buf = buffer + i * 188;
      uint8_t * end = buf + 188;
      if (buf[0] != 0x47)
      {
        vcl_cerr << "bad sync byte\n";
        vcl_exit(1);
      }
      int pid = ((buf[1] << 8) + buf[2]) & 0x1fff;
      if (pid != impl->demux_pid)
        continue;
      uint8_t * data = buf + 4;
      if (buf[3] & 0x20) // buf contains an adaptation field
      {
        data = buf + 5 + buf[4];
        if (data > end)
          continue;
      }
      if (buf[3] & 0x10)
        demux(data, end, (buf[1] & 0x40) ? DEMUX_PAYLOAD_START : 0);
    }
  } while (packets == PACKETS);
}
#endif

int oxp_mpeg_codec::get_width() const
{
  return impl_->w;
}

int oxp_mpeg_codec::get_height() const
{
  return impl_->h;
}
