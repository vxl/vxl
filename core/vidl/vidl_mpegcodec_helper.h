#ifndef vidl_mpegcodec_helper_h
#define vidl_mpegcodec_helper_h

//:
// \file
// \author l.e.galup
// \date July 2002
// \brief   This is a rewrite of the mpeg2dec program. it has been modified
//          to work with the video player, i.e., by implementing a get_section
//          method. this required some buffering, and the ability to decode
//          a file grab at a time. there is some extra legacy code here.
//          though some things seem to serve no purpose, i left them there as hooks
//          for possible evolution of this class. 
// 
//          the mpeg_codec sends a request to this helper via
//          a decode_request. data is stored in the mpeg_codec_data class, which
//          holds a frame buffer, owned by the mpeg_codec. decoded frames are
//          stored in this frame_buffer, and subsequent user requests for frames
//          just does a memcpy from this frame_buffer into the user supplied buffer. 
// \verbatim
// Modifications
// \endverbatim

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

#define BUFFER_SIZE 4096
#define DEMUX_PAYLOAD_START 1
#define PACKETS (BUFFER_SIZE / 188)

extern "C" {
  typedef unsigned char uint8_t;
  typedef unsigned int uint32_t;
#define this c_this
#include <video_out.h>
#include <mpeg2.h>
#include <mm_accel.h>
#include <../libvo/video_out_internal.h>
#undef this
}

class decode_request 
{
 public:
  enum request_type
    {
      SEEK, FILE_GRAB, SKIP, REWIND
    };
  //seek seeks from the current file pointer 
  //position until it gets frame position
  //file grab does a single fread from the 
  //file of size BUFFER_SIZE

  request_type rt;
  int position; //which frame is desired
  int x0, y0, w, h; // (x0,y0) is upper left corner of roi
  bool done; //when this flag is set, the decoder stops
};

class frame_buffer
{
 public:

  frame_buffer()
    {
      _buffers = new vcl_map<int,unsigned char *>; 
    }

  void init(int width, int height, int bits_pixel)
    {
      //allocate mem 4 ring buffer
      for (int i=1; i<30 ; i++)
	{
	  unsigned char * buf = new unsigned char[width*height*(bits_pixel/8)];
	  (*_buffers)[-i] = buf;
	}
      return;
    }

  ~frame_buffer()
    {
      vcl_cout << "frame_buffer DTOR. entering." << vcl_endl;
      vcl_map<int,unsigned char *>::iterator vmiuit = _buffers->begin();
      for (;vmiuit != _buffers->end(); vmiuit++)
	delete[] (*vmiuit).second;
      delete _buffers;
      vcl_cout << "frame_buffer DTOR. exiting." << vcl_endl;
    }

  unsigned char * get_buff(int i) {return (*_buffers)[i];}
  unsigned char * next(int framenum)
  {
    //first, get the oldest frame in the buffer
    unsigned char * buf = (*_buffers->begin()).second;

    //remove it from the map
    _buffers->erase(_buffers->begin());

    //use the memory for the new frame
    (*_buffers)[framenum] = buf;

    return buf;
  }

  void print()
    {
      vcl_map<int,unsigned char *>::const_iterator vmiucit = _buffers->begin();
      for (;vmiucit != _buffers->end(); vmiucit++)
	vcl_cout << (*vmiucit).first << vcl_endl;
    }

  int first_frame_num() { return (*_buffers->begin()).first;}

  bool reset()
    {
      vcl_map<int,unsigned char *>::iterator vmit = _buffers->begin();
      for(int i=-30; vmit != _buffers->end(); vmit++,i++)
	{
	  unsigned char * buf = (*vmit).second;
	  
	  //remove it from the map
	  _buffers->erase(vmit);
	  
	  //use the memory for the new frame
	  (*_buffers)[i] = buf;
	}
    }

 private:
  vcl_map<int,unsigned char *> * _buffers;
};

struct vidl_mpegcodec_data : vo_instance_t
{
 public:
  enum output_format_t 
    {
      GREY, RGB
    };

  int prediction_index;
  vo_frame_t * frame_ptr[3]; //legacy code
  vo_frame_t frame[3]; //is the current frame
  int width;    //doesn't get set until the first frame gets decoded
  int height;   //doesn't get set until the first frame gets decoded
  int framenum; //current frame number. starts at -2.
                // this is because we use the first frame to find
                // the stats on the frame.
  char header[1024];
  decode_request * pending_decode; //request sent by mpeg_codec
  output_format_t output_format; //is either gray or rgb
  int last_frame_decoded; //this gets incremented after every fread
  frame_buffer * buffers;
};

//this class does the actual decoding.
class vidl_mpegcodec_helper
{
  friend class vidl_mpegcodec;

 public:
  vidl_mpegcodec_helper(vo_open_t * vopen, 
		    vcl_string filename, 
		    frame_buffer * buffers);
  ~vidl_mpegcodec_helper();
  bool init();
  int execute(decode_request * p);
  void print();
  int get_width() {return _output->width;}
  int get_height() {return _output->height;}
  int get_last_frame() {return _output->last_frame_decoded;}
  vidl_mpegcodec_data::output_format_t get_format() {return _output->output_format;}

 private:
  /////////////////////////////////////////////////////////
  // the following methods were lifted straight from
  // mpeg2dec-0.2.1.
  /////////////////////////////////////////////
  void decode_mpeg2 (uint8_t * current, uint8_t * end);
  int demux (uint8_t * buf, uint8_t * end, int flags);
  /////////////////////////////////////////////////////////////

  //these methods were factored out from the above
  //they correspond to the demultiplexing done
  //per stream protocol
  bool decode_ps(int reads);
  bool decode_ts(int packets);
  bool decode_es(int reads);

  // each of these arguments were static 
  // and global in the original implementation
  // of mpeg2dec
  uint8_t _buffer[BUFFER_SIZE];
  vcl_string _filename;
  FILE * _in_file;
  int _demux_track;
  int _demux_pid;
  int _disable_accel;
  mpeg2dec_t * _mpeg2dec;
  vo_open_t * _output_open;
  vidl_mpegcodec_data * _output;
  int _chunk_size;
  int _chunk_number;
  bool (vidl_mpegcodec_helper::*decoder_routine)(int);

  bool _init; //true once init() has completed
};

#endif // vidl_mpegcodec_helper
