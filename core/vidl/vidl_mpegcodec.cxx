#include "vidl_mpegcodec.h"
#include "vidl_yuv_2_rgb.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vul/vul_file.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb_byte.h>

// and now, for something completely different...
// ultimately all these static functions are used to make
// vo_vil_im_open, whose function pointer i will pass to
// vidl_mpegcodec_helper.
//
/////////////////////////////////////////////////////////////////////
//this copies the frame into the current frame buffer
// N.B. the decoder currently decodes the entire frame.
// hence, the width gotten from the instance variable
// below, the frame width. however, the width from
// the request is the roi width.
static void internal_draw_frame (vidl_mpegcodec_data * instance,
                                 vo_frame_t * frame,
                                 unsigned char * buf)
{
  uint8_t *Y = frame->base[0];
  uint8_t *U = frame->base[1];
  uint8_t *V = frame->base[2];
  decode_request * p = instance->pending_decode;

  int c = 0;
  int w = instance->width;

  int roix = p->x0;
  int roiy = p->y0;
  int roixend = roix + p->w;

  if (instance->output_format == vidl_mpegcodec_data::GREY)
    {
      int roiyend = roiy+p->h;

      // Recover in gray
      for (int i=roiy; i<(roiyend); ++i)
        for (int j=roix; j<(roixend); ++j, ++c)
          buf[c]= Y[i*p->w+j];
    }
  else
    {
      int roiyend = w*(roiy+p->h);

      // Recover in RGB
      for (int i=roiy; i<(roiyend); i+=w)
        for (int j=roix; j<(roixend); ++j, c+=3)
          {
            int arg = (i>>2)+(j>>1);
            // this is assuming the chroma channels are half-size in each direction.
            vidl_yuv_2_rgb(Y[i+j],
                           U[arg],
                           V[arg],
                           &(buf[c]));
          }
    }
  return;
}

static int internal_setup (vo_instance_t * instance_,
                           int width,
                           int height,
                           void (* draw_frame) (vo_frame_t *))
{
  vidl_mpegcodec_data * instance;

  instance = (vidl_mpegcodec_data *) instance_;

  instance->close = libvo_common_free_frames;
  instance->get_frame = libvo_common_get_frame;
  instance->width = width;
  instance->height = height;

  //sanity check.
  //i am assuming here that pending_decode has already been set
  decode_request * p = instance->pending_decode;
  if ((p->x0+p->w)>width) p->w = width - p->x0;
  if ((p->y0+p->h)>height) p->h = height - p->y0;

  int hh = height>>1;
  vcl_sprintf(instance->header, "P5\n\n%d %d\n255\n", width, hh * 3);
  return libvo_common_alloc_frames ((vo_instance_t *) instance,
                                    width,
                                    height,
                                    sizeof (vo_frame_t),
                                    0, 0, draw_frame);
}

static void vil_im_draw_frame (vo_frame_t * frame)
{
  vidl_mpegcodec_data * instance;

  instance = (vidl_mpegcodec_data *) frame->instance;
  int n = ++(instance->framenum);
  if ( n < 0)
      return;

  decode_request * p = instance->pending_decode;
  if (!p)
    {
      vcl_cerr << "vidl_mpegcodec. vil_im_draw_frame."
               << "decode request was never set\n";
      return;
    }

  frame_buffer * fb = instance->buffers;

  if (p->rt == decode_request::SEEK)
    {
      if (instance->framenum == p->position )
          p->done = true;

      internal_draw_frame (instance, frame, fb->next(n));
    }
  else if (p->rt == decode_request::FILE_GRAB)
      internal_draw_frame (instance, frame, fb->next(n));

  return;
}

//another callback, called by helper class
//after this is called, the client of this class
//should set the decode request
static int vil_im_setup (vo_instance_t * instance, int width, int height)
{
  return internal_setup (instance, width, height, vil_im_draw_frame);
}

// this method is a callback, called by the helper class
vo_instance_t * vo_vil_im_open (void)
{
  vidl_mpegcodec_data * instance;

  instance = new vidl_mpegcodec_data;

  //set call backs
  instance->setup = vil_im_setup;
  instance->framenum = -2;
  return (vo_instance_t *) instance;
}

//////////////////////////////////////////////////////////////////////////////
vidl_mpegcodec::vidl_mpegcodec()
{
  decoder_ = 0;
  buffers_ = new frame_buffer;
  inited = false;
  set_number_frames(-1);
}

vidl_mpegcodec::~vidl_mpegcodec()
{
  vcl_cout << "vidl_mpegcodec::~vidl_mpegcodec. entering" << vcl_endl;
  if (decoder_) decoder_->print();
  buffers_->print();
  vcl_cout << "first frame number in memory is: " << buffers_->first_frame_num() << vcl_endl;
  delete buffers_;
  if (decoder_) delete decoder_;

  vcl_cout << "vidl_mpegcodec::~vidl_mpegcodec. exiting" << vcl_endl;
}

void
vidl_mpegcodec::set_grey_scale(bool grey)
{
  if (!decoder_)
    {
      vcl_cout << "vidl_mpegcodec::set_gray_scale. need to load file first.\n";
      return;
    }

  if (grey) decoder_->output_->output_format = vidl_mpegcodec_data::GREY;
  else decoder_->output_->output_format = vidl_mpegcodec_data::RGB;
}

//this method is SUPPOSED to parse the header and determine stuff
//like height, width, number of frames, bits per pixel, etc. however,
//i don't have the time right now to write such a header. hopefully it
//will be done someday.
vidl_codec_sptr vidl_mpegcodec::load(const char* fname, char mode)
{
  vcl_string filename(fname);

  //just running probe here just to be safe,
  //though the client is supposed to run this anyway before
  //using this method.
  if (this->probe(fname))
    {
      decoder_ = new vidl_mpegcodec_helper(vo_vil_im_open,
                                           filename,
                                           buffers_);
      return this;
    }
  return 0;
}

bool
vidl_mpegcodec::get_section(int position,
                        void* ib,
                        int x0,
                        int y0,
                        int width,
                        int height) const
{
  assert(inited == true);

  int h = this->height();
  int w = this->width();

  //CASE 1:
  //if a frame is requested that is prior to what is
  //in the frame buffer, need to rewind and start all over.
  if (position < buffers_->first_frame_num())
    {
      decode_request req;
      req.rt = decode_request::REWIND;
      req.position = position;
      req.x0 = x0;
      req.y0 = y0;
      req.w = w;
      req.h = h;
      req.done = false;
      decoder_->execute(&req);
      buffers_->reset();

      req.rt = decode_request::SEEK;

      decoder_->execute(&req);
    }
  //CASE 2:
  //the requested frame is beyond what is in the frame buffer
  else if (position > decoder_->get_last_frame())
    {
      decode_request req;
      req.rt = decode_request::SEEK;
      req.position = position;
      req.x0 = x0;
      req.y0 = y0;
      req.w = w;
      req.h = h;
      req.done = false;
      decoder_->execute(&req);
    }
  //CASE 3: position requested is actually in frame buffer
  else
    {
      if ((decoder_->get_last_frame() - position) < 10)
        {
          decode_request req;
          req.rt = decode_request::FILE_GRAB;
          req.position = position;
          req.x0 = x0;
          req.y0 = y0;
          req.w = w;
          req.h = h;
          req.done = false;

          //grab a couple of times. why? because from experiment, this seems
          //to be the minimal number necessary to grab at least two frames.
          //this varies, of course, with roi size, stream protocol, etc.
          decoder_->execute(&req);
          decoder_->execute(&req);
        }
    }

  unsigned char * buf = buffers_->get_buff(position);
  vcl_memcpy(ib,(void *) buf,((this->get_bytes_pixel())*w*h));

  return true;
}

bool
vidl_mpegcodec::probe(const char* fname)
{
  vcl_string exten = vul_file::extension(fname);
  bool isthere = (vul_file::exists(fname)) && ((exten == ".mpeg") ||
                                               (exten == ".mpg")  ||
                                               (exten == ".mp2")  ||
                                               (exten == ".mp1"));

  return isthere;
}

void
vidl_mpegcodec::set_demux_video()
{
  decoder_->demux_track_ = 0xe0;
}

void
vidl_mpegcodec::set_pid(vcl_string pid)
{
  decoder_->demux_pid_ = vcl_strtol(pid.c_str(),0,16);
}

#if 0
vil_image *
vidl_mpegcodec::get_image(int frame_position,
                      int x0,
                      int y0,
                      int width,
                      int height)
{
  vil_image * frame = 0;

  int indy = width * height * this->get_bytes_pixel();
  unsigned char ib[indy];
  this->get_section(frame_position,(void*)ib,x0,y0,width,height);

  if (decoder_->get_format() == vidl_mpegcodec_data::GREY)
    frame = new vil_memory_image_of<unsigned char >(&ib[0],this->width(),this->height());
  else
    {
      int w = this->width();
      int h = this->height();
      vil_rgb_byte bites[w*h];
      int c=0;
      for (int i=0; i<(w*h); i++,c+=3)
        bites[i] = vil_rgb_byte(ib[c],ib[c+1],ib[c+2]);
      frame = new vil_memory_image_of<vil_rgb_byte >(&bites[0],w,h);
    }
  return frame;
}
#endif

//called by load method
//assumed the helper is already instantiated
bool
vidl_mpegcodec::init()
{
  if (inited) return true;

  //decode at least one
  //first make a proper request
  //this is done to get the true width and height
  //and total number of frames, all without
  //have to read the header.
  decode_request req;
  req.rt = decode_request::SKIP;
  req.position = 0;
  req.x0 = 0;
  req.y0 = 0;
  req.w = 1000;
  req.h = 1000;
  req.done = false;

  decoder_->init();

  //if the total number of frames is not manually
  //entered by this point, then decode the whole
  //bloody thing till the end. else, just decode
  //once to get the true width and height.
  if (this->length() == -1)
    {
      while (decoder_->execute(&req) != -1);
      this->set_number_frames(decoder_->get_last_frame());
    }
  else decoder_->execute(&req);

  req.rt = decode_request::REWIND;
  decoder_->execute(&req);

  //initialize codec members
  //first, truncate to our frame
  int w = decoder_->get_width();
  int h = decoder_->get_height();

  //i need to play nicey-nicey and set these variables
  //from the base class.
  int b;
  this->set_width(w);
  this->set_height(h);
  if (decoder_->get_format() == vidl_mpegcodec_data::RGB)
    b=24;
  else if (decoder_->get_format() == vidl_mpegcodec_data::GREY)
    b=8;
  set_bits_pixel(b);

  //allocate memory for frame buffer now
  buffers_->init(w,h,b);

 //get at least 30 frames in there
  req.rt = decode_request::SEEK;
  req.position = 20;
  req.x0 = 0;
  req.y0 = 0;
  req.w = w;
  req.h = h;
  req.done = false;

  decoder_->execute(&req);

  //clean up
  inited = true;
  return true;
}
